#include <gtk/gtk.h>

#include "dyn-patch.h"


DEFINE_FUNC(void, gtk_menu_shell, insert, (GtkMenuShell * shell, GtkWidget * child, int position)) {
	GtkMenuBar * menubar = dyn_patch_get_menubar(shell);
	VFUNC_TYPE(gtk_menu_shell, insert) vfunc = CHAINUP(gtk_menu_shell, insert);
	if(vfunc) {
		vfunc(shell, child, position);
	}
	if(menubar) {
		dyn_patch_set_menubar_r(child, menubar);
		dyn_patch_queue_changed(menubar, child);
	}
	g_debug("Insert %p to menu shell %p on menu bar %p", child, shell, menubar);
}
DEFINE_FUNC(void, gtk_menu_shell, remove, (GtkMenuShell * shell, GtkWidget * child)) {

	GtkMenuBar * menubar = dyn_patch_get_menubar(shell);
	dyn_patch_set_menubar_r(child, NULL);
	VFUNC_TYPE(gtk_menu_shell, remove) vfunc = CHAINUP(gtk_menu_shell, remove);
	if(vfunc) vfunc(shell, child);
	if(menubar) {
		dyn_patch_queue_changed(menubar, child);
	}
	g_debug("remove %p from menu shell %p on menu bar %p", child, shell, menubar);
}

void dyn_patch_menu_shell_patcher(GType menu_shell_type) {
	GObjectClass * klass =  dyn_patch_hold_type(menu_shell_type);
	GtkContainerClass * container_klass = (GtkContainerClass*)klass;
	GtkMenuShellClass * menu_shell_klass = (GtkMenuShellClass*)klass;

	if(menu_shell_type == GTK_TYPE_MENU_SHELL) {
		OVERRIDE_SAVE(menu_shell_klass, gtk_menu_shell, insert);
		OVERRIDE_SAVE(container_klass, gtk_menu_shell, remove);
	} else {
		OVERRIDE(menu_shell_klass, gtk_menu_shell, insert);
		OVERRIDE(container_klass, gtk_menu_shell, remove);
	}
}

void dyn_patch_menu_shell_unpatcher(GType menu_shell_type) {
	GObjectClass * klass =  g_type_class_ref(menu_shell_type);
	if(!klass) return;
	GtkContainerClass * container_klass = (GtkContainerClass*)klass;
	GtkMenuShellClass * menu_shell_klass = (GtkMenuShellClass*)klass;

	RESTORE(menu_shell_klass, gtk_menu_shell, insert);
	RESTORE(container_klass, gtk_menu_shell, remove);

	g_type_class_unref(klass);
	dyn_patch_release_type(menu_shell_type);
}

