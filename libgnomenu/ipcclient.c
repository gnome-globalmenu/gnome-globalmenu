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
#include "ipcdispatcher.h"
#include "ipceventsink.h"
#include "ipceventsource.h"

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
	gchar * event_data = ipc_wait_for_property(GDK_WINDOW_XWINDOW(client_window), IPC_PROPERTY_EVENT, TRUE);
	LOG("event dataa %s", event_data);
	if(!event_data) {
		g_critical("can't obtain event data");	
		return;
	}
	GList * list = ipc_event_list_parse(event_data);
	XFree(event_data);
	if(!list) {
		/*This occurs when multiple events accumulate*/
		LOG("empty event list, ignore this message");
		return;
	}
	GList * node;
	for(node = list; node; node = node->next){
		IPCEvent * event = node->data;
		ipc_event_sink_dispatch(event);
	}
	ipc_event_list_free(list);
}
static void client_message_call(XClientMessageEvent * client_message) {
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
	gpointer data;
	data = ipc_get_property(GDK_WINDOW_XWINDOW(client_window), IPC_PROPERTY_SERVERCALL);
	if(!data) {
		g_warning("could not obtain call information, ignoring the call");
		goto no_prop;
	}
	LOG("%s", data);
	GList * commands = ipc_command_list_parse(data);
	XFree(data);
	if(!commands){
		g_warning("malformed command, ignoring the call");
		goto parse_fail;
	}
	GList * node;
	for(node = commands; node; node=node->next){
		IPCCommand * command = node->data;
		if(g_str_equal(cid, ipc_command_get_target(command))) {
			if(!ipc_dispatcher_call_cmd(command)) {
				g_warning("command was not successfull, ignoring the call");
				IPCFail(command);
			}
		} else {
			g_warning("command is not for this client, ignoreing the call: %s %s", cid, ipc_command_get_target(command));
		}
	}
	gchar * ret = ipc_command_list_to_string(commands);
	gdk_error_trap_push();

	ipc_set_property(GDK_WINDOW_XWINDOW(client_window), IPC_PROPERTY_SERVERRETURN, ret);
	if(gdk_error_trap_pop()) {
		g_warning("could not set the property for returing the command");
	}
	g_free(ret);
unknown_client:
call_fail:
	ipc_command_list_free(commands);
parse_fail:
no_prop:
//	gdk_x11_ungrab_server();
	return;

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
			if(client_message->message_type == gdk_x11_atom_to_xatom(IPC_CLIENT_MESSAGE_CALL)) {
				client_message_call(client_message);
				return GDK_FILTER_REMOVE;
			}
		return GDK_FILTER_CONTINUE;
	}
	return GDK_FILTER_CONTINUE;
}
/**
 * ipc_client_started:
 *
 * Returns: whether the client is started.
 */
gboolean ipc_client_started(){
	return started;
}
static gboolean Ping(IPCCommand * command, gpointer data) {
	IPCRet(command, g_strdup(IPCParam(command, "message")));
	return TRUE;
}
static gboolean Mute(IPCCommand * command, gpointer data){
	ipc_event_source_mute(IPCParam(command, "event"));
	return TRUE;
}
static gboolean Unmute(IPCCommand * command, gpointer data){
	ipc_event_source_unmute(IPCParam(command, "event"));
	return TRUE;
}
static void ServerSampleEvent(IPCEvent * event, gpointer data){
	LOG("received a server event");
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
	IPC_DISPATCHER_REGISTER("Ping", Ping, IPC_IN("message"), IPC_OUT("result"), NULL);
	IPC_DISPATCHER_REGISTER("Mute", Mute, IPC_IN("event"), IPC_OUT("result"), NULL);
	IPC_DISPATCHER_REGISTER("Unmute", Unmute, IPC_IN("event"), IPC_OUT("result"), NULL);
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
	ipc_send_client_message(GDK_WINDOW_XWINDOW(client_window), server, IPC_CLIENT_MESSAGE_NEGO);
	gchar * identify = ipc_wait_for_property(GDK_WINDOW_XWINDOW(client_window), IPC_PROPERTY_CID, FALSE);
	if(identify) {
		cid = g_strdup(identify);
		LOG("CID obtained: %s", cid);
		XFree(identify);
		started = TRUE;
		ipc_event_sink_listen("SampleEvent", "SERVER", ServerSampleEvent, NULL);
	} else {
		started = FALSE;
	}
no_server:
	return started;
}
static gchar * ipc_client_call_server_xml(gchar * xml){
	gchar * ret_xml;
	g_return_val_if_fail(started, NULL);
	g_assert(client_window);
	LOG("calling %s", xml);
	gdk_error_trap_push();
	ipc_set_property(GDK_WINDOW_XWINDOW(client_window), IPC_PROPERTY_CALL, xml);	
	ipc_send_client_message(GDK_WINDOW_XWINDOW(client_window), server, IPC_CLIENT_MESSAGE_CALL);
	if(gdk_error_trap_pop()) {
		g_warning("could not set the property for calling the command, ignoring the command");
		goto no_prop_set;
	}
	ret_xml = ipc_wait_for_property(GDK_WINDOW_XWINDOW(client_window), IPC_PROPERTY_RETURN, TRUE);
	if(!ret_xml) {
		g_warning("No return value obtained");
		goto no_return_val;
	}
no_return_val:
no_prop_set:
	return ret_xml;

}
/**
 * ipc_client_call_list:
 *
 * performance a transaction, send a command list.
 */
static gboolean ipc_client_call_list(GList ** command_list) {
	gchar * xml = ipc_command_list_to_string(*command_list);
	gchar * ret_xml = ipc_client_call_server_xml(xml);
	GList * newlist = NULL;
	g_free(xml);
	if(!ret_xml) return FALSE;
	newlist = ipc_command_list_parse(ret_xml);
	XFree(ret_xml);
	if(!*command_list) {
		g_warning("malformed return value");	
		return FALSE;
	}
	ipc_command_list_free(*command_list);
	*command_list = newlist;
	return TRUE;
}
/**
 *	ipc_client_call_server_command:
 *
 *	Internal function to call a command. 
 *
 *	It first creates an transaction, then calls
 *	ipc_client_call_server_list.
 *	the command will be replaced with the a new command object with the result.
 *
 * */
static gboolean ipc_client_call_server_command(IPCCommand * command){
	/*The command is 'destroyed' after this function'*/
	if(!in_transaction) {
		gchar * xml = ipc_command_to_string(command);
		gchar * ret_xml = NULL;
		ret_xml = ipc_client_call_server_xml(xml);
		IPCCommand * returns = ipc_command_parse(ret_xml);
		g_free(xml);
		XFree(ret_xml);
		if(!returns) {
			g_warning("malformed return value");
			return FALSE;
		} else {
			ipc_command_steal(command, returns);
			return TRUE;
		}
	} else {
		transaction = g_list_append(transaction, command);
		return TRUE;
	}
}
/**
 * ipc_client_call_valist:
 *
 * valist_version of ipc_client_call.
 */
gboolean ipc_client_call_valist(gchar * target, const gchar * command_name, gchar ** rt, va_list va) {
	IPCCommand * command = ipc_command_new(command_name);
	ipc_command_set_source(command, cid);
	ipc_command_set_target(command, target?target:"SERVER");
	ipc_command_set_parameters_valist(command, va);
	if(ipc_client_call_server_command(command)){
		if(rt) *rt = g_strdup(ipc_command_get_return_value(command));
		ipc_command_free(command);
		return TRUE;
	} else {
		return FALSE;
	}
}
gboolean ipc_client_call_command(IPCCommand * command) {
	ipc_command_set_source(command, cid);
	return ipc_client_call_server_command(command);
}
/**
 * ipc_client_call:
 * 	@command_name: the name of the command.
 * 	@para_name: name of the first parameter.
 * 	@...: 	value of the first parameter and (name, value) pairs for
 * 			other parameters. Ended by NULL.
 *
 * Returns: the 'default' result.
 */
gboolean ipc_client_call(gchar * target, const gchar * command_name, gchar ** rt, ...) {
	gboolean r = NULL;
	va_list va;
	va_start(va, rt);
	r = ipc_client_call_valist(target, command_name, rt, va);
	va_end(va);
	return rt;
}
/**
 * ipc_client_call_server_array:
 *
 * 	Array version of ipc_client_call_server.
 */
gboolean ipc_client_call_array(gchar * target, const gchar * command_name, gchar ** rt, gchar ** paras, gchar ** values){
	IPCCommand * command = ipc_command_new(command_name);
	ipc_command_set_source(command, cid);
	ipc_command_set_target(command, target?target:"SERVER");
	ipc_command_set_parameters_array(command, paras, values);
	if(ipc_client_call_server_command(command)){
		if(rt) *rt = g_strdup(ipc_command_get_return_value(command));
		ipc_command_free(command);
		return TRUE;
	} else {
		return FALSE;
	}
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
 * Returns: FALSE if transaction fails, where we are still in the transaction
 * after this function returns.
 */
gboolean ipc_client_end_transaction(GList ** return_list){
	if(ipc_client_call_list(&transaction)) {
		if(return_list)
			*return_list = transaction;
		else
			ipc_command_list_free(transaction);
		transaction = NULL;
		in_transaction = FALSE;
	}
	else {
		return FALSE;
	}
}
