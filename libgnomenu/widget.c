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

gchar * gtk_widget_introspect(GtkWidget * widget){
	gchar * rt;
		g_object_set_data_full(widget, "widget-introspection", _introspect(widget), g_free);
		rt = g_object_get_data(widget, "widget-introspection");
	return rt;
}
