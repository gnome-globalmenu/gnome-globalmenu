#include <gtk/gtk.h>
#include "ipc.h"
#include "ipccommand.h"
#include "ipcdispatcher.h"

typedef struct _CommandInfo {
	const gchar * name;
	IPCCMD server_cmd;
	gpointer data;
	const gchar ** in;
	const gchar ** out;
} CommandInfo;

static GData * command_hash = NULL;
static gboolean initialized = FALSE;
static void command_info_destroy(CommandInfo * info) {
	g_slice_free(CommandInfo, info);
}

static void Introspect_foreach(GQuark command_quark, gpointer data, gpointer foo[]){
	GString * string = foo[0];
	CommandInfo * info = data;
	int i;
	g_string_append_printf(string, "%s ", g_quark_to_string(command_quark));
	g_string_append_printf(string, "IN( ");
	for(i = 0; info->in[i]; i++){
		g_string_append_printf(string, "%s ", info->in[i]);
	}
	g_string_append_printf(string, ") ");
	g_string_append_printf(string, "OUT( ");
	for(i = 0; info->out[i]; i++){
		g_string_append_printf(string, "%s ", info->out[i]);
	}
	g_string_append_printf(string, ")\n");
}
static gboolean Introspect(IPCCommand * command, gpointer data){
	GString * string = g_string_new("");
	gpointer foo[] = {string};
	g_datalist_foreach(&command_hash, Introspect_foreach, foo);
	IPCRet(command, g_string_free(string, FALSE));
	return TRUE;
}
/** Never call this function directly, use the macro IPC_DISPATCHER_REGISTER instead*/
void ipc_dispatcher_register(const gchar * name, IPCCMD cmd_handler, const gchar * in, const gchar * out, gpointer data) {
	CommandInfo * info = g_slice_new0(CommandInfo);
	info->name = name;
	info->server_cmd = cmd_handler;
	info->data = data;
	info->in = in;
	info->out = out;
	if(!initialized){
		g_datalist_init(&command_hash);
		initialized = TRUE;
		IPC_DISPATCHER_REGISTER("Introspect", Introspect, IPC_IN("VOID"), IPC_OUT("result"), NULL)
	}
	g_datalist_set_data_full(&command_hash, name, info, command_info_destroy);
}
gboolean ipc_dispatcher_call_cmd(IPCCommand * command) {
	CommandInfo * info = g_datalist_get_data(&command_hash, ipc_command_get_name(command));
	if(!info) return FALSE;
	return info->server_cmd(command, info->data);
}

