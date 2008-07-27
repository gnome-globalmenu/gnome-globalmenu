#ifndef _IPC_SERVER_H_
#define _IPC_SERVER_H_
#include "ipccommand.h"
#include "ipcevent.h"

typedef gboolean (*ServerCMD)(IPCCommand * command, gpointer data);
typedef void (*ClientDestroyCallback)(gchar * cid, gpointer data);
void ipc_server_freeze();
void ipc_server_thaw();
gboolean ipc_server_listen(ClientDestroyCallback cb, gpointer data);
void ipc_server_register_cmd(const gchar * name, ServerCMD cmd_handler, gpointer data);
gboolean ipc_server_send_event(IPCEvent * event);
#endif
