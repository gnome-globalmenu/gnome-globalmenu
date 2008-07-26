#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuGlobalMenu>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gdk/gdkx.h>
#include "ipc.h"
#include "ipcclient.h"
#include "ipccommand.h"

static GdkWindow * client_window = NULL;
static gboolean client_frozen = TRUE;
static IPCClientServerDestroyNotify server_destroy_notify = NULL;
static gpointer server_destroy_notify_data = NULL;
static GList * queue = NULL;
static gchar * client_id = NULL; /*obtained after NEGO*/

static GdkFilterReturn server_filter(GdkXEvent * xevent, GdkEvent * event, gpointer data){
	if(((XEvent *)xevent)->type == DestroyNotify) {
		XDestroyWindowEvent * dwe = (XDestroyWindowEvent *) xevent;
		g_critical("Server is down!");
		if(server_destroy_notify)
			server_destroy_notify(server_destroy_notify_data);
	} else {
	}
	return GDK_FILTER_CONTINUE;
}
static void ipc_client_send_client_message(GdkNativeWindow server, GdkAtom message_type) {
	GdkEventClient ec;
	ec.type = GDK_CLIENT_EVENT;
	ec.window = 0;
	ec.send_event = TRUE;
	ec.message_type = message_type;
	ec.data_format = 8;
	*((GdkNativeWindow *)&ec.data.l[0]) = GDK_WINDOW_XWINDOW(client_window);
	gdk_event_send_client_message(&ec, server);
}
static gpointer ipc_client_wait_for_property(GdkAtom property_name, gboolean remove) {
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	Atom type_return;
	unsigned long format_return;
	unsigned long remaining_bytes;
	unsigned long nitems_return;
	gpointer data;
	while(1) {
		gdk_error_trap_push();
		XGetWindowProperty(display,
				GDK_WINDOW_XWINDOW(client_window),
				gdk_x11_atom_to_xatom(property_name),
				0,
				-1,
				remove,
				AnyPropertyType,
				&type_return,
				&format_return,
				&nitems_return,
				&remaining_bytes,
				&data);
		if(gdk_error_trap_pop()) {
			return NULL;
		} else {
			if(type_return != None)
				return data;
		}
	}
	return data;
}
gboolean ipc_client_start(IPCClientServerDestroyNotify notify, gpointer data){
	gboolean rt = FALSE;
	gdk_x11_grab_server();
	GdkNativeWindow server = ipc_find_server();
	if(server == 0) {
		gdk_x11_ungrab_server();
		goto no_server;
	}
	GdkWindow * server_gdk = gdk_window_lookup(server);
	if(!server_gdk) server_gdk = gdk_window_foreign_new(server);

	gdk_window_set_events(server_gdk, gdk_window_get_events(server_gdk) | GDK_STRUCTURE_MASK);
	gdk_window_add_filter(server_gdk, server_filter, NULL);
	server_destroy_notify = notify;
	server_destroy_notify_data = data;
	GdkWindowAttr attr;
	attr.title = IPC_CLIENT_TITLE;
	attr.wclass = GDK_INPUT_ONLY;
	client_window = gdk_window_new(NULL, &attr, GDK_WA_TITLE);
	client_frozen = FALSE;

	gdk_x11_ungrab_server();
	ipc_client_send_client_message(server, IPC_CLIENT_MESSAGE_NEGO);
	gchar * identify = ipc_client_wait_for_property(IPC_PROPERTY_CID, FALSE);
	if(identify) {
		client_id = g_strdup(identify);
		LOG("client_id obtained: %s", client_id);
		XFree(identify);
		rt = TRUE;
	} else {
		rt = FALSE;
	}
no_server:
	return rt;
}
gchar * ipc_client_call_server(const gchar * command_name, gchar * para_name, ...) {
	/* dummy variables */
	Atom type_return;
	unsigned long format_return;
	unsigned long remaining_bytes;
	unsigned long nitems_return;
	gpointer data;
	/* dummy variables ends here*/
	gchar * rt;
	g_assert(client_window);
	GdkNativeWindow server = ipc_find_server();
	IPCCommand * command = ipc_command_new();
	command->name = g_strdup(command_name);
	va_list va;
	va_start(va, para_name);
	ipc_command_set_parameters_valist(command, para_name, va);
	va_end(va);
	data = ipc_command_to_string(command);
	g_return_if_fail(data != NULL);
	ipc_command_free(command);

	if(!server) {
		g_critical("no server is found, method failed");
		return NULL;
	}

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
	if(gdk_error_trap_pop()) {
		g_warning("could not set the property for calling the command, ignoring the command");
		goto no_prop_set;
	}
	ipc_client_send_client_message(server, IPC_CLIENT_MESSAGE_CALL);
	data = ipc_client_wait_for_property(IPC_PROPERTY_RETURN, TRUE);
	if(!data) {
		g_warning("No return value obtained");
		goto no_return_val;
	}
	IPCCommand * ret = ipc_command_parse(data);
	if(!ret){
		g_warning("malformed return value, ignoring it");
		goto malform;
	}
	rt = g_strdup(g_hash_table_lookup(ret->results, "default"));
	ipc_command_free(ret);
malform:
	XFree(data);
no_return_val:
no_prop_set:
	return rt;
}
