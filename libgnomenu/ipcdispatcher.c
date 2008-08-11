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

static void command_info_destroy(CommandInfo * info) {
	g_free(info->name);
	g_slice_free(CommandInfo, info);
}

void ipc_dispatcher_register_cmd(const gchar * name, IPCCMD cmd_handler, gpointer data) {
	CommandInfo * info = g_slice_new0(CommandInfo);
	info->name = g_strdup(name);
	info->server_cmd = cmd_handler;
	info->data = data;
	if(command_hash == NULL){
		g_datalist_init(&command_hash);
	}
	g_datalist_set_data_full(&command_hash, name, info, command_info_destroy);
}
gboolean ipc_dispatcher_call_cmd(IPCCommand * command) {
	CommandInfo * info = g_datalist_id_get_data(&command_hash, command->name);
	if(!info) return FALSE;
	return info->server_cmd(command, info->data);
}

