#ifndef _IPC_CLIENT_H_
#define _IPC_CLIENT_H_
#include "ipccommand.h"
#include "ipcevent.h"
typedef void (*IPCClientServerDestroyNotify)(gpointer data);
typedef void (*IPCClientEventHandler)(IPCEvent * event, gpointer data);

gboolean ipc_client_start(IPCClientServerDestroyNotify notify, gpointer data) ;
gboolean ipc_client_started() ;
gchar * ipc_client_call_server(const gchar * command_name, gchar * para_name, ...);
gchar * ipc_client_call_server_valist(const gchar * command_name, gchar * para_name, va_list va);
gchar * ipc_client_call_server_array(const gchar * command_name, gchar ** paras, gchar ** values);

void ipc_client_begin_transaction();
void ipc_client_cancel_transaction();
void ipc_client_end_transaction(GList ** return_list);

void ipc_client_set_event(gchar * event, IPCClientEventHandler handler, gpointer data);
void ipc_client_remove_event(gchar * event);
#endif
