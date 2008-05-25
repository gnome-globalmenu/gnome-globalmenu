
typedef struct _Builder Builder;
Builder * builder_new ();
Builder * builder_destroy(Builder * builder);
void builder_parse(Builder * builder, const gchar * string);
GtkWidget * builder_get_object(Builder * builder, const gchar * id);
void builder_foreach(Builder * builder, GHFunc callback, gpointer data);
