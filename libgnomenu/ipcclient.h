#ifndef _IPC_CLIENT_H_
#define _IPC_CLIENT_H_
typedef void (*IPCClientServerDestroyNotify)(gpointer data);
gboolean ipc_client_start(IPCClientServerDestroyNotify notify, gpointer data) ;
gchar * ipc_client_call_server(const gchar * command_name, gchar * para_name, ...);

#endif
