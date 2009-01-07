#include <gtk/gtk.h>
#include <libgnomenu/menuitem.h>

static void gmsg_foreach_cb(GtkWidget * child, gpointer data[]) {
	gint * pos = data[0];
	GtkMenuShell * menu_shell = data[1];
	if(*pos == 0) data[2] = child;
	(*pos) --;
}
GtkMenuItem * gtk_menu_shell_get_item(GtkMenuShell * menu_shell, gint position) {
	gint pos = position;
	gpointer data[3] = { &pos, menu_shell, NULL};
	gtk_container_foreach(menu_shell, gmsg_foreach_cb, data);
	return (GtkMenuItem*) data[2];
}
gboolean gtk_menu_shell_has_item(GtkMenuShell * menu_shell, gint position) {
	return gtk_menu_shell_get_item(menu_shell, position) != NULL;
}
void gtk_menu_shell_truncate(GtkMenuShell * menu_shell, gint length) {
	GList * children = gtk_container_get_children(menu_shell);
	GList * iter;
	gint l = g_list_length(children);
	for(iter = g_list_last(children); iter; iter = iter->prev) {
		if(l > length) {
			//gtk_container_remove(menu_shell, iter->data);	
//			gtk_widget_hide(GTK_WIDGET(iter->data));
			gnomenu_menu_item_set_truncated(iter->data, TRUE);
		}
		l--;
	}
	g_list_free(children);
}
static void gmsl_foreach(GtkWidget * child, gpointer data[]) {
	gint * length = data[0];
	if(!gnomenu_menu_item_get_truncated(child)) (*length) ++;
}
gint gtk_menu_shell_length(GtkMenuShell * menu_shell) {
	gint length = 0;
	gpointer data[1] = {&length};
	gtk_container_foreach(menu_shell, gmsl_foreach, data);
	return length;
}
