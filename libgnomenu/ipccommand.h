#ifndef _IPC_COMMAND_H_
#define _IPC_COMMAND_H_
typedef struct _IPCCommand IPCCommand;
IPCCommand * ipc_command_parse(const gchar * string);
GList * ipc_command_list_parse(const gchar * string);

gchar * ipc_command_to_string(IPCCommand * command);
gchar * ipc_command_list_to_string(GList * command_list);
#define IPCParam(c, p) \
	ipc_command_get_parameter(c, p)
#define IPCSetParam(c, p, v) \
	ipc_command_set_parameter(c, p, v)
#define IPCRet(c, rt) \
	ipc_command_set_return_value(c, rt)
#define IPCRetDup(c, rt) \
	IPCRet(c, g_strdup(rt))
#define IPCRetBool(c, rt) \
	if(rt) IPCRetDup(c, "TRUE"); else IPCRetDup(c, "FALSE"); 
#define IPCFail(c) \
	ipc_command_set_fail(c, TRUE)
void ipc_command_free(IPCCommand * command);
void ipc_command_list_free(GList * list);
IPCCommand * ipc_command_new(gchar * name);
void ipc_command_set_parameters(IPCCommand * command,  ...);
void ipc_command_set_results(IPCCommand * command, ...);
void ipc_command_clear_parameters(IPCCommand * command);
void ipc_command_clear_results(IPCCommand * command);
void ipc_command_set_parameters_valist(IPCCommand * command,  va_list va);
void ipc_command_set_parameters_array(IPCCommand * command, gchar ** paras, gchar ** values);
void ipc_command_set_results_valist(IPCCommand * command, va_list va);
void ipc_command_steal(IPCCommand * theft, IPCCommand * stolen);

void ipc_command_set_source(IPCCommand * command, const gchar * from);
const gchar * ipc_command_get_source(IPCCommand * command);

void ipc_command_set_target(IPCCommand * command, const gchar * target);
const gchar * ipc_command_get_target(IPCCommand * command);

const gchar * ipc_command_get_parameter(IPCCommand * command, const gchar * paraname);
void ipc_command_set_parameter(IPCCommand * command, const gchar * paraname, const gchar * value);

void ipc_command_set_return_value(IPCCommand * command, const gchar * value);
const gchar * ipc_command_get_return_value(IPCCommand * command);

void ipc_command_set_fail(IPCCommand * command, const gboolean fail);
gboolean ipc_command_get_fail(IPCCommand * command);

const gchar * ipc_command_get_name(IPCCommand * command);
#endif
