#ifndef _IPC_EVENT_SOURCE_H_
#define _IPC_EVENT_SOURCE_H_
#include "ipcevent.h"
#define IPC_EVENT_SOURCE_REGISTER(name, in) \
{ \
	static gchar ** _ipc_in = in; \
	static gchar _ipc_name[] = name; \
	ipc_event_source_register(_ipc_name, in); \
}
void ipc_event_source_register(const gchar * event, const gchar ** in);
void ipc_event_source_mute(const gchar * event);
void ipc_event_source_unmute(const gchar * event);
void ipc_event_source_emit(const gchar * event, ...);
void ipc_event_source_emit_va(const gchar * event, va_list va);
void ipc_event_source_emit_array(const gchar * event, gchar ** parameters, gchar ** values);
void ipc_event_source_emit_event(IPCEvent * event);
#endif
