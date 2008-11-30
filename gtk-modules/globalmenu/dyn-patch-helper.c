#include <gtk/gtk.h>

#include "dyn-patch-helper.h"

static gboolean _dyn_patch_emit_changed(GtkMenuBar * menubar) {
	g_signal_emit_by_name(menubar, "changed", 0, NULL);
	g_message("Changed: %p", menubar);
	g_object_set_data(menubar, "__dirty__", NULL);
	return FALSE;
}
void dyn_patch_queue_changed(GtkMenuBar * menubar, GtkWidget * widget) {
	if(g_object_get_data(menubar, "__dirty__")) return;
	g_object_set_data(menubar, "__dirty__", GINT_TO_POINTER(1));
	g_idle_add_full(G_PRIORITY_HIGH_IDLE, _dyn_patch_emit_changed, g_object_ref(menubar), g_object_unref);
}

void dyn_patch_set_menubar(GtkWidget * widget, GtkMenuBar * menubar) {
	if(menubar != NULL) {
		g_object_set_data_full(widget, "__menubar__", g_object_ref(menubar), g_object_unref);
	} else {
		g_object_set_data(widget, "__menubar__", NULL);
	}
}
GtkMenuBar * dyn_patch_get_menubar(GtkWidget * widget) {
	return g_object_get_data(widget, "__menubar__");
}
static void _dyn_patch_label_changed(GtkWidget * widget, GParamSpec * pspec, GtkMenuBar * menubar) {
	dyn_patch_queue_changed(menubar, widget);
}
static void _dyn_patch_submenu_changed(GtkWidget * widget, GParamSpec * pspec, GtkMenuBar * menubar) {
	GtkWidget * old_submenu = g_object_get_data(widget, "__old_submenu__");
	GtkWidget * submenu = gtk_menu_item_get_submenu(widget);
	g_message("submenu changed %p %p", widget, submenu);
	if(submenu != old_submenu) {
		if(old_submenu) {
			dyn_patch_set_menubar_r(submenu, NULL);
		}
		if(submenu) {
			dyn_patch_set_menubar_r(submenu, menubar);
			g_object_set_data_full(widget, "__old_submenu__", g_object_ref(submenu), g_object_unref); 
		} else {
			g_object_set_data(widget, "__old_submenu__", NULL); 
		}
		/* although the property already hold a reference, 
		 * we want to ensure old_submenu above is still alive
		 * */
		dyn_patch_queue_changed(menubar, widget);
	}
}
void dyn_patch_set_menubar_r(GtkWidget * widget, GtkMenuBar * menubar) {
	GtkWidget * old = dyn_patch_get_menubar(widget);
	if(old && GTK_IS_LABEL(widget))
		g_signal_handlers_disconnect_by_func(widget, _dyn_patch_label_changed, old);
	if(old && GTK_IS_MENU_ITEM(widget))
		g_signal_handlers_disconnect_by_func(widget, _dyn_patch_submenu_changed, old);

	dyn_patch_set_menubar(widget, menubar);

	if(GTK_IS_CONTAINER(widget)) {
		GList * children = gtk_container_get_children(widget);
		GList * node;
		for(node = children; node; node = node->next) {
			dyn_patch_set_menubar_r(node->data, menubar);
		}
	}
	if(GTK_IS_MENU_ITEM(widget)) {
		GtkWidget * submenu = gtk_menu_item_get_submenu(widget);
		if(submenu) {
			g_object_set_data_full(submenu, "__item__", g_object_ref(widget), g_object_unref);
			dyn_patch_set_menubar_r(submenu, menubar);
		}
	}
	if(menubar && GTK_IS_LABEL(widget)) 
		g_signal_connect(widget, "notify::label", _dyn_patch_label_changed, menubar);
	if(menubar && GTK_IS_MENU_ITEM(widget)) {
		g_signal_connect(widget, "notify::submenu", _dyn_patch_submenu_changed, menubar);
	}
}

