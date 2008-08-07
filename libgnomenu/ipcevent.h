#ifndef _IPC_EVENT_H_
#define _IPC_EVENT_H_
#include "ipcevent.h"
typedef IPCCommand IPCEvent;

IPCEvent * ipc_event_parse(const gchar * string);
IPCEvent * ipc_event_new(const gchar * from, const gchar * to, const gchar * name);
void ipc_event_free(IPCEvent * event);
gchar * ipc_event_to_string(IPCEvent * event);
void ipc_event_set_parameters(IPCEvent * event,  ...);
void ipc_event_set_parameters_valist(IPCEvent * event, va_list va);
#endif
