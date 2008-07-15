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
	int foo;
} GnomenuGlobalMenuPrivate;

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
static gpointer build_menu_bar(GdkNativeWindow xwindow){
	GdkWindow * window = gdk_window_lookup(xwindow);
	if(!window) window = gdk_window_foreign_new(xwindow);
	if(window){
		Builder * builder;
		GnomenuMenuBar * built_menubar;
		gchar * built_menubar_name ;
		builder = builder_new();
		gchar * introspection = gdkx_tools_get_window_prop(window, gdk_atom_intern("GNOMENU_MENU_BAR", FALSE), NULL);
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
static void sms_filter(GnomenuGlobalMenu * self, GnomenuSMS * sms, gint size){
	GET_OBJECT(self, global_menu, priv);
	gpointer key;
	GnomenuMenuBar * menu_bar;
	switch(sms->action) {
	case MENUBAR_ACTIVATED:
		if(self->auto_switch){
			key = sms->w[0];
			gnomenu_global_menu_switch(self, key);
		}
		break;
	case INVALIDATE_MENUBAR:
		key = sms->w[0];
		if(key == self->active_key){
			gtk_widget_unparent(global_menu->active_menu_bar);
			/*ref = 1*/
		}
		menu_bar = build_menu_bar(key);
		g_hash_table_replace(self->cache, key, menu_bar);
		/*ref = 0*/
		if(key == self->active_key){
			global_menu->active_menu_bar = menu_bar;
			gtk_widget_set_parent(menu_bar, self);
		}
		break;
	}
}

static void gnomenu_global_menu_init (GnomenuGlobalMenu * self) {
	self->cache = g_hash_table_new_full(
			g_direct_hash,
			g_direct_equal,
			NULL,
			g_object_unref);

	gdkx_tools_add_sms_filter(sms_filter, self);
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
	GnomenuMenuBar * menu_bar = g_hash_table_lookup(self->cache, key);
	if(!menu_bar){
		menu_bar = build_menu_bar(key);
		g_hash_table_insert(self->cache, key, menu_bar);
	}
	if(global_menu->active_menu_bar){
		gtk_widget_unparent(global_menu->active_menu_bar);
	}
	global_menu->active_key = key;
	global_menu->active_menu_bar = menu_bar;
	gtk_widget_set_parent(menu_bar, self);
}
