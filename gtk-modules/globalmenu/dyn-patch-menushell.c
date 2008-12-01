#include <gtk/gtk.h>

#include "dyn-patch-helper.h"

extern guint SIGNAL_CHANGED;
DEFINE_FUNC(void, gtk_menu_shell, insert, (GtkMenuShell * shell, GtkWidget * child, int position)) {
	GtkMenuBar * menubar = dyn_patch_get_menubar(shell);
	_old_gtk_menu_shell_insert(shell, child, position);
	if(menubar) {
		dyn_patch_set_menubar_r(child, menubar);
	}
}
DEFINE_FUNC(void, gtk_menu_shell, remove, (GtkMenuShell * shell, GtkWidget * child)) {
	dyn_patch_set_menubar_r(child, NULL);
	_old_gtk_menu_shell_remove(shell, child);
}

void dyn_patch_menu_shell() {
	return;
	GObjectClass * klass =  g_type_class_ref(GTK_TYPE_MENU_SHELL);
	GtkContainerClass * container_klass = (GtkContainerClass*)klass;
	GtkMenuShellClass * menu_shell_klass = (GtkMenuShellClass*)klass;

	OVERRIDE(menu_shell_klass, gtk_menu_shell, insert);
	OVERRIDE(container_klass, gtk_menu_shell, remove);
}

