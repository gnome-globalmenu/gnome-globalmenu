#ifndef _IPC_CLIENT_H_
#define _IPC_CLIENT_H_
void ipc_client_start() ;
gchar * ipc_client_call_server(const gchar * command_name, gchar * para_name, ...);

#endif
