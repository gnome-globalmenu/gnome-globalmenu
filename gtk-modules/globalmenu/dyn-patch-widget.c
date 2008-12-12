#include <gtk/gtk.h>

#include "dyn-patch-helper.h"

DEFINE_FUNC(void, gtk_widget, parent_set, (GtkWidget * widget, GtkWidget * old_parent)) {
	GtkWidget * parent = widget->parent;
	GtkWidget * menubar = NULL;
	if(GTK_IS_MENU_BAR(widget) || GTK_IS_MENU(widget)) {
		/* NOTE: for a gtk menu, the parent is a fake window.
		 * We handle it in dyn-patch-helper.c*/
		return;
	}
	if(parent != NULL) 
		menubar = dyn_patch_get_menubar(parent);

	GtkWidget * old = dyn_patch_get_menubar(widget);
	if(old != menubar) {
		dyn_patch_set_menubar_r(widget, menubar);
		if(old)
			dyn_patch_queue_changed(old, widget);
		if(menubar) {
			dyn_patch_queue_changed(menubar, widget);
		}
	}
}

void dyn_patch_widget() {
	GObjectClass * klass =  g_type_class_ref(GTK_TYPE_WIDGET);
	GtkWidgetClass * widget_klass = (GtkWidgetClass*)klass;
	OVERRIDE(widget_klass, gtk_widget, parent_set);
}
