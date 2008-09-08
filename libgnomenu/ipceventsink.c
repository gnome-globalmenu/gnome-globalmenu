#include <gtk/gtk.h>
#include "ipceventsink.h"
typedef struct {
	IPCEventHandler handler;
	gpointer data;
} IPCEventHandlerInfo;
typedef struct {
	gchar * name;
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
void ipc_event_sink_listen(const gchar * event, IPCEventHandler handler, gpointer data) {
	IPCEventInfo * info  = g_datalist_get_data(&events, event);
	if(!info) {
		info = g_slice_new0(IPCEventInfo);
		g_datalist_set_data_full(&events, event, info, ipc_event_info_free);
	} else {
		IPCEventHandlerInfo * hinfo = g_slice_new0(IPCEventHandlerInfo);
		hinfo->handler = handler;
		hinfo->data = data;
		info->handlers = g_list_append(info->handlers, hinfo);
	}
}
void ipc_event_sink_unlisten(const gchar * event, IPCEventHandler handler, gpointer data) {
	IPCEventInfo * info  = g_datalist_get_data(&events, event);
	g_return_if_fail(info != NULL);
	GList * node;
	for(node = info->handlers; node; node = node->next) {
		IPCEventHandlerInfo * hinfo = node->data;
		if(hinfo->handler == handler && hinfo->data == data) {
			g_slice_free(IPCEventHandlerInfo, node->data);
			info->handlers = g_list_delete_link(info->handlers, node);
			break;
		}
	}
}

void ipc_event_sink_dispatch(IPCEvent * event) {
	IPCEventInfo * info  = g_datalist_id_get_data(&events, event->name);
	g_return_if_fail(info != NULL);
	GList * node;
	for(node = info->handlers; node; node = node->next) {
		IPCEventHandlerInfo * hinfo = node->data;
		hinfo->handler(event, hinfo->data);
	}
}
