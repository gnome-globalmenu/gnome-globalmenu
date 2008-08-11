#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<Menu>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gdk/gdkx.h>
#include "ipcclient.h"

//TODO:
//Local cache and server crash recovery.

static GData * object_list = NULL;
static gchar * get_native_object_name(GtkWidget * widget){
	return g_object_get_data(G_OBJECT(widget), "native-menu-object");
}
static void introspect_property(GString * string, gchar * propname, gchar * value){
	g_string_append_printf(string, "<p name=\"%s\">%s</p>",
			propname, value);

}
gchar * guess_title(GtkWidget * item){
	GtkLabel * label = gtk_bin_get_child(GTK_BIN(item));
	if(GTK_IS_LABEL(label)){
		return gtk_label_get_text(label);
	} else {
		if(GTK_IS_SEPARATOR_MENU_ITEM(item)){
			return "|";	
		}
	}
	return get_native_object_name(item);
}
static void introspect_item(GString * string, GtkWidget * item) {
	gboolean visible;
	g_object_get(item, "visible", &visible, NULL);

	if(visible) {
		gchar * title = g_markup_escape_text(guess_title(item), -1);
		GtkWidget * submenu =gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
		g_string_append_printf(string, "<item name=\"%s\">", get_native_object_name(item));
		introspect_property(string, "title", title);
		
		if(submenu){
			gchar * submenu_str = g_markup_escape_text(get_native_object_name(submenu), -1);
			introspect_property(string, "submenu", submenu_str);
			g_free(submenu_str);
		}
		g_string_append_printf(string, "</item>\n");
		g_free(title);
	}
}
static void introspect_menu_foreach(GtkWidget * widget, gpointer foo[]){
	GString * string = foo[0];
	introspect_item(string, widget);
}
static void introspect_menu(GString * string, GtkWidget * menu){
	gpointer foo[] = { string };
	gchar * name = g_markup_escape_text(get_native_object_name(menu), -1);
	g_string_append_printf(string, "<menu name=\"%s\">\n", name);
	gtk_container_foreach(menu, introspect_menu_foreach, foo);
	g_string_append_printf(string, "</menu>\n");
	g_free(name);
}
static gboolean QueryMenu(IPCCommand * command, gpointer data){
	gchar * objectname = IPCParam(command, "menu");
	GtkWidget * menu_shell = GTK_WIDGET(g_datalist_get_data(&object_list, objectname));
	if(GTK_IS_MENU_SHELL(menu_shell)){
		GString * string = g_string_new("");
		introspect_menu(string, menu_shell);
		IPCRet(command, g_string_free(string, FALSE));
		return TRUE;
	} else {
		g_warning("the menu_shell is missing");
		return FALSE;
	}
}
static gboolean ActivateItem(IPCCommand * command, gpointer data){
	gchar * objectname = IPCParam(command, "item");
	GtkWidget * menu_item = g_datalist_get_data(&object_list, objectname);
	if(GTK_IS_MENU_ITEM(menu_item)){
		gtk_menu_item_activate(menu_item);	
		IPCRetDup(command, "OK");	
		return TRUE;
	} else {
		return FALSE;
	}
}

gboolean gnomenu_init(){
	ipc_dispatcher_register_cmd("QueryMenu", QueryMenu, NULL);
	ipc_dispatcher_register_cmd("ActivateItem", ActivateItem, NULL);
	g_datalist_init(&object_list);
	if(!ipc_client_start(NULL, NULL)) {
		return FALSE;
	}
	return TRUE;
}
static void toggle_ref_notify(gchar * object_name, GObject * object, gboolean is_last){
	if(is_last){
		gnomenu_unwrap_widget(g_quark_try_string(object));
	}
}
GQuark gnomenu_wrap_widget(GtkWidget * widget){
	static guint id = 99;
	gchar * name = g_strdup_printf("Widget%d", id++);
	LOG("Creating %s", name);
	GQuark object = g_quark_from_string(name);
	g_object_set_data(widget, "native-menu-object", g_quark_to_string(object));
	g_object_add_toggle_ref(widget, toggle_ref_notify, g_quark_to_string(object));
	g_datalist_id_set_data_full(&object_list, object, widget, NULL);
	g_free(name);
	return object;
}
GtkWidget * gnomenu_find_widget(GQuark object){
	return g_datalist_id_get_data(&object_list, object);
}
void gnomenu_unwrap_widget(GQuark object){
	GtkWidget * widget = g_datalist_id_get_data(&object_list, object);
	if(widget) {
		g_object_remove_toggle_ref(widget, toggle_ref_notify, NULL);
		g_object_set_data(widget, "native-menu-object", NULL);
		g_datalist_id_remove_data(&object_list, object);
	}
}
void gnomenu_bind_menu(GdkWindow * window, GtkWidget * menubar){
	gchar * window_str = g_strdup_printf("%ld", GDK_WINDOW_XWINDOW(window));
	ipc_client_call(NULL, "BindMenu", NULL, "window", window_str, "menu", get_native_object_name(menubar), NULL); 
	g_free(window_str);
}
void gnomenu_unbind_menu(GdkWindow * window, GtkWidget * menubar){
	gchar * window_str = g_strdup_printf("%ld", GDK_WINDOW_XWINDOW(window));
	ipc_client_call(NULL, "UnbindMenu", NULL, "window", window_str, "menu", get_native_object_name(menubar), NULL); 
	g_free(window_str);
}
