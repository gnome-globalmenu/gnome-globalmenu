#include <gtk/gtk.h>
#include "ipceventsink.h"

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<IPCEventSink>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)
typedef struct {
	IPCEventHandler handler;
	const gchar * source;
	gpointer data;
} IPCEventHandlerInfo;
typedef struct {
	const gchar * name;
	GList * handlers;
} IPCEventInfo;

static GData * events = NULL;

void ipc_event_info_free(gpointer p){
	IPCEventInfo * info = p;
	GList * node;
	for(node = info->handlers; node; node = node->next) {
		g_slice_free(IPCEventHandlerInfo, node->data);
	}
	g_list_free(info->handlers);
	g_slice_free(IPCEventInfo, p);
}
void ipc_event_sink_listen(const gchar * event, const gchar * source, IPCEventHandler handler, gpointer data) {
	IPCEventInfo * info  = g_datalist_get_data(&events, event);
	if(!info) {
		info = g_slice_new0(IPCEventInfo);
		g_datalist_set_data_full(&events, event, info, ipc_event_info_free);
	}
	{
	IPCEventHandlerInfo * hinfo = g_slice_new0(IPCEventHandlerInfo);
	hinfo->handler = handler;
	hinfo->data = data;
	hinfo->source = g_strdup(source);
	info->handlers = g_list_append(info->handlers, hinfo);
	}
	ipc_client_call(NULL, "Listen", NULL, "source", source, "event", event, NULL);
}
void ipc_event_sink_unlisten(const gchar * event, const gchar * source, IPCEventHandler handler, gpointer data) {
	IPCEventInfo * info  = g_datalist_get_data(&events, event);
	g_return_if_fail(info != NULL);
	GList * node;
	for(node = info->handlers; node; node = node->next) {
		IPCEventHandlerInfo * hinfo = node->data;
		if(g_str_equal(hinfo->source, source) && hinfo->handler == handler && hinfo->data == data) {
			ipc_client_call(NULL, "Unlisten", NULL, "source", source, "event", event, NULL);
			g_free(hinfo->source);
			g_slice_free(IPCEventHandlerInfo, node->data);
			info->handlers = g_list_delete_link(info->handlers, node);
			break;
		}
	}
}

void ipc_event_sink_dispatch(IPCEvent * event) {
	IPCEventInfo * info  = g_datalist_get_data(&events, ipc_command_get_name(event));
	LOG("dispatching event %s", ipc_command_get_name(event));
	g_return_if_fail(info != NULL);
	GList * node;
	for(node = info->handlers; node; node = node->next) {
		IPCEventHandlerInfo * hinfo = node->data;
		LOG("matching %s against %s", ipc_command_get_source(event), hinfo->source);
		if(g_str_equal(hinfo->source, ipc_command_get_source(event))){
			hinfo->handler(event, hinfo->data);
		}
	}
}
