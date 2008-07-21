#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <glade/glade.h>
#include <libgnomenu/globalmenu.h>
int main (int argc, char **argv){ 
	    GladeXML *xml;
	GtkBox * box;
	GtkWidget * window;
	GnomenuGlobalMenu * globalmenu;
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//gtk_window_set_accept_focus (window, FALSE);
	globalmenu = gnomenu_global_menu_new();
	globalmenu->auto_switch = TRUE;
	box = gtk_vbox_new(0, FALSE);
	gtk_container_add(window, box);
	gtk_container_add(box, globalmenu);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
