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

G_DEFINE_TYPE(GnomenuGlobalMenu, gnomenu_global_menu, GTK_TYPE_CONTAINER);
typedef struct {
	GCache * cache;
} GnomenuGlobalMenuPrivate;
static gpointer g_direct_dup(gpointer p){
	return p;
}
static void g_direct_destroy(gpointer p){

}
static void _s_activate(GtkMenuItem * menu_item, gpointer data){
	gpointer handle = g_object_get_data(menu_item, "introspect-handle");
	GnomenuSMS sms;
	g_message("menu item activated, handle = %p", handle);
	sms.action = MENUITEM_CLICKED;
	sms.p[0] = handle;
	gdkx_tools_send_sms(&sms, sizeof(sms));
}
static void setup_handler(gchar * id, GtkWidget * widget, gpointer data){
	if(GTK_IS_MENU_ITEM(widget)){
		g_signal_connect(widget,
				"activate",
				_s_activate, data);
	}
}
static gpointer _new_menu_bar(GdkNativeWindow xwindow){
	GdkWindow * window = gdk_window_lookup(xwindow);
	if(!window) window = gdk_window_foreign_new(xwindow);
	if(window){
		Builder * builder;
		GnomenuMenuBar * built_menubar;
		gchar * built_menubar_name ;
		builder = builder_new();
		gchar * introspection = gdkx_tools_get_window_prop(window, gdk_atom_intern("GNOMENU_MENU_BAR", FALSE), NULL);
		g_print("%s", introspection);
		builder_parse(builder, introspection);
		built_menubar_name = g_strdup_printf("%p", xwindow);
		built_menubar = builder_get_object(builder, built_menubar_name);
		built_menubar = g_object_ref(built_menubar);
		gnomenu_menu_bar_set_show_arrow(built_menubar, TRUE);
		g_free(built_menubar_name);
		builder_foreach(builder, setup_handler, NULL); 
		g_free(introspection);
		builder_destroy(builder);
		return built_menubar;
	}
	return NULL;
}
static void sms_filter(GtkWidget * self, GnomenuSMS * sms, gint size){
	GET_OBJECT(self, global_menu, priv);
	gpointer key;
	if(sms->action != MENUBAR_ACTIVATED) return;
	key = sms->w[0];
	GnomenuMenuBar * active_menu_bar = g_cache_insert(
				priv->cache, key);
	if(global_menu->active_menu_bar){
		gtk_widget_unparent(global_menu->active_menu_bar);
	}
	global_menu->active_menu_bar = active_menu_bar;
	gtk_widget_set_parent(global_menu->active_menu_bar, self);
}

static void gnomenu_global_menu_init (GnomenuGlobalMenu * self) {
	GnomenuGlobalMenuPrivate * priv = GNOMENU_GLOBAL_MENU_GET_PRIVATE(self);
	priv->cache = g_cache_new(
			_new_menu_bar,
			g_object_unref,
			g_direct_dup,
			g_direct_destroy,
			g_direct_hash,
			g_direct_hash,
			g_direct_equal);
	gdkx_tools_add_sms_filter(sms_filter, self);
	GTK_WIDGET_SET_FLAGS(self, GTK_NO_WINDOW);
}
static void _finalize(GnomenuGlobalMenu * global_menu) {
	GnomenuGlobalMenuPrivate * priv = GNOMENU_GLOBAL_MENU_GET_PRIVATE(global_menu);
	g_cache_destroy(priv->cache);
	if(G_OBJECT_CLASS(gnomenu_global_menu_parent_class)->finalize)
		G_OBJECT_CLASS(gnomenu_global_menu_parent_class)->finalize (global_menu);
}
static void _size_allocate(GnomenuGlobalMenu * global_menu, GtkAllocation * allocation) {
	gtk_widget_size_allocate(global_menu->active_menu_bar, allocation);
	GTK_WIDGET_CLASS(gnomenu_global_menu_parent_class)->size_allocate(global_menu, allocation);
}
static void _size_request(GnomenuGlobalMenu * global_menu, GtkRequisition * requisition) {
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
