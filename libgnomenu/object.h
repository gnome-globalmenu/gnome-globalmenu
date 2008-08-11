#ifndef _GNOMENU_OBJECT_H_
#define _GNOMENU_OBJECT_H_
typedef struct _ObjectGroup ObjectGroup;
typedef struct {
	gchar * name;
	ObjectGroup * group;
	gint ref_count;
	GList * children;
	GData * properties;
} Object;
struct _ObjectGroup {
	gchar * name;
	GHashTable * object_hash;
};

ObjectGroup * create_object_group(gchar * name);
ObjectGroup * lookup_object_group(gchar * name);
void destroy_object_group(ObjectGroup * group);
gboolean create_object(ObjectGroup * group, gchar * name);
gboolean destroy_object(ObjectGroup * group, gchar * name);
gboolean set_property(ObjectGroup * group, gchar * name, gchar * prop, gchar * value);
gboolean insert_child(ObjectGroup * group, gchar * name, gchar * child, gint pos);
gboolean remove_child(ObjectGroup * group, gchar * name, gchar * child);
gchar * introspect_object(ObjectGroup * group, gchar * name);
gboolean parse_objects(ObjectGroup * group, gchar * string);
gchar * list_objects(ObjectGroup * group);

#endif
