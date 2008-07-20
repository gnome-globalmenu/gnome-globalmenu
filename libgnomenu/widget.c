#include <config.h>
#include <gtk/gtk.h>
#include "introspector.h"
int GNOMENU_INTROSPECT_FLAGS = INTROSPECT_HANDLE;
static guint IS_INVALIDATED = 0;

gchar * gtk_widget_get_id(GtkWidget * widget){
	return g_object_get_data(widget, "introspect-id");
}
void gtk_widget_set_id(GtkWidget * widget, gchar * id){
		g_object_set_data_full(widget, "introspect-id", g_strdup(id), g_free);
}
static gchar * _introspect(GtkWidget * widget){
		Introspector * spector = introspector_new();
		introspector_set_flags(spector, GNOMENU_INTROSPECT_FLAGS);
		introspector_queue_widget(spector, widget);
		introspector_visit_all(spector);
	return introspector_destroy(spector, FALSE);
}
static gboolean _expose_event(GtkWidget * widget, GdkEvent * event, gpointer p){
#ifdef INVALIDATE_INTROSPECTION
	gchar * data = _introspect(widget);
	if(!g_str_equal(data, g_object_get_data(widget, "widget-introspection"))) {
		g_object_set_data_full(widget, "widget-introspection", data, g_free);
		g_signal_emit(widget, IS_INVALIDATED, 0);
		g_message("invalidate introspection");
	}
#endif
	return FALSE;
}

gchar * gtk_widget_introspect(GtkWidget * widget){
	gchar * rt;
#ifdef INVALIDATE_INTROSPECTION

	if(G_UNLIKELY(!IS_INVALIDATED)){
		/*this signal is not working*/
		IS_INVALIDATED = g_signal_new(
				"introspection-invalidated",
				GTK_TYPE_WIDGET,
				G_SIGNAL_RUN_LAST,
				0,
				NULL,
				NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE,
				0);
	}
	rt = g_object_get_data(widget, "widget-introspection");
	if(G_UNLIKELY(!rt)) {
		g_signal_connect(widget, "expose-event", _expose_event, NULL);
#endif
		g_object_set_data_full(widget, "widget-introspection", _introspect(widget), g_free);
		rt = g_object_get_data(widget, "widget-introspection");
#ifdef INVALIDATE_INTROSPECTION
	}
#endif
	return rt;
}
typedef struct {
	gchar * name;
	gpointer data;
} tree_set_data_t;

static void _tree_transverse (GtkWidget * widget, tree_set_data_t * data) {
	g_object_set_data(widget, data->name, data->data);
	if(GTK_IS_CONTAINER(widget)){
		gtk_container_forall(widget, _tree_transverse, data);
	}
	if(GTK_IS_MENU_ITEM(widget)){
		GtkMenu * submenu = gtk_menu_item_get_submenu(widget);
		if(G_TYPE_IS_OBJECT(submenu)) {
			_tree_transverse(submenu, data);
			g_message("visiting submenu");
		}
	}
}
void gtk_widget_tree_set_data(GtkWidget * widget, gchar * name, gpointer data){
	tree_set_data_t pass_to_children = { name, data};
	_tree_transverse(widget, &pass_to_children);
}
