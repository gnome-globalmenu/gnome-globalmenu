#ifndef _IPC_COMMAND_H_
#define _PIC_COMMAND_H_
typedef struct {
	gchar * name;
	gchar * client_id;
	GHashTable * parameters;
	GHashTable * results;
} IPCCommand;
IPCCommand * ipc_command_parse(const gchar * string);
gchar * ipc_command_to_string(IPCCommand * command);
void ipc_command_free(IPCCommand * command);
IPCCommand * ipc_command_new();
void ipc_command_set_parameters(IPCCommand * command, gchar * para_name, ...);
void ipc_command_set_results(IPCCommand * command, gchar * result_name, ...);

void ipc_command_set_parameters_valist(IPCCommand * command, gchar * para_name, va_list va);
void ipc_command_set_results_valist(IPCCommand * command, gchar * para_name, va_list va);
#endif
