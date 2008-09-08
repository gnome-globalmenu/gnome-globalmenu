#ifndef _IPC_DISPATCHER_H_
#define _IPC_DISPATCHER_H_
#include "ipccommand.h"
typedef gboolean (*IPCCMD)(IPCCommand * command, gpointer data);
void ipc_dispatcher_register(const gchar * name, IPCCMD cmd_handler, const gchar * in, const gchar * out, gpointer data);
#define IPC_IN(...) {__VA_ARGS__, NULL}
#define IPC_OUT(...) {__VA_ARGS__, NULL}
#define IPC_DISPATCHER_REGISTER(name, cmd_handler, in, out, data) \
{\
	static const gchar * _ipc_in[] = in ;\
	static const gchar * _ipc_out[] = out;  \
	ipc_dispatcher_register(name, cmd_handler, _ipc_in, _ipc_out, data); \
}

gboolean ipc_dispatcher_call_cmd(IPCCommand * command);
#endif
