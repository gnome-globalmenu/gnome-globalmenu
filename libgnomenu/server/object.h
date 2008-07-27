#ifndef _GNOMENU_OBJECT_H_
#define _GNOMENU_OBJECT_H_
typedef struct {
	gchar * name;
	gint ref_count;
	GList * children;
	GData * properties;
} Object;

gboolean create_object(gchar * name);
gboolean destroy_object(gchar * name);
gboolean set_property(gchar * name, gchar * prop, gchar * value);
gboolean activate_object(gchar * name);
gboolean insert_child(gchar * name, gchar * child, gint pos);
gboolean remove_child(gchar * name, gchar * child);
gchar * introspect_object(gchar * name);
gboolean parse_objects(gchar * string);


void object_manager_init();


#endif
