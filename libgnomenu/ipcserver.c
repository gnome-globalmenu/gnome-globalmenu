#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<IPCServer>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gdk/gdkx.h>
#include "ipc.h"
#include "ipcutils.h"
#include "ipcserver.h"
#include "ipccommand.h"

typedef struct _ClientInfo {
	GQuark cid;
	GdkNativeWindow xwindow;
	GdkWindow * window;
	GData * event_mask;
} ClientInfo;

static GdkWindow * server_window = NULL;
static gboolean server_frozen = TRUE;
static GHashTable * client_hash = NULL;
static GHashTable * client_hash_by_cid = NULL;
static ClientDestroyCallback client_destroy_callback = NULL;
static ClientCreateCallback client_create_callback = NULL;
static gpointer callback_data = NULL;

static void client_info_destroy(gpointer data){
	ClientInfo * info = data;
	g_hash_table_remove(client_hash_by_cid, g_quark_to_string(info->cid));
//	gdk_window_destroy(info->window);
	g_datalist_clear(&info->event_mask);
	g_slice_free(ClientInfo, info);
}
static GdkFilterReturn default_filter (GdkXEvent * xevent, GdkEvent * event, gpointer data);
static void AddEvent_foreach(GQuark key, gchar * value, gpointer foo[1]){
	GHashTable * hash = foo[0];
	if(key != g_quark_from_string("_event_"))
		g_hash_table_insert(hash, g_quark_to_string(key), g_strdup(value));
}
static gboolean AddEvent(IPCCommand * command, gpointer data) {
	ClientInfo * info = g_hash_table_lookup(client_hash_by_cid, g_quark_to_string(command->from));
	g_assert(info);
	gchar * eventname = IPCParam(command, "_event_");
	/*strdup because IPCRemoveParam will free it*/
	LOG("%s is hooking to %s;", g_quark_to_string(info->cid), eventname);
	
	/*This is ugly. building a new parameter list is better*/
	GHashTable * ipc_event_filter_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);
	gpointer foo[] = {ipc_event_filter_hash};
	g_datalist_foreach(&command->parameters, AddEvent_foreach, foo);
	g_datalist_set_data_full(&(info->event_mask), 
			eventname,
			ipc_event_filter_hash, (GDestroyNotify)g_hash_table_unref);
	return TRUE;
}
static gboolean RemoveEvent(IPCCommand * command, gpointer data) {
	ClientInfo * info = g_hash_table_lookup(client_hash_by_cid, g_quark_to_string(command->from));
	g_assert(info);
	g_datalist_remove_data(&(info->event_mask), 
			IPCParam(command, "_event_"));
	return TRUE;
}
static gboolean Ping(IPCCommand * command, gpointer data) {
	IPCRet(command, g_strdup(IPCParam(command, "message")));
	return TRUE;
}
static gboolean Emit(IPCCommand * command, gpointer data) {
	gchar * event_name = IPCParam(command, "_event_");
	IPCEvent * event = ipc_event_new(g_quark_to_string(command->from), g_quark_to_string(command->to), event_name);
	/*FIXME: This is ugly, use a foreach to build the parameters*/
	gpointer tmp = event->parameters;
	event->parameters = command->parameters;
	ipc_server_send_event(event);
	event->parameters = tmp;
	ipc_event_free(event);
	return TRUE;
}
static gchar * ipc_server_call_client_xml(ClientInfo * info, gchar * xml){
	gchar * ret_xml;

	gdk_error_trap_push();
	ipc_set_property(info->xwindow, IPC_PROPERTY_SERVERCALL, xml);	
	ipc_send_client_message(GDK_WINDOW_XWINDOW(server_window), info->xwindow, IPC_CLIENT_MESSAGE_CALL);
	if(gdk_error_trap_pop()) {
		g_warning("could not set the property for calling the command, ignoring the command");
		goto no_prop_set;
	}
	ret_xml = ipc_wait_for_property(info->xwindow, IPC_PROPERTY_SERVERRETURN, TRUE);
	if(!ret_xml) {
		g_warning("No return value obtained");
		goto no_return_val;
	}
no_return_val:
no_prop_set:
	return ret_xml;

}
static gboolean ipc_server_call_client_command(IPCCommand ** command) {
	ClientInfo * info = g_hash_table_lookup(client_hash_by_cid, 
			g_quark_to_string((*command)->to));
	LOG("target = %s", g_quark_to_string((*command)->to));
	g_return_val_if_fail(info, FALSE);
	gchar * xml = ipc_command_to_string(*command);
	gchar * ret_xml = NULL;
	ret_xml = ipc_server_call_client_xml(info, xml);
	IPCCommand * returns = ipc_command_parse(ret_xml);
	g_free(xml);
	XFree(ret_xml);
	if(!returns) {
		g_warning("malformed return value");
		return FALSE;
	} else {
		ipc_command_free(*command);
		*command = returns;
		return TRUE;
	}
	return FALSE;
}

gboolean ipc_server_listen(ClientCreateCallback cccb, ClientDestroyCallback cdcb, gpointer data) {
	ipc_dispatcher_register_cmd("Ping", Ping, NULL);
	ipc_dispatcher_register_cmd("Emit", Emit, NULL);
	ipc_dispatcher_register_cmd("_AddEvent_", AddEvent, NULL);
	ipc_dispatcher_register_cmd("_RemoveEvent_", RemoveEvent, NULL);
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
	gdk_flush();
	XSync(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), FALSE);
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
//	gdk_x11_grab_server();
	data = ipc_get_property(info->xwindow, IPC_PROPERTY_CALL);
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
		if(info->cid != (command->from)) {
			g_warning("unknown client, ignoring the call: cid = %s from =%s", g_quark_to_string(info->cid), g_quark_to_string(command->from));
			goto unknown_client;
		}
		if(g_quark_from_string("SERVER") == command->to) {
			if(!ipc_dispatcher_call_cmd(command)) {
				IPCFail(command);
				g_warning("command was not successfull, ignoring the call");
			}
		} else {
			if(!ipc_server_call_client_command(&command)){
				IPCFail(command);
			}
			node->data = command;
		}
	}
	gchar * ret = ipc_command_list_to_string(commands);
	gdk_error_trap_push();

	ipc_set_property(info->xwindow, IPC_PROPERTY_RETURN, ret);
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

static GdkFilterReturn client_filter(GdkXEvent * xevent, GdkEvent * event, gpointer data){
	ClientInfo * info = data;
	if(((XEvent *)xevent)->type == DestroyNotify) {
		XDestroyWindowEvent * dwe = (XDestroyWindowEvent *) xevent;
		LOG("client %s is down!", g_quark_to_string(info->cid));
		gdk_window_remove_filter(info->window, client_filter, info);
		if(client_destroy_callback)
			client_destroy_callback(info->cid, callback_data);
		g_hash_table_remove(client_hash, (gpointer) info->xwindow);
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
	client_info->cid = g_quark_from_string(identify);
	gdk_error_trap_push();
	
	ipc_set_property(src, IPC_PROPERTY_CID, identify);

	g_hash_table_insert(client_hash, (gpointer) src, client_info);
	g_hash_table_insert(client_hash_by_cid, g_quark_to_string(client_info->cid), client_info);
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
			ClientInfo * info = g_hash_table_lookup(client_hash, (gpointer) src);

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
	ipc_send_client_message(GDK_WINDOW_XWINDOW(server_window), client_xwindow, message_type);
}
static gboolean ipc_server_send_event_to(GdkNativeWindow xwindow, IPCEvent * event) {
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
	gchar * data = ipc_event_to_string(event);
	if(!data) {
		g_critical("Could not format the event");
		return FALSE;
	}
	gdk_error_trap_push();
	gdk_x11_grab_server(); /*ensure the client is not taking away the unprocessed event*/
	gchar * old_event_data = ipc_get_property(xwindow, IPC_PROPERTY_EVENT);
	gchar * new_event_data;
	if(old_event_data) {
		new_event_data = g_strdup_printf("%s\n%s", old_event_data, data);
		XFree(old_event_data);
	} else
		new_event_data = g_strdup(data);
	g_free(data);
	ipc_set_property(xwindow, IPC_PROPERTY_EVENT, new_event_data);
	gdk_x11_ungrab_server();
	g_free(new_event_data);
	ipc_server_send_client_message(xwindow, IPC_CLIENT_MESSAGE_EVENT);
	if(gdk_error_trap_pop()) {
		g_warning("could not set the property for the event");
		goto set_prop_fail;
	}
	return TRUE;
set_prop_fail:
	return FALSE;
}
/**
 * ipc_server_send_event:
 *
 *
 * Send the event only if 
 *
 * (1) the client added the filter.
 * (2) the parameters and values in the filter matches with the event.
 *
 */
gboolean ipc_server_send_event(IPCEvent * event) {
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, client_hash);
	GdkNativeWindow xwindow; 
	ClientInfo * info;
	while(g_hash_table_iter_next(&iter, (gpointer*)&xwindow, (gpointer*)&info)){
		GHashTable * filter = g_datalist_id_get_data(&(info->event_mask), event->name);
		LOG("testing client %s for event %s: filter = %p", g_quark_to_string(info->cid), g_quark_to_string(event->name), filter);
		gboolean send = FALSE;
		if(filter){
			send = TRUE;
			GHashTableIter iter2;
			g_hash_table_iter_init(&iter2, filter);
			gpointer prop;
			gpointer value;
			while(g_hash_table_iter_next(&iter2, &prop, &value)){
				gchar * event_value = IPCParam(event, prop);
				if(!event_value || !g_str_equal(event_value, value)){
					send = FALSE;	
					break;
				}
			}
		}
		if(send) {
			LOG("sending event %s to %s", g_quark_to_string(event->name), g_quark_to_string(info->cid));
			ipc_server_send_event_to(xwindow, event);
		}
	}
	return TRUE;
}
