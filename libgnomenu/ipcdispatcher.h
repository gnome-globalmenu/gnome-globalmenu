#ifndef _IPC_DISPATCHER_H_
#define _IPC_DISPATCHER_H_
#include "ipccommand.h"
typedef gboolean (*IPCCMD)(IPCCommand * command, gpointer data);
void ipc_dispatcher_register_cmd(const gchar * name, IPCCMD cmd_handler, gpointer data);
gboolean ipc_dispatcher_call_cmd(IPCCommand * command);
#endif
