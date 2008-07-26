#ifndef _IPC_SERVER_H_
#define _IPC_SERVER_H_
#include "ipccommand.h"

typedef gboolean (*ServerCMD)(IPCCommand * command, gpointer data);
void ipc_server_freeze();
void ipc_server_thaw();
gboolean ipc_server_listen();
void ipc_server_register_cmd(const gchar * name, ServerCMD cmd_handler, gpointer data);
#endif
