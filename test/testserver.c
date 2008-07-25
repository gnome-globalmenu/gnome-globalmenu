#include <gtk/gtk.h>
#include <libgnomenu/ipcserver.h>

gboolean ping(GHashTable * parameters, GHashTable * returns, gpointer data) {
	gchar * message = g_hash_table_lookup(parameters, "message");
	g_message("Ping received: %s", message);
	g_hash_table_insert(returns, g_strdup("default"), g_strdup(message));
	return TRUE;
}
int main(int argc, char* argv[]){
	gtk_init(&argc, &argv);
	ipc_server_register_cmd("Ping", ping, NULL);
	ipc_server_listen();
	gtk_main();
	return 0;
}
