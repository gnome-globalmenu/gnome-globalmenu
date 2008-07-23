#include <config.h>
#include <gtk/gtk.h>
#define GET_OBJECT(_s, sgmb, p) \
	GnomenuGlobalMenu * sgmb = GNOMENU_GLOBAL_MENU(_s); \
	GnomenuGlobalMenuPrivate * p = GNOMENU_GLOBAL_MENU_GET_PRIVATE(_s);

#define GNOMENU_GLOBAL_MENU_GET_PRIVATE(o) G_TYPE_INSTANCE_GET_PRIVATE(o, GNOMENU_TYPE_GLOBAL_MENU, GnomenuGlobalMenuPrivate)

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuGlobalMenu>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include "globalmenu.h"
#include "menubar.h"
#include "widget.h"
#include "builder.h"
#include "tools.h"
#include "sms.h"
#include <gdk/gdkx.h>
G_DEFINE_TYPE(GnomenuGlobalMenu, gnomenu_global_menu, GTK_TYPE_CONTAINER);
static gboolean parsing = FALSE;
typedef struct {
	int foo;
} GnomenuGlobalMenuPrivate;
typedef struct {
	Builder * builder;
	GnomenuMenuBar * menu_bar;
	gchar * name;
} MenuBarInfo;

GdkNativeWindow get_sms_window(GdkNativeWindow key) {
	GdkNativeWindow * pwindow = gdkx_tools_get_window_prop(
			gdkx_tools_lookup_window(key),
			"GNOMENU_SMS_LISTENER", NULL);
	GdkNativeWindow window = 0;
	if(pwindow) {
		window = * pwindow;
		g_free(pwindow);
	}
	return window;
}
static void _s_activate(GtkMenuItem * menu_item, GtkWidget * menubar){
	gpointer handle = g_object_get_data(menu_item, "introspect-handle");
	GnomenuGlobalMenu * global_menu =
		gtk_widget_get_parent(menubar);
	g_return_if_fail(GNOMENU_IS_GLOBAL_MENU(global_menu));
//	if(menu_item->submenu != NULL) return;
	GnomenuSMS sms;
	LOG("menu item activated, handle = %p", handle);
	sms.action = MENUITEM_CLICKED;
	sms.p[0] = handle;
	sms.p[1] = global_menu->active_key;
	LOG("key = %p", global_menu->active_key);
	gdkx_tools_send_sms_to(get_sms_window(global_menu->active_key), &sms, sizeof(sms));
}
static void remove_handler(gchar * id, GtkWidget * widget, gpointer data){
	if(GTK_IS_MENU_ITEM(widget)){
		g_signal_handlers_disconnect_matched(widget, 
			G_SIGNAL_MATCH_FUNC ,	
			g_signal_lookup("activate", GTK_TYPE_MENU_ITEM),
			0, NULL, _s_activate, NULL);
	}
}
static void setup_handler(gchar * id, GtkWidget * widget, gpointer data){
	if(GTK_IS_MENU_ITEM(widget)){
		g_signal_connect(widget,
				"activate",
				_s_activate, data);
	}
}
static gchar * get_introspection(GdkNativeWindow key) {
	GdkWindow * window = gdkx_tools_lookup_window(get_sms_window(key));
	if(window){
		gchar * introspection = gdkx_tools_get_window_prop(window, "GNOMENU_MENU_BAR", NULL);
		return introspection;
	}
	return NULL;
}
static gchar * get_partial_introspection(GdkNativeWindow key) {
	GdkWindow * window = gdkx_tools_lookup_window(get_sms_window(key));
	if(window){
		gchar * introspection = gdkx_tools_get_window_prop(window, "GNOMENU_MENU_BAR_PART", NULL);
		return introspection;
	}
	return NULL;
}
static MenuBarInfo * create_menu_bar_info(GnomenuGlobalMenu * self, GdkNativeWindow key){
	MenuBarInfo * info = g_hash_table_lookup(self->cache, key);
	if(info) return info;

	info = g_slice_new0(MenuBarInfo);
	GnomenuMenuBar * menu_bar;
	gchar * introspection = get_introspection(key);
	if(introspection){
		info->builder = builder_new();
		builder_foreach(info->builder, remove_handler, info->menu_bar); 
		builder_parse(info->builder, introspection);
		builder_foreach(info->builder, setup_handler, info->menu_bar); 
		info->name = g_strdup_printf("%p", key);
		info->menu_bar = builder_get_object(info->builder, info->name);
		g_free(introspection);
		g_hash_table_insert(self->cache, key, info);
		return info;
	}
	return NULL;
}
static void destroy_menu_bar_info(MenuBarInfo * info){
	builder_destroy(info->builder);
	g_free(info->name);
	g_slice_free(MenuBarInfo, info);
}

static void sms_filter(GnomenuGlobalMenu * self, GnomenuSMS * sms, gint size){
	GET_OBJECT(self, global_menu, priv);
	GdkNativeWindow key;
	gpointer handle;
	GnomenuMenuBar * menu_bar;
	gchar * introspection = NULL;
	switch(sms->action) {
	case INTROSPECTION_PARTIALLY_UPDATED:
		key = sms->w[0];
		introspection = get_partial_introspection(key);
	case INTROSPECTION_UPDATED:
		key = sms->w[0];
		if(!introspection)
			introspection = get_introspection(key);
		handle = sms->w[1];
		LOG("received updated introspection: %p->%p", key, handle);
		MenuBarInfo * info = create_menu_bar_info(self, key);
		if(!info) break;
		if(introspection) {
			builder_foreach(info->builder, remove_handler, info->menu_bar); 
			builder_parse(info->builder, introspection);
			builder_foreach(info->builder, setup_handler, info->menu_bar); 
			g_free(introspection);
		} else {
			builder_cleanup(info->builder);
			if(key == self->active_key) {
				if(self->active_menu_bar) 
					gtk_widget_unparent(self->active_menu_bar);
				self->active_menu_bar = NULL;
			}
		}
		break;
	}
}

static void gnomenu_global_menu_init (GnomenuGlobalMenu * self) {
	self->cache = g_hash_table_new_full(
			g_direct_hash,
			g_direct_equal,
			NULL,
			destroy_menu_bar_info);

	gdkx_tools_add_sms_filter(NULL, sms_filter, self, FALSE);
	GTK_WIDGET_SET_FLAGS(self, GTK_NO_WINDOW);
}
static void _finalize(GnomenuGlobalMenu * global_menu) {
	g_hash_table_destroy(global_menu->cache);
	gdkx_tools_remove_sms_filter(sms_filter, global_menu);
	if(G_OBJECT_CLASS(gnomenu_global_menu_parent_class)->finalize)
		G_OBJECT_CLASS(gnomenu_global_menu_parent_class)->finalize (global_menu);
}
static void _size_allocate(GnomenuGlobalMenu * global_menu, GtkAllocation * allocation) {
	if(global_menu->active_menu_bar)
		gtk_widget_size_allocate(global_menu->active_menu_bar, allocation);
	GTK_WIDGET_CLASS(gnomenu_global_menu_parent_class)->size_allocate(global_menu, allocation);
}
static void _size_request(GnomenuGlobalMenu * global_menu, GtkRequisition * requisition) {
	if(global_menu->active_menu_bar)
		gtk_widget_size_request(global_menu->active_menu_bar, requisition);
//	GTK_WIDGET_CLASS(gnomenu_global_menu_parent_class)->size_request(global_menu, requisition);
}
static void gnomenu_global_menu_class_init (GnomenuGlobalMenuClass * klass) {
	GObjectClass * obj_class = (GObjectClass *)klass;
	GtkWidgetClass * widget_class = (GtkWidgetClass *) klass;
	obj_class->finalize = _finalize;
	widget_class->size_allocate = _size_allocate;
	widget_class->size_request = _size_request;
	g_type_class_add_private(klass, sizeof(GnomenuGlobalMenuPrivate));
}
GtkWidget * gnomenu_global_menu_new(){
	return g_object_new(GNOMENU_TYPE_GLOBAL_MENU, NULL);
}
void gnomenu_global_menu_switch(GnomenuGlobalMenu * self, gpointer key){
	GET_OBJECT(self, global_menu, priv);
	LOG("switch: key = %p", key);
	MenuBarInfo * menu_bar_info;
	g_hash_table_remove(self->cache, global_menu->active_key);
	/*Work around to disable the cache*/
	menu_bar_info = create_menu_bar_info(self, key);
	if(global_menu->active_menu_bar){
		gtk_widget_unparent(global_menu->active_menu_bar);
	}
	global_menu->active_key = key;
	global_menu->active_menu_bar = menu_bar_info?menu_bar_info->menu_bar:NULL;
	if(global_menu->active_menu_bar)
		gtk_widget_set_parent(global_menu->active_menu_bar, self);
	GnomenuSMS sms;
	sms.action = UPDATE_INTROSPECTION;
	gdkx_tools_send_sms_to(get_sms_window(key), &sms, sizeof(sms));
}
