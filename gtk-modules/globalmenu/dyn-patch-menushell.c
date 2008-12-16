#include <gtk/gtk.h>

#include "dyn-patch.h"


DEFINE_FUNC(void, gtk_menu_shell, insert, (GtkMenuShell * shell, GtkWidget * child, int position)) {
	GtkMenuBar * menubar = dyn_patch_get_menubar(shell);
	_old_gtk_menu_shell_insert(shell, child, position);
	if(menubar) {
		dyn_patch_set_menubar_r(child, menubar);
		dyn_patch_queue_changed(menubar, child);
	}
}
DEFINE_FUNC(void, gtk_menu_shell, remove, (GtkMenuShell * shell, GtkWidget * child)) {

	GtkMenuBar * menubar = dyn_patch_get_menubar(shell);
	dyn_patch_set_menubar_r(child, NULL);
	_old_gtk_menu_shell_remove(shell, child);
	if(menubar) {
		dyn_patch_queue_changed(menubar, child);
	}
}

void dyn_patch_menu_shell_patcher(GType menu_shell_type) {
	GObjectClass * klass =  g_type_class_ref(menu_shell_type);
	GtkContainerClass * container_klass = (GtkContainerClass*)klass;
	GtkMenuShellClass * menu_shell_klass = (GtkMenuShellClass*)klass;

	OVERRIDE(menu_shell_klass, gtk_menu_shell, insert);
	OVERRIDE(container_klass, gtk_menu_shell, remove);
}

void dyn_patch_menu_shell_unpatcher(GType menu_shell_type) {
	GObjectClass * klass =  g_type_class_ref(menu_shell_type);
	GtkContainerClass * container_klass = (GtkContainerClass*)klass;
	GtkMenuShellClass * menu_shell_klass = (GtkMenuShellClass*)klass;

	RESTORE(menu_shell_klass, gtk_menu_shell, insert);
	RESTORE(container_klass, gtk_menu_shell, remove);
}

