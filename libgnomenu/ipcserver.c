#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<IPCServer>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gdk/gdkx.h>
#include "ipc.h"
#include "ipcutils.h"
#include "ipcserver.h"
#include "ipccommand.h"

typedef struct _CommandInfo {
	gchar * name;
	ServerCMD server_cmd;
	gpointer data;
} CommandInfo;
typedef struct _ClientInfo {
	gchar * cid;
	GdkNativeWindow xwindow;
	GdkWindow * window;
	GData * event_mask;
} ClientInfo;

static GHashTable * command_hash = NULL;
static GdkWindow * server_window = NULL;
static gboolean server_frozen = TRUE;
static GHashTable * client_hash = NULL;
static GHashTable * client_hash_by_cid = NULL;
static ClientDestroyCallback client_destroy_callback = NULL;
static ClientCreateCallback client_create_callback = NULL;
static gpointer callback_data = NULL;

static void command_info_destroy(CommandInfo * info) {
	g_free(info->name);
	g_slice_free(CommandInfo, info);
}
static void client_info_destroy(ClientInfo * info){
	g_hash_table_remove(client_hash_by_cid, info->cid);
	g_free(info->cid);
//	gdk_window_destroy(info->window);
	g_datalist_clear(&info->event_mask);
	g_slice_free(ClientInfo, info);
}
void ipc_server_register_cmd(const gchar * name, ServerCMD cmd_handler, gpointer data) {
	CommandInfo * info = g_slice_new0(CommandInfo);
	info->name = g_strdup(name);
	info->server_cmd = cmd_handler;
	info->data = data;
	if(command_hash == NULL) {
		command_hash = g_hash_table_new_full(g_str_hash,
				g_str_equal,
				NULL,
				(GDestroyNotify) command_info_destroy);
	}
	if(g_hash_table_lookup(command_hash, name)){
		g_warning("Replacing old command definition");
	}
	g_hash_table_insert(command_hash, name, info);
}
static gboolean ipc_server_call_cmd(IPCCommand * command) {
	if(command_hash == NULL) {
		return FALSE;
	}
	CommandInfo * info = g_hash_table_lookup(command_hash, command->name);
	if(!info) return FALSE;
	return info->server_cmd(command, info->data);
}
static gchar * ipc_server_get_property(GdkNativeWindow src, GdkAtom property_name){
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
	gpointer data;
	Atom type_return;
	unsigned long format_return;
	unsigned long nitems_return;
	unsigned long remaining_bytes;
	gdk_error_trap_push();
	XGetWindowProperty(display,
			src,
			gdk_x11_atom_to_xatom(property_name),
			0,
			-1,
			TRUE,
			AnyPropertyType,
			&type_return,
			&format_return,
			&nitems_return,
			&remaining_bytes,
			&data);
	if(gdk_error_trap_pop()){
		return NULL;
	} else {
		if(type_return == None) return NULL;
		return data;
	}
}
static GdkFilterReturn default_filter (GdkXEvent * xevent, GdkEvent * event, gpointer data);
static gboolean AddEvent(IPCCommand * command, gpointer data) {
	ClientInfo * info = g_hash_table_lookup(client_hash_by_cid, command->cid);
	g_assert(info);
	g_datalist_set_data(&(info->event_mask), 
			IPCParam(command, "event"),
			(gpointer) 1);
	return TRUE;
}
static gboolean RemoveEvent(IPCCommand * command, gpointer data) {
	ClientInfo * info = g_hash_table_lookup(client_hash_by_cid, command->cid);
	g_assert(info);
	g_datalist_remove_data(&(info->event_mask), 
			IPCParam(command, "event"));
	return TRUE;
}
gboolean ipc_server_listen(ClientCreateCallback cccb, ClientDestroyCallback cdcb, gpointer data) {
	ipc_server_register_cmd("_AddEvent_", AddEvent, NULL);
	ipc_server_register_cmd("_RemoveEvent_", RemoveEvent, NULL);
	gdk_x11_grab_server();
	GdkNativeWindow old_server = ipc_find_server();
	if(old_server) return FALSE;
	GdkWindowAttr attr;
	attr.title = IPC_SERVER_TITLE;
	attr.wclass = GDK_INPUT_ONLY;
	server_window = gdk_window_new(NULL, &attr, GDK_WA_TITLE);
	gdk_window_set_events(server_window, GDK_STRUCTURE_MASK || gdk_window_get_events(server_window));
	gdk_window_add_filter(server_window, default_filter, NULL);
	server_frozen = FALSE;
	gdk_x11_ungrab_server();
	client_hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, client_info_destroy);
	client_hash_by_cid = g_hash_table_new(g_str_hash, g_str_equal);
	client_destroy_callback = cdcb;	
	client_create_callback = cccb;	
	callback_data = data;	
	return TRUE;
}
void ipc_server_freeze() {
	server_frozen = TRUE;
}
void ipc_server_thaw() {
	server_frozen = FALSE;
}
static void client_message_call(ClientInfo * info, XClientMessageEvent * client_message) {
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
	gpointer data;
	Atom type_return;
	unsigned long format_return;
	unsigned long nitems_return;
	unsigned long remaining_bytes;
//	gdk_x11_grab_server();
	data = ipc_server_get_property(info->xwindow, IPC_PROPERTY_CALL);
	if(!data) {
		g_warning("could not obtain call information, ignoring the call");
		goto no_prop;
	}
	GList * commands = ipc_command_list_parse(data);
	XFree(data);
	if(!commands){
		g_warning("malformed command, ignoring the call");
		goto parse_fail;
	}
	GList * node;
	for(node = commands; node; node=node->next){
		IPCCommand * command = node->data;
		if(!g_str_equal(info->cid, command->cid)) {
			g_warning("unknown client, ignoring the call");
			goto unknown_client;
		}
		if(!ipc_server_call_cmd(command)) {
			g_warning("command was not successfull, ignoring the call");
			goto call_fail;
		}
	}
	gchar * ret = ipc_command_list_to_string(commands);
	gdk_error_trap_push();

	XChangeProperty(display,
		info->xwindow,
		gdk_x11_atom_to_xatom(IPC_PROPERTY_RETURN),
		gdk_x11_atom_to_xatom(IPC_PROPERTY_RETURN), /*type*/
		8,
		PropModeReplace,
		ret,
		strlen(ret) + 1);
	XSync(display, FALSE);
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

static GdkFilterReturn client_filter(GdkXEvent * xevent, GdkEvent * event, ClientInfo * info){
	if(((XEvent *)xevent)->type == DestroyNotify) {
		XDestroyWindowEvent * dwe = (XDestroyWindowEvent *) xevent;
		g_message("client %s is down!", info->cid);
		gdk_window_remove_filter(info->window, client_filter, info);
		if(client_destroy_callback)
			client_destroy_callback(info->cid, callback_data);
		g_hash_table_remove(client_hash, info->xwindow);
	} else {
	}
	return GDK_FILTER_CONTINUE;
}
static void client_message_nego(ClientInfo * unused, XClientMessageEvent * client_message) {
	static guint id = 1000;
	GdkNativeWindow src = * ((GdkNativeWindow *) (&client_message->data.b));
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
	gchar * identify = g_strdup_printf("%d", id++);
	
	ClientInfo * client_info = g_slice_new0(ClientInfo);
	client_info->xwindow = src;
	g_datalist_init(&(client_info->event_mask));
	gdk_x11_grab_server();
	client_info->window = gdk_window_lookup(src);
	if(!client_info->window) client_info->window = gdk_window_foreign_new(src);
	client_info->cid = identify;
	gdk_error_trap_push();
	
	XChangeProperty(display,
		src,
		gdk_x11_atom_to_xatom(IPC_PROPERTY_CID),
		gdk_x11_atom_to_xatom(IPC_PROPERTY_CID), /*type*/
		8,
		PropModeReplace,
		identify,
		strlen(identify) + 1);
	XSync(display, FALSE);
	g_hash_table_insert(client_hash, src, client_info);
	g_hash_table_insert(client_hash_by_cid, client_info->cid, client_info);
	gdk_window_set_events(client_info->window, gdk_window_get_events(client_info->window) | GDK_STRUCTURE_MASK);
	gdk_window_add_filter(client_info->window, client_filter, client_info);
	if(gdk_error_trap_pop()) {
		g_warning("could not set the identify during NEGO process");
	}
	gdk_x11_ungrab_server();
	if(client_create_callback)
		client_create_callback(client_info->cid, callback_data);
}
static GdkFilterReturn default_filter (GdkXEvent * xevent, GdkEvent * event, gpointer data){
	if(server_frozen) return GDK_FILTER_CONTINUE;
	XClientMessageEvent * client_message = (XClientMessageEvent *) xevent;
#define GET_INFO \
			GdkNativeWindow src = * ((GdkNativeWindow *) (&client_message->data.b)); \
			ClientInfo * info = g_hash_table_lookup(client_hash, src);

	switch(((XEvent *)xevent)->type) {
		case ClientMessage:
			if(client_message->message_type == gdk_x11_atom_to_xatom(IPC_CLIENT_MESSAGE_CALL)) {
				GET_INFO;
				client_message_call(info, client_message);
				return GDK_FILTER_REMOVE;
			} else
			if(client_message->message_type == gdk_x11_atom_to_xatom(IPC_CLIENT_MESSAGE_NEGO)){
				client_message_nego(NULL, client_message);
				return GDK_FILTER_REMOVE;
			}
		return GDK_FILTER_CONTINUE;
	}
	return GDK_FILTER_CONTINUE;
}
static void ipc_server_send_client_message(GdkNativeWindow client_xwindow, GdkAtom message_type) {
	GdkEventClient ec;
	ec.type = GDK_CLIENT_EVENT;
	ec.window = 0;
	ec.send_event = TRUE;
	ec.message_type = message_type;
	ec.data_format = 8;
	*((GdkNativeWindow *)&ec.data.l[0]) = GDK_WINDOW_XWINDOW(server_window);
	gdk_event_send_client_message(&ec, client_xwindow);
}
static gboolean ipc_server_send_event_to(GdkNativeWindow xwindow, IPCEvent * event) {
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
	gchar * data = ipc_event_to_string(event);
	if(!data) {
		g_critical("Could not format the event");
		return FALSE;
	}
	gdk_error_trap_push();

	XChangeProperty(display,
		xwindow,
		gdk_x11_atom_to_xatom(IPC_PROPERTY_EVENT),
		gdk_x11_atom_to_xatom(IPC_PROPERTY_EVENT), /*type*/
		8,
		PropModeReplace,
		data,
		strlen(data) + 1);
	XSync(display, FALSE);
	g_free(data);
	ipc_server_send_client_message(xwindow, IPC_CLIENT_MESSAGE_EVENT);
	if(gdk_error_trap_pop()) {
		g_warning("could not set the property for the event");
		goto set_prop_fail;
	}
	return TRUE;
set_prop_fail:
	return FALSE;
}
gboolean ipc_server_send_event(IPCEvent * event) {
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, client_hash);
	GdkNativeWindow xwindow; 
	ClientInfo * info;
	while(g_hash_table_iter_next(&iter, &xwindow, &info)){
		if(g_datalist_get_data(&(info->event_mask), event->name))
			ipc_server_send_event_to(xwindow, event);
	}
	return TRUE;
}
