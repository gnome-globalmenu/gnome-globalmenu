#include <config.h>
#include <glib.h>
#include "object.h"

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuServer>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif

static GHashTable * object_hash;

static void object_unref(Object * object) {
	object->ref_count--;
	if(object->ref_count == 0) {
		GList * node;
		for(node = object->children; node; node = node->next)
			object_unref(node->data);
		g_list_free(object->children);
		g_datalist_clear(&object->properties);
		g_slice_free(Object, object);
		g_hash_table_remove(object_hash, object->name);
		g_free(object->name);
	}
}
void object_manager_init() {
	object_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);
}
gboolean create_object(gchar * name) {
	g_return_val_if_fail(name, FALSE);
	if(g_hash_table_lookup(object_hash, name)) return FALSE;
	Object * object = g_slice_new0(Object);
	object->name = g_strdup(name);
	object->ref_count = 1;
	object->children = NULL;
	g_datalist_init(&object->properties);
	g_hash_table_insert(object_hash, object->name, object);
	return TRUE;
}
gboolean destroy_object(gchar * name){
	g_return_val_if_fail(name, FALSE);
	Object * object = g_hash_table_lookup(object_hash, name);
	g_return_val_if_fail(object, FALSE);
	object_unref(object);
	return TRUE;
}
gboolean set_property(gchar * name, gchar * prop, gchar * value){
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(prop, FALSE);
	Object * object = g_hash_table_lookup(object_hash, name);
	g_return_val_if_fail(object, FALSE);
	if(value) {
		g_datalist_set_data_full(&object->properties, prop, g_strdup(value), g_free);
	} else {
		g_datalist_remove_data(&object->properties, prop);
	}
	return TRUE;
}
gboolean activate_object(gchar * name){
	return FALSE;
}
gboolean insert_child(gchar * name, gchar * child, gint pos) {
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(child, FALSE);
	Object * object = g_hash_table_lookup(object_hash, name);
	Object * obj_child = g_hash_table_lookup(object_hash, child);
	g_return_val_if_fail(object, FALSE);
	g_return_val_if_fail(child, FALSE);
	object->children = g_list_insert(object->children, obj_child, pos);
	obj_child->ref_count++;
	return TRUE;
}
gboolean remove_child(gchar * name, gchar * child) {
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(child, FALSE);
	Object * object = g_hash_table_lookup(object_hash, name);
	Object * obj_child = g_hash_table_lookup(object_hash, child);
	g_return_val_if_fail(object, FALSE);
	g_return_val_if_fail(child, FALSE);
	object->children = g_list_remove(object->children, obj_child);
	object_unref(obj_child);
	return TRUE;
}
gchar * introspect_object(gchar * name){
	g_return_val_if_fail(name, NULL);
	Object * object = g_hash_table_lookup(object_hash, name);
	g_return_val_if_fail(object, NULL);
	GString * string = g_string_new("");
	g_string_append_printf(string, "%s ", name);
	GList * node;
	for(node = object->children; node; node = node->next){
		Object * child = node->data;
		g_string_append_printf(string, "%s", introspect_object(child->name));
	}
	return g_string_free(string, FALSE);
}
gboolean parse_objects(gchar * string){
	g_return_val_if_fail(string, FALSE);
	return FALSE;
}
