#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<Menu>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gdk/gdkx.h>
#include "ipcclient.h"

//TODO:
//Local cache and server crash recovery.

static GData * object_list = NULL;

typedef void (* GnomenuListener)(GQuark item, gpointer data);
static void activated_handler(IPCEvent * event, gpointer data){
	GQuark object = g_quark_from_string(IPCParam(event, "object"));
	/*invoke the handler*/

}
gboolean gnomenu_init(){
	if(!ipc_client_start(NULL, NULL)) {
		ipc_client_set_event("Activated", activated_handler, NULL);
		return FALSE;
	}
	return TRUE;
}
GQuark gnomenu_create(const gchar * hint){
	static guint id = 99;
	gchar * name = g_strdup_printf("%s%d", hint, id);
	GQuark object = g_quark_from_string(name);
	ipc_client_call_server("CreateObject", NULL, "object", name, NULL);
	g_free(name);
	return object;
}
gboolean gnomenu_set_property(GQuark item, gchar * property, gchar * value){
	return ipc_client_call_server("SetProperty", NULL, "object", g_quark_to_string(item), "property", property, "value", value, NULL);
}
gboolean gnomenu_get_property(GQuark item, gchar * property, gchar ** value){
	return ipc_client_call_server("GetProperty", value, "object", g_quark_to_string(item), "property", property, NULL);
}
gboolean gnomenu_insert_child(GQuark menu, GQuark item, gint pos){
	gchar * pos_str = g_strdup_printf("%d", pos);
	gboolean b = ipc_client_call_server("InsertChild", NULL, "object", g_quark_to_string(menu), "child", g_quark_to_string(item), "pos", pos_str, NULL);
	g_free(pos_str);
	return b;
}
gboolean gnomenu_remove_item(GQuark menu, GQuark item){
	return ipc_client_call_server("RemoveChild", NULL, "object", g_quark_to_string(menu), "child", g_quark_to_string(item), NULL);
}
gboolean gnomenu_destroy(GQuark object){
	return ipc_client_call_server("DestroyObject", NULL, "object", g_quark_to_string(object), NULL);
}
gboolean gnomenu_listen(GnomenuListener func, gpointer data){
}
