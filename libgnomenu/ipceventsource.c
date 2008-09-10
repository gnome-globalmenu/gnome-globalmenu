#include <gtk/gtk.h>

#include "ipceventsource.h"
#include "ipcclient.h"

typedef struct {
	const gchar * name;
	const gchar ** in;
	gboolean muted;
} EventInfo;

static GData * event_hash = NULL;
static gboolean initialized = FALSE;

static void event_info_destroy(EventInfo * info){
	g_slice_free(EventInfo, info);
}
void ipc_event_source_register(const gchar * name, const gchar * in) {
	EventInfo * info = g_slice_new0(EventInfo);
	info->name = name;
	info->in = in;
	info->muted = TRUE;
	if(!initialized){
		g_datalist_init(&event_hash);
	}
	g_datalist_set_data_full(&event_hash, name, info, event_info_destroy);
}
void ipc_event_source_mute(const gchar * name) {
	EventInfo * info = g_datalist_get_data(&event_hash, name);
	g_return_if_fail(info != NULL);
	info->muted = TRUE;
}
void ipc_event_source_unmute(const gchar * name) {
	EventInfo * info = g_datalist_get_data(&event_hash, name);
	g_return_if_fail(info != NULL);
	info->muted = FALSE;
}
void ipc_event_source_emit_event(IPCEvent * event) {
	EventInfo * info = g_datalist_id_get_data(&event_hash, event->name);
	if(!info->muted) {
		gchar * event_str = ipc_event_to_string(event);
		ipc_client_call(NULL, "Emit", NULL, "event", event_str, NULL);
		g_free(event_str);
	}
}
void ipc_event_source_emit(const gchar * event, ...);

void ipc_event_source_emit_va(const gchar * event, va_list va);
void ipc_event_source_emit_array(const gchar * event, gchar ** parameters, gchar ** values);
