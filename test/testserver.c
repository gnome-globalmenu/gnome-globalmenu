#include <gtk/gtk.h>
#include <libgnomenu/ipcserver.h>

gboolean ping(IPCCommand * command, gpointer data) {
	gchar * message = g_hash_table_lookup(command->parameters, "message");
	g_message("Ping received from %s: %s", command->cid, message);
	g_hash_table_insert(command->results, g_strdup("default"), g_strdup(message));
	return TRUE;
}
int main(int argc, char* argv[]){
	gtk_init(&argc, &argv);
	ipc_server_register_cmd("Ping", ping, NULL);
	if(!ipc_server_listen()) {
		g_error("server already there");
		return 1;
	}
	gtk_main();
	return 0;
}
