#define	INTROSPECT_DEFAULT 0
#define	INTROSPECT_HANDLE 1

typedef int IntrospectFlags;
typedef struct _Introspector Introspector;
gchar * gtk_widget_introspect(GtkWidget * widget);
void introspector_queue_widget(Introspector * spector, GtkWidget * widget);
Introspector * introspector_new();
void introspector_set_flags(Introspector * spector, IntrospectFlags flags);
gchar * introspector_destroy(Introspector * spector, gboolean free_blob_string);
void introspector_visit_all(Introspector * spector);
extern int GNOMENU_INTROSPECT_FLAGS;
