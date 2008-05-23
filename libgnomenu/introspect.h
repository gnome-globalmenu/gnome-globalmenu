typedef struct _Introspector Introspector;
gchar * gtk_widget_introspect(GtkWidget * widget);
void introspector_queue_widget(Introspector * spector, GtkWidget * widget);
Introspector * introspector_new();
gchar * introspector_destroy(Introspector * spector, gboolean free_blob_string);
void introspector_visit_all(Introspector * spector);
