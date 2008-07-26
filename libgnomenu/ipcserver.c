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
#include "ipcserver.h"
#include "ipccommand.h"

typedef struct _CommandInfo {
	gchar * name;
	ServerCMD server_cmd;
	gpointer data;
} CommandInfo;

static GHashTable * command_hash = NULL;
static GdkWindow * server_window = NULL;
static gboolean server_frozen = TRUE;
static void command_info_destroy(CommandInfo * info) {
	g_free(info->name);
	g_slice_free(CommandInfo, info);
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
	return info->server_cmd(command->parameters, command->results, info->data);
}
static GdkFilterReturn default_filter (GdkXEvent * xevent, GdkEvent * event, gpointer data);

gboolean ipc_server_listen() {
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

	return TRUE;
}
void ipc_server_freeze() {
	server_frozen = TRUE;
}
void ipc_server_thaw() {
	server_frozen = FALSE;
}
static GdkFilterReturn default_filter (GdkXEvent * xevent, GdkEvent * event, gpointer data){
	if(server_frozen) return GDK_FILTER_CONTINUE;
	switch(((XEvent *)xevent)->type) {
		case ClientMessage: {
			XClientMessageEvent * client_message = (XClientMessageEvent *) xevent;
			if(client_message->message_type != gdk_x11_atom_to_xatom(IPC_CLIENT_MESSAGE_CALL)) 
				return GDK_FILTER_CONTINUE;
			GdkNativeWindow src = * ((GdkNativeWindow *) (&client_message->data.b));
			gpointer data;
			Atom type_return;
			unsigned long format_return;
			unsigned long nitems_return;
			unsigned long remaining_bytes;
			Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) ;
			gdk_x11_grab_server();
			gdk_error_trap_push();
			XGetWindowProperty(display,
					src,
					gdk_x11_atom_to_xatom(IPC_PROPERTY_CALL),
					0,
					-1,
					TRUE,
					AnyPropertyType,
					&type_return,
					&format_return,
					&nitems_return,
					&remaining_bytes,
					&data);
			if(gdk_error_trap_pop()) {
				g_warning("could not obtain call information, ignoring the call");
				goto no_prop;
			}
			IPCCommand * command = ipc_command_parse(data);
			if(!command){
				g_warning("malformed command, ignoring the call");
				goto parse_fail;
			}
			if(!ipc_server_call_cmd(command)) {
				g_warning("command was not successfull, ignoring the call");
				goto call_fail;
			}
			gchar * ret = ipc_command_to_string(command);
			gdk_error_trap_push();
	
			XChangeProperty(display,
				src,
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
call_fail:
			ipc_command_free(command);
parse_fail:
			XFree(data);
no_prop:
			gdk_x11_ungrab_server();
			return GDK_FILTER_REMOVE;
		}
		break;	
	}
	return GDK_FILTER_CONTINUE;
}
