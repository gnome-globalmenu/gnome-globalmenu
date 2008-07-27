#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuGlobalMenu>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include "ipcserver.h"

gboolean ping(IPCCommand * command, gpointer data) {
	gchar * message = g_hash_table_lookup(command->parameters, "message");
	g_message("Ping received from %s: %s", command->cid, message);
	g_hash_table_insert(command->results, g_strdup("default"), g_strdup(message));
	return TRUE;
}

int main(int argc, char* argv[]){
	gtk_init(&argc, &argv);
	ipc_server_register_cmd("Ping", ping, NULL);
	if(!ipc_server_listen(NULL, NULL)) {
		g_error("server already there");
		return 1;
	}
	gtk_main();
	return 0;
}
