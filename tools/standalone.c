#include <gtk/gtk.h>
#include <globalmenu-server.h>


int
main (int argc, char **argv)
{
    GtkWidget *window;
    GnomenuGlobalMenuBar *menubar;

    gtk_init (&argc, &argv);

    menubar = gnomenu_global_menu_bar_new();
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_MENU);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_accept_focus(GTK_WINDOW(window), FALSE);
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(menubar));
    gtk_widget_show_all(window);

    gtk_main ();
    return 0;
}
