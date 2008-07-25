#ifndef _IPC_COMMAND_H_
#define _PIC_COMMAND_H_
gboolean ipc_command_parse(const gchar * string, 
		gchar ** name, GHashTable ** parameters, GHashTable ** results);
gchar * ipc_command_to_string(gchar * name, GHashTable * parameters, GHashTable * results);
GHashTable * ipc_parameters(gchar * name, va_list va);
GHashTable * ipc_results(gchar * name, va_list va);
#endif
