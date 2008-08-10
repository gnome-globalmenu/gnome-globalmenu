#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<GnomenuServer>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif

#include "ipcserver.h"
#include "object.h"

typedef struct {
	GQuark cid;
	GHashTable * menus;
} ClientInfo;
#define ADD_MENU(cli, wind, menu) g_hash_table_insert(cli->menus, wind, menu)
#define REMOVE_MENU(cli, wind, menu) g_hash_table_remove(cli->menus, wind)
#define FIND_MENU(cli, wind) g_hash_table_lookup(cli->menus, wind)
#define find_client(cli) g_datalist_id_get_data(&client_list, cli)
static GData * client_list = NULL;
void client_info_free(ClientInfo * info) {
	g_hash_table_destroy(info->menus);
	g_slice_free(ClientInfo, info);
}
static void client_create_callback(GQuark cid, gpointer data) {
	LOG("New client %s", g_quark_to_string(cid));
	ClientInfo * info = g_slice_new0(ClientInfo);
	info->cid = cid;
	info->menus = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	g_datalist_id_set_data_full(&client_list, info->cid, info, client_info_free);
}
static void client_destroy_callback(GQuark cid, gpointer data) {
	LOG("Dead client %s", g_quark_to_string(cid));
	g_datalist_id_remove_data(&client_list, cid);
}
gboolean Unimplemented(IPCCommand * command, gpointer data) {
	IPCRet(command, g_strdup("This method is Unimplemented"));
	return TRUE;
}
gboolean BindMenu(IPCCommand * command, gpointer data){
	ClientInfo * ci = find_client(command->from);
	g_return_val_if_fail(ci, FALSE);
	gchar * wind = IPCParam(command, "window");
	gchar * menu = IPCParam(command, "menu");
	ADD_MENU(ci, g_strdup(wind), g_strdup(menu));
	return TRUE;
}
gboolean UnbindMenu(IPCCommand * command, gpointer data){
	ClientInfo * ci = find_client(command->from);
	g_return_val_if_fail(ci, FALSE);
	gchar * wind = IPCParam(command, "window");
	gchar * menu = IPCParam(command, "menu");
	/*FIXME: menu parameter is ignored*/
	REMOVE_MENU(ci, wind, menu);
	return TRUE;
}
static void LookupMenu_foreach_client(GQuark key, gpointer value, gpointer foo[]){
	ClientInfo * ci = value;
	gchar * wind = foo[0];
	gchar * menu = FIND_MENU(ci, wind);
	if(menu) {
		foo[1] = menu;
		foo[2] = ci;
	}
}
gboolean LookupWindow(IPCCommand * command, gpointer data){
	gchar * wind = IPCParam(command, "window");
	gpointer foo[3] = {wind, 
		NULL/*Return value, the found menu*/,
		NULL/*Return value, the cid*/};
	g_datalist_foreach(&client_list, LookupMenu_foreach_client, foo);
	if(foo[2]){
		IPCRetDup(command, g_quark_to_string(((ClientInfo*)foo[2])->cid));
	}
	return TRUE;
}
static void ListClients_foreach_menu(gpointer key, gpointer value, gpointer foo[]){
	GString * string = foo[0];
	gchar * wind = key;
	gchar * menu = value;
	g_string_append_printf(string, "<toplevel window=\"%s\" menu=\"%s\"/>\n",
			wind, menu);
}
static void ListClients_foreach_client(GQuark cid, gpointer value, gpointer foo[]){
	GString * string = foo[0];
	ClientInfo * info = value;
	g_string_append_printf(string, "<client cid=\"%s\">\n", g_quark_to_string(info->cid));
	g_hash_table_foreach(info->menus, ListClients_foreach_menu, foo);
	g_string_append_printf(string, "</client>\n");
}
gboolean ListClients(IPCCommand * command, gpointer data){
	GString * string = g_string_new("");
	gpointer foo[] = { string };
	g_datalist_foreach(&client_list, ListClients_foreach_client, foo);
	IPCRet(command, g_string_free(string, FALSE));
	return TRUE;
}
int main(int argc, char* argv[]){
	GModule * module = g_module_open(NULL, 0);
	gboolean * no_global_menu = NULL;
	g_module_symbol(module, "gtk_no_global_menu", &no_global_menu);
	if(no_global_menu) {
		* no_global_menu = TRUE;
		g_message("hacking gnomenu patch in gtk...");
	}
	gtk_init(&argc, &argv);

	g_datalist_init(&client_list);
	/* BindMenu:
	 * 	window: the toplevel GdkNativeWindow to bind to (string from in dec)
	 * 	menu: the name of the menu object
	 *
	 * 	Bind a menu object to a toplevel window.
	 * 	*/
	ipc_dispatcher_register_cmd("BindMenu", BindMenu, NULL);
	/* UnbindMenu:
	 * 	window: the toplevel GdkNativeWindow to bind to (string from in dec)
	 * 	menu: the name of the menu object
	 *
	 * 	Unbind a menu object to a toplevel window.
	 * 	*/
	ipc_dispatcher_register_cmd("UnbindMenu", UnbindMenu, NULL);
	/* LookupWindow:
	 * 	window: the toplevel window you are interested in.
	 *
	 * 	Returns:
	 * 		the cid associated with the window.
	 */
	ipc_dispatcher_register_cmd("LookupWindow", LookupWindow, NULL);
	/* ListClients:
	 * 	void
	 *
	 * 	Returns:
	 * 	 a GMarkup text segment describing the clients and their toplevels with menus.
	 * 	*/
	ipc_dispatcher_register_cmd("ListClients", ListClients, NULL);
	if(!ipc_server_listen(client_create_callback, client_destroy_callback, NULL)) {
		g_critical("server already there");
		return 1;
	}
	gtk_main();
	return 0;
}
