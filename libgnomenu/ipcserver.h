#ifndef _IPC_SERVER_H_
#define _IPC_SERVER_H_
#include "ipccommand.h"
#include "ipcevent.h"

typedef void (*ClientDestroyCallback)(const gchar * cid, gpointer data);
typedef void (*ClientCreateCallback)(const gchar * cid, gpointer data);
void ipc_server_freeze();
void ipc_server_thaw();
gboolean ipc_server_listen(ClientCreateCallback cccb, ClientDestroyCallback cdcb, gpointer data);
gboolean ipc_server_send_event(IPCEvent * event);
#endif
