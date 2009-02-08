#include <gtk/gtk.h>
#include <libgnomenu/menuitem.h>

static void gmsg_foreach_cb(GtkWidget * child, gpointer data[]) {
	gint * pos = data[0];
	GtkMenuShell * menu_shell = data[1];
	if(*pos == 0) data[2] = child;
	(*pos) --;
}
void gtk_menu_shell_truncate(GtkMenuShell * menu_shell, gint length) {
	GList * children = gtk_container_get_children(menu_shell);
	GList * iter;
	gint l = g_list_length(children);
	if( l < length) {
		int i;
		for(i = l; i < length; i++) {
			gtk_menu_shell_append(menu_shell, gnomenu_menu_item_new());	
		}
		l = length;
		g_list_free(children);
		children = gtk_container_get_children(menu_shell);
	}
	for(iter = g_list_last(children); iter; iter = iter->prev) {
		if(l > length) {
			gnomenu_menu_item_set_truncated(iter->data, TRUE);
		} else {
			gnomenu_menu_item_set_truncated(iter->data, FALSE);
		}
		l--;
	}
	g_list_free(children);
}
GtkMenuItem * gtk_menu_shell_get_item(GtkMenuShell * menu_shell, gint position) {
	if(position >= gtk_menu_shell_length_without_truncated(menu_shell)) {
		gtk_menu_shell_truncate(menu_shell, position + 1);
	}
	gint length = gtk_menu_shell_length_without_truncated(menu_shell);
	if(position == -1) position = length - 1;

	gint pos = position;
	gpointer data[3] = { &pos, menu_shell, NULL};
	gtk_container_foreach(GTK_CONTAINER(menu_shell), 
			(GtkCallback)gmsg_foreach_cb, data);
	return (GtkMenuItem*) data[2];
}
static void gmsgip_foreach_cb(GtkWidget * child, gpointer data[]) {
	gint * i = data[0];
	gboolean * found = data[1];
	GtkWidget * foo = data[2];
	if(child == foo) {
		*found = TRUE;
	}
	if(!*found) (*i)++;
}
gint gtk_menu_shell_get_item_position(GtkMenuShell *menu_shell, GtkMenuItem * item) {
	gint i = 0;
	gboolean found = FALSE;
	gpointer data[] = {&i, &found, item};
	gtk_container_foreach((GtkContainer*)menu_shell, (GtkCallback)gmsgip_foreach_cb, data);
	if(found)
		return i;
	else
		return -1;
}
static void gmsl_foreach(GtkWidget * child, gpointer data[]) {
	gint * length = data[0];
	if(!gnomenu_menu_item_get_truncated((GnomenuMenuItem*)child)) (*length) ++;
}
gint gtk_menu_shell_length_without_truncated(GtkMenuShell * menu_shell) {
	gint length = 0;
	gpointer data[1] = {&length};
	gtk_container_foreach((GtkContainer*)menu_shell, (GtkCallback)gmsl_foreach, data);
	return length;
}
