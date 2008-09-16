#ifndef _IPC_SERVER_BASE_H_
#define _IPC_SERVER_BASE_H_
#include "ipccommand.h"
#include "ipcevent.h"
typedef struct _ClientInfo {
	const gchar * cid;
	GdkNativeWindow xwindow;
	GdkWindow * window;
	struct {
		GData * installed;
		GData * listening;
	} events;
} ClientInfo;
typedef struct _InstalledEventInfo{
	gint ref_count;
} InstalledEventInfo;
typedef struct _ListeningEventInfo{
	ClientInfo * source_info;
} ListeningEventInfo;
#define SVR_STR(str) ipc_server_define_string(str)

gboolean ipc_server_boot(ClientCreateCallback cccb, ClientDestroyCallback cdcb, gpointer data);
const gchar * ipc_server_define_string(const gchar * string);
#endif
