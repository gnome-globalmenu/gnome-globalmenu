#include <config.h>
#include <glib.h>
#include "object.h"

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<GnomenuServer>::" fmt "\n",  ## args)
#else
#define LOG(fmt, args...)
#endif
static GHashTable * group_hash = NULL;
static void object_destroy(Object * object) {
	if(object->ref_count > 0)
		LOG("%s is leaked ref_count = %d",  object->name, object->ref_count);
	g_list_free(object->children);
	g_datalist_clear(&object->properties);
	g_hash_table_remove(object->group->object_hash, object->name);
	g_free(object->name);
	g_slice_free(Object, object);
}
static void object_unref(Object * object) {
	object->ref_count--;
	if(object->ref_count == 0) {
		GList * node;
		for(node = object->children; node; node = node->next)
			object_unref(node->data);
		object_destroy(object);
	}
}
ObjectGroup * create_object_group(gchar * name) {
	ObjectGroup * group = g_slice_new0(ObjectGroup);
	group->name =g_strdup(name);
	group->object_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);
	if(!group_hash){
		group_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);
	}
	g_hash_table_insert(group_hash, group->name, group);
	return group;
}
ObjectGroup * lookup_object_group(gchar * name){
	ObjectGroup * group =g_hash_table_lookup(group_hash, name);
	return group;
}
void destroy_object_group(ObjectGroup * group) {
	GList * keys = g_hash_table_get_keys(group->object_hash);
	GList * node;
	for(node = keys; node; node=node->next){
		Object * obj = (g_hash_table_lookup(group->object_hash, node->data));
		object_destroy(obj);
	}
	g_hash_table_destroy(group->object_hash);
	g_hash_table_remove(group_hash, group->name);
	g_free(group->name);
	g_slice_free(ObjectGroup, group);
}
gboolean create_object(ObjectGroup * group, gchar * name) {
	g_return_val_if_fail(name, FALSE);
	if(g_hash_table_lookup(group->object_hash, name)) return FALSE;
	Object * object = g_slice_new0(Object);
	object->name = g_strdup(name);
	object->group = group;
	object->ref_count = 1;
	object->children = NULL;
	g_datalist_init(&object->properties);
	g_hash_table_insert(group->object_hash, object->name, object);
	return TRUE;
}
gboolean destroy_object(ObjectGroup * group, gchar * name){
	g_return_val_if_fail(name, FALSE);
	Object * object = g_hash_table_lookup(group->object_hash, name);
	g_return_val_if_fail(object, FALSE);
	object_unref(object);
	return TRUE;
}
gboolean set_property(ObjectGroup * group, gchar * name, gchar * prop, gchar * value){
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(prop, FALSE);
	Object * object = g_hash_table_lookup(group->object_hash, name);
	g_return_val_if_fail(object, FALSE);
	if(value) {
		g_datalist_set_data_full(&object->properties, prop, g_strdup(value), g_free);
	} else {
		g_datalist_remove_data(&object->properties, prop);
	}
	return TRUE;
}
gboolean insert_child(ObjectGroup * group, gchar * name, gchar * child, gint pos) {
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(child, FALSE);
	Object * object = g_hash_table_lookup(group->object_hash, name);
	Object * obj_child = g_hash_table_lookup(group->object_hash, child);
	g_return_val_if_fail(object, FALSE);
	g_return_val_if_fail(child, FALSE);
	object->children = g_list_insert(object->children, obj_child, pos);
	obj_child->ref_count++;
	return TRUE;
}
gboolean remove_child(ObjectGroup * group, gchar * name, gchar * child) {
	g_return_val_if_fail(name, FALSE);
	g_return_val_if_fail(child, FALSE);
	Object * object = g_hash_table_lookup(group->object_hash, name);
	Object * obj_child = g_hash_table_lookup(group->object_hash, child);
	g_return_val_if_fail(object, FALSE);
	g_return_val_if_fail(child, FALSE);
	object->children = g_list_remove(object->children, obj_child);
	object_unref(obj_child);
	return TRUE;
}
static void prepend(GString * string, guint level){
	guint i;
	for(i=0; i<level; i++){
		g_string_append_c(string, ' ');
	}
}
static void for_each_property(GQuark key_id, gpointer data, gpointer para[2]){
	gchar * escaped = g_markup_escape_text(data, -1);
	GString * string = para[0];
	gint * level = para[1];
	prepend(string, *level+1);
	g_string_append_printf(string, "<prop name=\"%s\">%s</prop>\n",
			g_quark_to_string(key_id), escaped);
	g_free(escaped);
}
static void introspect(GString * string, Object * object, guint level){
	prepend(string, level);
	g_string_append_printf(string, "<object name=\"%s\">\n", object->name);
	prepend(string, level + 1);
	g_string_append_printf(string, "<children>\n");
	GList * node;
	for(node = object->children; node; node = node->next){
		Object * child = node->data;
		introspect(string, child, level + 2);
	}
	prepend(string, level + 1);
	g_string_append_printf(string, "</children>\n");
	gpointer para[2] = {string, &level};
	g_datalist_foreach(&object->properties, for_each_property, para);
	prepend(string, level);
	g_string_append_printf(string, "</object>\n");
}
gchar * introspect_object(ObjectGroup * group, gchar * name){
	g_return_val_if_fail(name, NULL);
	Object * object = g_hash_table_lookup(group->object_hash, name);
	g_return_val_if_fail(object, NULL);
	GString * string = g_string_new("");
	introspect(string, object, 0);
	return g_string_free(string, FALSE);
}
gchar * list_objects(ObjectGroup * group){
	GHashTableIter iter;
	gchar * key;
	Object * value;
	g_hash_table_iter_init(&iter, group->object_hash);
	GString * string = g_string_new("");
	while(g_hash_table_iter_next(&iter, &key, &value)){
		g_string_append_printf(string, "%s\n", key);
	}
	return g_string_free(string, FALSE);
}
gboolean parse_objects(ObjectGroup * group, gchar * string){
	g_return_val_if_fail(string, FALSE);
	return FALSE;
}
