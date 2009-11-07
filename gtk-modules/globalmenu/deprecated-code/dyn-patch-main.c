#include <gtk/gtk.h>

#include "dyn-patch-main.h"
#include "dyn-patch-vfunc.h"
#include "dyn-patch-utils.h"

extern void dyn_patch_widget_patcher();
extern void	dyn_patch_menu_shell_patcher();
extern void	dyn_patch_menu_bar_patcher();
extern void dyn_patch_widget_unpatcher();
extern void	dyn_patch_menu_shell_unpatcher();
extern void	dyn_patch_menu_bar_unpatcher();

GQuark __MENUBAR__ = 0;
GQuark __DIRTY__ = 0;
GQuark __OLD_SUBMENU__ = 0;
GQuark __ITEM__  =  0;
GQuark __IS_LOCAL__ = 0;
GQuark __TOPLEVEL__ = 0;


GTimer * timer = NULL;
GHashTable * notifiers = NULL;
GHashTable * old_vfuncs = NULL;
GHashTable * classes = NULL;

static gboolean discover_menubars();

void dyn_patch_init () {
	GDK_THREADS_ENTER();
	__MENUBAR__ = g_quark_from_string("__dyn-patch-menubar__");
	__DIRTY__ = g_quark_from_string("__dyn-patch-dirty__");
	__OLD_SUBMENU__ = g_quark_from_string("__dyn-patch-old-submenu__");
	__ITEM__ = g_quark_from_string("__dyn-patch-item__");
	__IS_LOCAL__ = g_quark_from_string("__dyn-patch-is-local__");
	__TOPLEVEL__ = g_quark_from_string("__dyn-patch-toplevel__");

	old_vfuncs = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	classes = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_type_class_unref);
	notifiers = g_hash_table_new_full(g_direct_hash, g_direct_equal, g_object_unref, (GDestroyNotify)g_source_remove);

	dyn_patch_type(GTK_TYPE_WIDGET, dyn_patch_widget_patcher);
	dyn_patch_type(GTK_TYPE_MENU_SHELL, dyn_patch_menu_shell_patcher);
	dyn_patch_type(GTK_TYPE_MENU_BAR, dyn_patch_menu_bar_patcher);

	timer = g_timer_new();
	g_timer_stop(timer);
	
	g_idle_add_full(G_PRIORITY_HIGH, discover_menubars, NULL, NULL);
	GDK_THREADS_LEAVE();
}

void dyn_patch_uninit_vfuncs() {

	dyn_patch_discover_menubars(DISCOVER_MODE_UNINIT_VFUNCS);

	dyn_patch_type(GTK_TYPE_MENU_BAR, dyn_patch_menu_bar_unpatcher);
	dyn_patch_type(GTK_TYPE_MENU_SHELL, dyn_patch_menu_shell_unpatcher);
	dyn_patch_type(GTK_TYPE_WIDGET, dyn_patch_widget_unpatcher);

}

void dyn_patch_uninit_final() {

	dyn_patch_discover_menubars(DISCOVER_MODE_UNINIT_FINAL);

	g_hash_table_unref(notifiers);
	g_hash_table_unref(old_vfuncs);
	g_hash_table_unref(classes);
	g_timer_destroy(timer);
}
static gboolean discover_menubars() {
	GDK_THREADS_ENTER();
	dyn_patch_discover_menubars(DISCOVER_MODE_INIT);
	GDK_THREADS_LEAVE();
	return FALSE;
}
