#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuServer>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif

#include "ipcserver.h"

gboolean ping(IPCCommand * command, gpointer data) {
	gchar * message = g_hash_table_lookup(command->parameters, "message");
	g_message("Ping received from %s: %s", command->cid, message);
	g_hash_table_insert(command->results, g_strdup("default"), g_strdup(message));
	return TRUE;
}

static GHashTable * client_hash = NULL;
static void client_create_callback(gchar * cid, gpointer data) {
	LOG("New client %s", cid);
	g_hash_table_insert(client_hash, g_strdup(cid), NULL);
}
static void client_destroy_callback(gchar * cid, gpointer data) {
	LOG("Dead client %s", cid);
	g_hash_table_remove(client_hash, cid);
}
int main(int argc, char* argv[]){
	gtk_init(&argc, &argv);

	client_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	ipc_server_register_cmd("Ping", ping, NULL);
	ipc_server_register_cmd("CreateObject", ping, NULL);
	ipc_server_register_cmd("DestroyObject", ping, NULL);
	ipc_server_register_cmd("SetProperty", ping, NULL);
	ipc_server_register_cmd("ActivateObject", ping, NULL);
	ipc_server_register_cmd("InsertChild", ping, NULL);
	ipc_server_register_cmd("RemoveChild", ping, NULL);
	if(!ipc_server_listen(client_create_callback, client_destroy_callback, NULL)) {
		g_error("server already there");
		return 1;
	}
	gtk_main();
	return 0;
}
