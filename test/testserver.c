#include <gtk/gtk.h>
#include <libgnomenu/ipcserver.h>

gboolean ping(IPCCommand * command, gpointer data) {
	gchar * message = g_hash_table_lookup(command->parameters, "message");
	g_message("Ping received from %s: %s", command->cid, message);
	g_hash_table_insert(command->results, g_strdup("default"), g_strdup(message));
	ipc_server_send_event(command);
	return TRUE;
}
int main(int argc, char* argv[]){
#if 0
	gtk_init(&argc, &argv);
	ipc_server_register_cmd("Ping", ping, NULL);
	if(!ipc_server_listen(NULL, NULL)) {
		g_error("server already there");
		return 1;
	}
	gtk_main();
#endif
	return 0;
}
