#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuServer>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif

#include "ipcserver.h"
#include "object.h"
typedef struct {
	gchar * cid;
	ObjectGroup * group;
} ClientInfo;
static GHashTable * client_hash = NULL;
ObjectGroup * global_group = NULL;
void client_info_free(ClientInfo * info) {
	g_free(info->cid);
	destroy_object_group(info->group);
	g_slice_free(ClientInfo, info);
}
static void client_create_callback(gchar * cid, gpointer data) {
	LOG("New client %s", cid);
	ClientInfo * info = g_slice_new0(ClientInfo);
	info->cid = g_strdup(cid);
	info->group = create_object_group(info->cid);
	g_hash_table_insert(client_hash, info->cid, info);
}
static void client_destroy_callback(gchar * cid, gpointer data) {
	LOG("Dead client %s", cid);
	g_hash_table_remove(client_hash, cid);
}
gboolean Unimplemented(IPCCommand * command, gpointer data) {
	IPCRet(command, g_strdup("This method is Unimplemented"));
	return TRUE;
}
ObjectGroup * find_group(IPCCommand * command){
	gchar * persist = IPCParam(command, "persist");
	if(persist && g_str_equal(persist, "true")) {
		return global_group;
	}
	gchar * cid = command->cid;
	ClientInfo * info = g_hash_table_lookup(client_hash, cid);
	if(!info) return NULL;
	return info->group;
}
gboolean CreateObject(IPCCommand * command, gpointer data) {
	gchar * objname = IPCParam(command, "object");
	IPCRetBool(command, create_object(find_group(command), objname));
	return TRUE;
}
gboolean DestroyObject(IPCCommand * command, gpointer data) {
	gchar * objname = IPCParam(command, "object");
	IPCRetBool(command, destroy_object(find_group(command), objname));
	return TRUE;
}
gboolean SetProperty(IPCCommand * command, gpointer data) {
	gchar * objname = IPCParam(command, "object");
	gchar * property = IPCParam(command, "property");
	gchar * value = IPCParam(command, "value");
	IPCRetBool(command, set_property(find_group(command), objname, property, value));
	return TRUE;
}
gboolean InsertChild(IPCCommand * command, gpointer data) {
	gchar * objname = IPCParam(command, "object");
	gchar * childname = IPCParam(command, "child");
	gchar * spos = IPCParam(command, "pos");
	gint pos = strtol(spos, NULL, 10);
	IPCRetBool(command, insert_child(find_group(command), objname, childname, pos));
	return TRUE;
}
gboolean RemoveChild(IPCCommand * command, gpointer data) {
	gchar * objname = IPCParam(command, "object");
	gchar * childname = IPCParam(command, "child");
	gchar * spos = IPCParam(command, "pos");
	gint pos = strtol(spos, NULL, 10);
	IPCRetBool(command, remove_child(find_group(command), objname, childname));
	return TRUE;
}
gboolean IntrospectObject(IPCCommand * command, gpointer data) {
	gchar * objname = IPCParam(command, "object");
	IPCRet(command, introspect_object(find_group(command), objname));
	return TRUE;
}
int main(int argc, char* argv[]){
	gtk_init(&argc, &argv);

	client_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, client_info_free);
	global_group = create_object_group("Global Group");
	ipc_server_register_cmd("CreateObject", CreateObject, NULL);
	ipc_server_register_cmd("DestroyObject", DestroyObject, NULL);
	ipc_server_register_cmd("SetProperty", SetProperty, NULL);
	ipc_server_register_cmd("ActivateObject", Unimplemented, NULL);
	ipc_server_register_cmd("InsertChild", InsertChild, NULL);
	ipc_server_register_cmd("RemoveChild", RemoveChild, NULL);
	ipc_server_register_cmd("IntrospectObject", IntrospectObject, NULL);
	if(!ipc_server_listen(client_create_callback, client_destroy_callback, NULL)) {
		g_error("server already there");
		return 1;
	}
	gtk_main();
	destroy_object_group(global_group);
	return 0;
}
