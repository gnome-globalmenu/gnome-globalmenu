#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<IPCClient>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gdk/gdkx.h>
#include "ipc.h"
#include "ipcutils.h"
#include "ipcclient.h"
#include "ipccommand.h"



/**
 * Section: ipcclient
 *
 *
 * This is `ipcclient`. A client for IPCX (ipc on top of X). 
 *
 * Caution: This is not thread safe. Every application should call
 * ipc_client_start at most once.
 *
 */

static GdkWindow * 
	client_window = NULL; /* The window for communication*/
static gboolean 
	client_frozen = TRUE; /* Is the client frozen? Unused*/
static GdkNativeWindow 
	server = 0;			/* The server window*/
static gboolean 
	started = FALSE; 	/*did last ipc_client_start success?*/
static IPCClientServerDestroyNotify 
	server_destroy_notify = NULL; /*Called when the server is destroyed*/
static gpointer 
	server_destroy_notify_data = NULL; /*data for the callback*/
static GList * 
	queue = NULL; /* Queue for unsent commands, unused.*/
static gchar * 
	cid = NULL; 	/*Client ID (cid) obtained after NEGO*/
static gboolean 
	in_transaction = FALSE;  /*Are we in transaction?*/
static GList * 
	transaction = NULL; /*List of IPCCommand for this transaction*/

static GData * 
	event_handler_list = NULL; /* DataList of EventHandlerInfo.
									'key' is the name of the event*/
/**
 * EventHandlerInfo: 
 * 	@handler:	Callback
 * 	@data:		data passed to the callback
 * 	
 * internal data keeping track of the event handlers.
 * */
typedef struct {
	IPCClientEventHandler handler;
	gpointer data;
	GData * filter;
} EventHandlerInfo;
static void event_handler_info_free(EventHandlerInfo * info){
	g_datalist_clear(&info->filter);
	g_slice_free(EventHandlerInfo, info);
}

static gpointer ipc_client_wait_for_property(GdkAtom property_name, gboolean remove);
/**
 * server_filter:
 *
 * Filters the server events. Only XDestroyWindowEvent is interested in. When that event
 * happends, cleanup and invoke the callback.
 *
 * FIXME: the cleanup is dirty. 
 * 	
 * 	1. The handlers are not freed.
 * 	2. Inner state is not consistent. (i.e. perhaps can't call start safely)
 */
static GdkFilterReturn server_filter(GdkXEvent * xevent, GdkEvent * event, gpointer data){
	if(((XEvent *)xevent)->type == DestroyNotify) {
		XDestroyWindowEvent * dwe = (XDestroyWindowEvent *) xevent;
		g_critical("Server is down!");
		started = FALSE;
		if(server_destroy_notify)
			server_destroy_notify(server_destroy_notify_data);
		server = 0;
	} else {
	}
	return GDK_FILTER_CONTINUE;
}
/**
 * client_message_event:
 *
 * Invoked by default_filter when IPCClient receives only
 * events from the server.
 * */
static void client_message_event(XClientMessageEvent * client_message) {
	gchar * event_data = ipc_client_wait_for_property(IPC_PROPERTY_EVENT, TRUE);
	if(!event_data) {
		g_critical("can't obtain event data");	
		return;
	}
	IPCEvent * event = ipc_event_parse(event_data);
	XFree(event_data);
	if(!event) {
		g_critical("malformed event data");
		return;
	}
	EventHandlerInfo * info = g_datalist_get_data(&event_handler_list, event->name);
	if(!info) {
		g_critical("no handlers for this event is set");
		ipc_event_free(event);
		return;
	}
	if(info->handler) 
		info->handler(event, info->data);
	ipc_event_free(event);
}
/**
 * default_filter:
 *
 * Default dispatcher of XClientMessageEvent.
 */
static GdkFilterReturn default_filter (GdkXEvent * xevent, GdkEvent * event, gpointer data){
	XClientMessageEvent * client_message = (XClientMessageEvent *) xevent;
	switch(((XEvent *)xevent)->type) {
		case ClientMessage:
			if(client_message->message_type == gdk_x11_atom_to_xatom(IPC_CLIENT_MESSAGE_EVENT)) {
				client_message_event(client_message);
				return GDK_FILTER_REMOVE;
			}
		return GDK_FILTER_CONTINUE;
	}
	return GDK_FILTER_CONTINUE;
}
/**
 * ipc_client_send_client_message:
 * 	@message_type: type of the message (IPC_CLIENT_MESSAGE_XXX
 * 
 * send a client message to the server.
 * */
static void ipc_client_send_client_message(GdkAtom message_type) {
	ipc_send_client_message(GDK_WINDOW_XWINDOW(client_window), server, message_type);
}
/**
 * ipc_client_wait_for_property:
 * 	@property_name: the property to wait for.
 * 	@remove: remove it after obtained?
 *
 * This function wait block the current thread,
 * until the given property is obtained from client_window,
 * Or an error occurs.
 *
 * This feature ensures the client and server are in sync.
 *
 * Use XFree to free the result.
 */
static gpointer ipc_client_wait_for_property(GdkAtom property_name, gboolean remove) {
	return ipc_wait_for_property(GDK_WINDOW_XWINDOW(client_window), property_name, remove);
}
/**
 * ipc_client_started:
 *
 * Returns: whether the client is started.
 */
gboolean ipc_client_started(){
	return started;
}
/**
 * ipc_client_start:
 * 	@notify: the callback when the server is destroy.
 * 	@data: data passed to the callback
 *
 * 	Start an IPCClient. 
 *
 * 	The IPCClient is not thread safe and shall be started
 * 	only once per application.
 *
 * 	If a server is not found, this function will try to spawn a new server
 * 	from the environment variable GNOMENU_SERVER.
 *
 * Returns: FALSE if it fails.
 */
gboolean ipc_client_start(IPCClientServerDestroyNotify notify, gpointer data){
	GdkWindow * server_gdk = NULL;
	server = ipc_find_server();
	if(server == 0) {
		gchar * server_bin = g_getenv("GNOMENU_SERVER");
		if(!server_bin) server_bin = "./gnomenu-server";
		GError * error = NULL;
		LOG("Spawning the server: %s", server_bin);
		if(!g_spawn_command_line_async(server_bin, &error)){
			g_critical("could not start the server.");
			if(error){
				g_critical("%s", error->message);
				g_error_free(error);
			}
			goto no_server;
		} else {
			int i = 0;;
			while(server == 0 || server_gdk == NULL){
				g_usleep(1000*100);
				server = ipc_find_server();
				server_gdk = gdk_window_lookup(server);
				if(!server_gdk) server_gdk = gdk_window_foreign_new(server);
			}
		}
	}

	gdk_x11_grab_server();
	server = ipc_find_server();
	server_gdk = gdk_window_lookup(server);
	if(!server_gdk) server_gdk = gdk_window_foreign_new(server);
	LOG("Server Found at = %p", server);
	gdk_window_set_events(server_gdk, gdk_window_get_events(server_gdk) | GDK_STRUCTURE_MASK);
	gdk_window_add_filter(server_gdk, server_filter, NULL);
	server_destroy_notify = notify;
	server_destroy_notify_data = data;
	GdkWindowAttr attr;
	attr.title = IPC_CLIENT_TITLE;
	attr.wclass = GDK_INPUT_ONLY;
	client_window = gdk_window_new(NULL, &attr, GDK_WA_TITLE);
	gdk_window_add_filter(client_window, default_filter, NULL);
	client_frozen = FALSE;

	gdk_x11_ungrab_server();
	ipc_client_send_client_message(IPC_CLIENT_MESSAGE_NEGO);
	gchar * identify = ipc_client_wait_for_property(IPC_PROPERTY_CID, FALSE);
	if(identify) {
		cid = g_strdup(identify);
		LOG("CID obtained: %s", cid);
		XFree(identify);
		g_datalist_init(&event_handler_list);
		started = TRUE;
	} else {
		started = FALSE;
	}
no_server:
	return started;
}
/**
 * ipc_client_call_list:
 *
 * performance a transaction, send a command list.
 */
static GList * ipc_client_call_list(GList * command_list) {
	GList * ret = NULL;
	g_return_val_if_fail(started, NULL);
	gchar * data = ipc_command_list_to_string(command_list);
	g_assert(client_window);

	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	gdk_error_trap_push();
	
	XChangeProperty(display,
			GDK_WINDOW_XWINDOW(client_window),
			gdk_x11_atom_to_xatom(IPC_PROPERTY_CALL),
			gdk_x11_atom_to_xatom(IPC_PROPERTY_CALL), /*type*/
			8,
			PropModeReplace,
			data,
			strlen(data) + 1);
	XSync(display, FALSE);
	g_free(data);
	ipc_client_send_client_message(IPC_CLIENT_MESSAGE_CALL);
	if(gdk_error_trap_pop()) {
		g_warning("could not set the property for calling the command, ignoring the command");
		goto no_prop_set;
	}
	data = ipc_client_wait_for_property(IPC_PROPERTY_RETURN, TRUE);
	if(!data) {
		g_warning("No return value obtained");
		goto no_return_val;
	}
	ret =ipc_command_list_parse(data);
	if(!ret){
		g_warning("malformed return value, ignoring it");
		goto malform;
	}
malform:
	XFree(data);
no_return_val:
no_prop_set:
	return ret;

}
/**
 *	ipc_client_call_server_command:
 *
 *	Internal function to call a command. 
 *
 *	It first creates an transaction, then calls
 *	ipc_client_call_server_list.
 *	the command will be freed.
 *
 * */
static gboolean ipc_client_call_server_command(IPCCommand * command, gchar ** rt){
	/*The command is 'destroyed' after this function'*/
	if(!in_transaction) {
		GList * commands = NULL;
		GList * returns = NULL;
		commands = g_list_append(commands, command);
		returns = ipc_client_call_list(commands);
		ipc_command_list_free(commands);
		if(returns) {
			if(rt)
			*rt = g_strdup(ipc_command_get_default_result(returns->data));
			ipc_command_list_free(returns);
			return TRUE;
		} else {
			return FALSE;
		}
	} else {
		transaction = g_list_append(transaction, command);
		return TRUE;
	}
}
/**
 * ipc_client_call_server_valist:
 *
 * valist_version of ipc_client_call_server.
 */
gboolean ipc_client_call_server_valist(const gchar * command_name, gchar ** rt, va_list va) {
	IPCCommand * command = ipc_command_new(cid, command_name);
	ipc_command_set_parameters_valist(command, va);
	return ipc_client_call_server_command(command, rt);
}
/**
 * ipc_client_call_server:
 * 	@command_name: the name of the command.
 * 	@para_name: name of the first parameter.
 * 	@...: 	value of the first parameter and (name, value) pairs for
 * 			other parameters. Ended by NULL.
 *
 * Returns: the 'default' result.
 */
gboolean ipc_client_call_server(const gchar * command_name, gchar ** rt, ...) {
	gboolean r = NULL;
	va_list va;
	va_start(va, rt);
	r = ipc_client_call_server_valist(command_name, rt, va);
	va_end(va);
	return rt;
}
/**
 * ipc_client_call_server_array:
 *
 * 	Array version of ipc_client_call_server.
 */
gboolean ipc_client_call_server_array(const gchar * command_name, gchar ** rt, gchar ** paras, gchar ** values){
	IPCCommand * command = ipc_command_new(cid, command_name);
	ipc_command_set_parameters_array(command, paras, values);
	return ipc_client_call_server_command(command, rt);
}
/**
 * ipc_client_begin_transaction:
 *
 * 	Starts a transaction.
 */
void ipc_client_begin_transaction() {
	transaction = NULL;
	in_transaction = TRUE;
}
/**
 * ipc_client_cancel_transaction:
 *
 * 	cancels a transaction.
 */
void ipc_client_cancel_transaction() {
	ipc_command_list_free(transaction);
	in_transaction = FALSE;
	transaction = NULL;
}
/**
 * ipc_client_end_transaction:
 *
 * Issue the transaction to the server,
 * and read the results.
 */
void ipc_client_end_transaction(GList ** return_list){
	GList * rt = ipc_client_call_list(transaction);
	if(return_list) *return_list = rt;
	else ipc_command_list_free(rt);
	ipc_command_list_free(transaction);
	transaction = NULL;
	in_transaction = FALSE;
}
/**
 * ipc_client_set_event:
 * 	@event: name of the event
 * 	@handler: handler
 * 	@data: data passed to the handler
 *
 * 	Listens to an event from the server. The old handler is removed.
 *
 * 	This function utilizes the internal IPC call _AddEvent to get the server-side task done.
 *
 */
void ipc_client_set_event_valist(gchar * event, IPCClientEventHandler handler, gpointer data, va_list va){
	EventHandlerInfo * info = g_slice_new0(EventHandlerInfo);
	IPCCommand * command = ipc_command_new(cid, "_AddEvent_");
	info->data = data;
	info->handler = handler;
	g_datalist_init(&info->filter);
	g_datalist_set_data_full(&event_handler_list, event, info, event_handler_info_free);
	/*can't reverse the order since set_parameters will clear the previous settings*/
	ipc_command_set_parameters_valist(command, va);
	IPCSetParam(command, g_strdup("_event_"), g_strdup(event));
	ipc_client_call_server_command(command, NULL);
}
void ipc_client_set_event_array(gchar * event, IPCClientEventHandler handler, gpointer data, gchar ** paras, gchar ** values){
	EventHandlerInfo * info = g_slice_new0(EventHandlerInfo);
	IPCCommand * command = ipc_command_new(cid, "_AddEvent_");
	info->data = data;
	info->handler = handler;
	g_datalist_init(&info->filter);
	g_datalist_set_data_full(&event_handler_list, event, info, event_handler_info_free);
	/*can't reverse the order since set_parameters will clear the previous settings*/
	ipc_command_set_parameters_array(command, paras, values);
	IPCSetParam(command, g_strdup("_event_"), g_strdup(event));
	ipc_client_call_server_command(command, NULL);
}
void ipc_client_set_event(gchar * event, IPCClientEventHandler handler, gpointer data, ...){
	va_list va;
	va_start(va, data);
	ipc_client_set_event_valist(event, handler, data, va);
	va_end(va);
}
/**
 * ipc_client_remove_event:
 * 	@event: name of the event
 *
 * 	Stop listening to an event. The handler is removed.
 *
 * 	This function utilizes the internal IPC call _RemoveEvent_ to get the server-side
 * 	task done.
 */
void ipc_client_remove_event(gchar * event){
	g_datalist_remove_data(&event_handler_list, event);
	ipc_client_call_server("_RemoveEvent_", NULL, "event", event, NULL);
}
