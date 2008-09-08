#include <gtk/gtk.h>
#include "ipc.h"
#include "ipccommand.h"
#include "ipcdispatcher.h"

typedef struct _CommandInfo {
	gchar * name;
	IPCCMD server_cmd;
	gpointer data;
} CommandInfo;

static GData * command_hash = NULL;
static gboolean initialized = FALSE;
static void command_info_destroy(CommandInfo * info) {
	g_free(info->name);
	g_slice_free(CommandInfo, info);
}

static void Introspect_foreach(GQuark command_quark, gpointer handler, gpointer foo[]){
	GString * string = foo[0];
	g_string_append_printf(string, "%s\n", g_quark_to_string(command_quark));
}
static gboolean Introspect(IPCCommand * command, gpointer data){
	GString * string = g_string_new("");
	gpointer foo[] = {string};
	g_datalist_foreach(&command_hash, Introspect_foreach, foo);
	IPCRet(command, g_string_free(string, FALSE));
	return TRUE;
}
void ipc_dispatcher_register_cmd(const gchar * name, IPCCMD cmd_handler, gpointer data) {
	CommandInfo * info = g_slice_new0(CommandInfo);
	info->name = g_strdup(name);
	info->server_cmd = cmd_handler;
	info->data = data;
	if(!initialized){
		g_datalist_init(&command_hash);
		initialized = TRUE;
		ipc_dispatcher_register_cmd("Introspect", Introspect, NULL);
	}
	g_datalist_set_data_full(&command_hash, name, info, command_info_destroy);
}
gboolean ipc_dispatcher_call_cmd(IPCCommand * command) {
	CommandInfo * info = g_datalist_id_get_data(&command_hash, command->name);
	if(!info) return FALSE;
	return info->server_cmd(command, info->data);
}

