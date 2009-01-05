#include <gtk/gtk.h>

#include "dyn-patch-vfunc.h"
extern GHashTable * old_vfuncs;
extern GHashTable * classes;
void dyn_patch_save_vfunc(const char * type, const char * name, gpointer vfunc) {
	char * long_name = g_strdup_printf("%s_%s", type, name);
	g_hash_table_insert(old_vfuncs, long_name, vfunc);
}

gpointer dyn_patch_hold_type(GType type) {
	gpointer klass = g_type_class_ref(type);
	g_hash_table_insert(classes, GINT_TO_POINTER(type), klass);
	return klass;
}
void dyn_patch_release_type(GType type) {
	g_hash_table_remove(classes, GINT_TO_POINTER(type));
}

gpointer dyn_patch_load_vfunc(const char * type, const char * name) {
	char * long_name = g_strdup_printf("%s_%s", type, name);
	gpointer rt = g_hash_table_lookup(old_vfuncs, long_name);
	g_free(long_name);
	return rt;
}

void dyn_patch_type(GType type, DynPatcherFunc patcher) {
	dyn_patch_type_r(type, patcher);
}

void dyn_patch_type_r(GType type, DynPatcherFunc patcher) {
	GType * children = g_type_children(type, NULL);
	int i;
	patcher(type);
	for(i = 0; children[i]; i++) {
		dyn_patch_type_r(children[i], patcher);
	}
	g_free(children);
}
