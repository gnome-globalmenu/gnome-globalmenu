#include <config.h>
#include <gtk/gtk.h>
#include "introspector.h"

gchar * gtk_widget_get_id(GtkWidget * widget){
	return g_object_get_data(widget, "introspect-id");
}
void gtk_widget_set_id(GtkWidget * widget, gchar * id){
		g_object_set_data_full(widget, "introspect-id", g_strdup(id), g_free);
}
gchar * gtk_widget_introspect(GtkWidget * widget){
	gchar * rt;
	Introspector * spector = introspector_new();
	introspector_queue_widget(spector, widget);
	introspector_visit_all(spector);
	return introspector_destroy(spector, FALSE);
}
gchar * gtk_widget_introspect_with_handle(GtkWidget * widget){
	gchar * rt;
	Introspector * spector = introspector_new();
	introspector_set_flags(spector, INTROSPECT_HANDLE);
	introspector_queue_widget(spector, widget);
	introspector_visit_all(spector);
	return introspector_destroy(spector, FALSE);

}

