#ifndef _IPC_CLIENT_H_
#define _IPC_CLIENT_H_
#include "ipccommand.h"
#include "ipcevent.h"
typedef void (*IPCClientServerDestroyNotify)(gpointer data);
typedef void (*IPCClientEventHandler)(IPCEvent * event, gpointer data);

gboolean ipc_client_start(IPCClientServerDestroyNotify notify, gpointer data) ;
gboolean ipc_client_started() ;
gboolean ipc_client_call(gchar * target, const gchar * command_name, gchar ** ret, ...);
gboolean ipc_client_call_valist(gchar * target, const gchar * command_name, gchar ** ret, va_list va);
gboolean ipc_client_call_array(gchar * target, const gchar * command_name, gchar ** ret, gchar ** paras, gchar ** values);

void ipc_client_begin_transaction();
void ipc_client_cancel_transaction();
gboolean ipc_client_end_transaction(GList ** return_list);

void ipc_client_set_event(gchar * event, IPCClientEventHandler handler, gpointer data, ...);
void ipc_client_set_event_valist(gchar * event, IPCClientEventHandler handler, gpointer data, va_list va);
void ipc_client_remove_event(gchar * event);
#endif
