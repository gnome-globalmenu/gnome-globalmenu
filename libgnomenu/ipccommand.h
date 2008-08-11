#ifndef _IPC_COMMAND_H_
#define _IPC_COMMAND_H_
typedef struct {
	GQuark name;
	GQuark from;
	GQuark to;
	GData * parameters;
	GData * results;
	gboolean failed;
} IPCCommand;
IPCCommand * ipc_command_parse(const gchar * string);
GList * ipc_command_list_parse(const gchar * string);

gchar * ipc_command_to_string(IPCCommand * command);
gchar * ipc_command_list_to_string(GList * command_list);
#define IPCParam(c, p) \
	g_datalist_get_data(&((IPCCommand*)(c))->parameters, (p))
#define IPCSetParam(c, p, v) \
	g_datalist_set_data_full(&((IPCCommand*)(c))->parameters, p, (v), g_free)
#define IPCRemoveParam(c, p) \
	g_datalist_remove_data(&((IPCCommand*)(c))->parameters, (p))
#define IPCRet(c, rt) \
	g_datalist_set_data_full(&((IPCCommand*)(c))->results, "default", (rt), g_free)
#define IPCRetDup(c, rt) \
	IPCRet(c, g_strdup(rt))
#define IPCRetBool(c, rt) \
	if(rt) IPCRetDup(c, "TRUE"); else IPCRetDup(c, "FALSE"); 
#define IPCFail(c) ((IPCCommand*)(c))->failed = TRUE

void ipc_command_free(IPCCommand * command);
void ipc_command_list_free(GList * list);
IPCCommand * ipc_command_new(gchar * from, gchar * to, gchar * name);
void ipc_command_set_parameters(IPCCommand * command,  ...);
void ipc_command_set_results(IPCCommand * command, ...);
void ipc_command_clear_parameters(IPCCommand * command);
void ipc_command_clear_results(IPCCommand * command);
void ipc_command_set_parameters_valist(IPCCommand * command,  va_list va);
void ipc_command_set_parameters_array(IPCCommand * command, gchar ** paras, gchar ** values);
void ipc_command_set_results_valist(IPCCommand * command, va_list va);
gchar * ipc_command_get_default_result(IPCCommand * command);
#endif
