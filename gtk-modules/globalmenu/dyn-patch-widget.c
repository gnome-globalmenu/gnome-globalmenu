#include <gtk/gtk.h>

#include "dyn-patch-vfunc.h"
#include "dyn-patch-utils.h"


DEFINE_FUNC(void, gtk_widget, parent_set, (GtkWidget * widget, GtkWidget * old_parent)) {
	GtkWidget * parent = widget->parent;
	GtkWidget * menubar = NULL;

	g_message("%s", gtk_widget_get_name(widget));
	VFUNC_TYPE(gtk_widget, parent_set) vfunc = CHAINUP(gtk_widget, parent_set);
	if(vfunc) vfunc(widget, old_parent);

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

void dyn_patch_widget_patcher(GType widget_type) {
	GObjectClass * klass =  dyn_patch_hold_type(widget_type);
	GtkWidgetClass * widget_klass = (GtkWidgetClass*)klass;

	if(widget_type == GTK_TYPE_WIDGET) {
		OVERRIDE_SAVE(widget_klass, gtk_widget, parent_set);
	} else {
		OVERRIDE(widget_klass, gtk_widget, parent_set);
	}
}

void dyn_patch_widget_unpatcher(GType widget_type) {
	GObjectClass * klass =  g_type_class_ref(widget_type);
	if(!klass) return;
	GtkWidgetClass * widget_klass = (GtkWidgetClass*)klass;

	RESTORE(widget_klass, gtk_widget, parent_set);

	g_type_class_unref(klass);
	dyn_patch_release_type(widget_type);
}
