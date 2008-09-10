#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<GnomenuServer>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif
#include "ipcdispatcher.h"
#include "ipcserver.h"

typedef struct {
	GQuark cid;
	GHashTable * menus;
} ClientInfo;
#define ADD_MENU(cli, wind, menu) add_menu(cli, wind, menu)
#define REMOVE_MENU(cli, wind, menu) g_hash_table_remove(cli->menus, wind)
#define FIND_MENUS(cli, wind) g_hash_table_lookup(cli->menus, wind)
#define find_client(cli) g_datalist_get_data(&client_list, cli)
void add_menu(ClientInfo * cli, gchar * window, gchar * menu){
	GList * list = g_hash_table_steal(cli->menus, window);
	list = g_list_append(list, g_strdup(menu));
	g_hash_table_insert(cli->menus, window, list);
}
gint strcmp(gpointer, gpointer);
void remove_menu(ClientInfo * cli, gchar * window, gchar * menu){
	GList * list = g_hash_table_steal(cli->menus, window);
	GList * node = g_list_find_custom(list, menu, strcmp);
	if(node) {
		g_free(node->data);
		list = g_list_delete_link(list, node);
	}
	g_hash_table_insert(cli->menus, window, list);
}
static GData * client_list = NULL;
static void client_info_free(ClientInfo * info) {
	g_hash_table_destroy(info->menus);
	g_slice_free(ClientInfo, info);
}
static void menu_list_free(gpointer p){
	GList * list = p;
	GList * node;
	for(node = list; node; node = node->next){
		g_free(node->data);
	}
	g_list_free(p);
}
static void client_create_callback(GQuark cid, gpointer data) {
	LOG("New client %s", g_quark_to_string(cid));
	ClientInfo * info = g_slice_new0(ClientInfo);
	info->cid = cid;
	info->menus = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, menu_list_free);
	g_datalist_id_set_data_full(&client_list, info->cid, info, client_info_free);
}
static void client_destroy_callback(GQuark cid, gpointer data) {
	LOG("Dead client %s", g_quark_to_string(cid));
	g_datalist_id_remove_data(&client_list, cid);
}
static gboolean Unimplemented(IPCCommand * command, gpointer data) {
	IPCRet(command, g_strdup("This method is Unimplemented"));
	return TRUE;
}
static gboolean BindMenu(IPCCommand * command, gpointer data){
	ClientInfo * ci = find_client(ipc_command_get_source(command));
	g_return_val_if_fail(ci, FALSE);
	gchar * wind = IPCParam(command, "window");
	gchar * menu = IPCParam(command, "menu");
	ADD_MENU(ci, g_strdup(wind), g_strdup(menu));
	return TRUE;
}
static gboolean UnbindMenu(IPCCommand * command, gpointer data){
	ClientInfo * ci = find_client(ipc_command_get_source(command));
	g_return_val_if_fail(ci, FALSE);
	gchar * wind = IPCParam(command, "window");
	gchar * menu = IPCParam(command, "menu");
	REMOVE_MENU(ci, wind, menu);
	return TRUE;
}
static void LookupWindow_foreach_client(GQuark key, gpointer value, gpointer foo[]){
	ClientInfo * ci = value;
	gchar * wind = foo[0];
	GList * menus = FIND_MENUS(ci, wind);
	if(menus) {
		foo[1] = menus;
		foo[2] = ci;
	}
}
static void introspect_client_foreach_menu(gpointer key, gpointer value, gpointer foo[]){
	GString * string = foo[0];
	gchar * wind = key;
	GList * menus = value;
	GList * node;
	g_string_append_printf(string, "<toplevel window=\"%s\">\n",
			wind);
	for(node = menus; node; node=node->next){
		g_string_append_printf(string, "<menu name=\"%s\"/>\n"	,
				node->data);
	}
	g_string_append_printf(string, "</toplevel>\n");
}
static void introspect_client(GString * string, ClientInfo * info){
	gpointer foo[] ={string};
	g_string_append_printf(string, "<client cid=\"%s\">\n", g_quark_to_string(info->cid));
	g_hash_table_foreach(info->menus, introspect_client_foreach_menu, foo);
	g_string_append_printf(string, "</client>\n");
}
static gboolean LookupWindow(IPCCommand * command, gpointer data){
	gchar * wind = IPCParam(command, "window");
	gpointer foo[3] = {wind, 
		NULL/*Return value, list all menus*/,
		NULL/*Return value, the cid*/};
	g_datalist_foreach(&client_list, LookupWindow_foreach_client, foo);
	if(foo[2]){
		ClientInfo * ci = foo[2];
		GString * string = g_string_new("");
		introspect_client(string, ci);
		IPCRet(command, g_string_free(string, FALSE));
	}
	return TRUE;
}
static void ListClients_foreach_client(GQuark cid, gpointer value, gpointer foo[]){
	GString * string = foo[0];
	ClientInfo * info = value;
	introspect_client(string, info);
}
gboolean ListClients(IPCCommand * command, gpointer data){
	GString * string = g_string_new("");
	gpointer foo[] = { string };
	g_datalist_foreach(&client_list, ListClients_foreach_client, foo);
	gchar * s = g_string_free(string, FALSE);
	LOG("rt = %s", s);
	IPCRet(command, s);
	return TRUE;
}
int main(int argc, char* argv[]){
	gnomenu_disable();
	gtk_init(&argc, &argv);

	g_datalist_init(&client_list);
	/* BindMenu:
	 * 	window: the toplevel GdkNativeWindow to bind to (string from in dec)
	 * 	menu: the name of the menu object
	 *
	 * 	Bind a menu object to a toplevel window.
	 * 	*/
	IPC_DISPATCHER_REGISTER("BindMenu", BindMenu, 
			IPC_IN("window", "menu"),
			IPC_OUT("VOID"),
			NULL);
	/* UnbindMenu:
	 * 	window: the toplevel GdkNativeWindow to bind to (string from in dec)
	 * 	menu: the name of the menu object
	 *
	 * 	Unbind a menu object to a toplevel window.
	 * 	*/
	IPC_DISPATCHER_REGISTER("UnbindMenu", UnbindMenu, 
			IPC_IN("window", "menu"),
			IPC_OUT("VOID"),
			NULL);
	/* LookupWindow:
	 * 	window: the toplevel window you are interested in.
	 *
	 * 	Returns:
	 * 		the cid associated with the window.
	 */
	IPC_DISPATCHER_REGISTER("LookupWindow", LookupWindow, 
			IPC_IN("window"),
			IPC_OUT("result"),
			NULL);
	/* ListClients:
	 * 	void
	 *
	 * 	Returns:
	 * 	 a GMarkup text segment describing the clients and their toplevels with menus.
	 * 	*/
	IPC_DISPATCHER_REGISTER("ListClients", ListClients, 
			IPC_IN("VOID"),
			IPC_OUT("result"),
			NULL);
	if(!ipc_server_listen(client_create_callback, client_destroy_callback, NULL)) {
		g_critical("server already there");
		return 1;
	}
	gtk_main();
	return 0;
}
