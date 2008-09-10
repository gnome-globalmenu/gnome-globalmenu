#ifndef _IPC_EVENT_SINK_H_
#define _IPC_EVENT_SINK_H_
#include "ipcevent.h"
typedef void (*IPCEventHandler)(IPCEvent * event, gpointer data);
void ipc_event_sink_listen(const gchar * event, const gchar * source, IPCEventHandler handler, gpointer data);
void ipc_event_sink_unlisten(const gchar * event, const gchar * source, IPCEventHandler handler, gpointer data);
void ipc_event_sink_dispatch(IPCEvent * event);
#endif
