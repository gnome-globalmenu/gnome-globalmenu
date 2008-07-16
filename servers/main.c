#include <config.h>
#include <gtk/gtk.h>
#include <libgnomenu/globalmenu.h>
#include <glade/glade.h>

int main(int argc, char * argv[]){
	gtk_init(&argc, &argv);

	GladeXML * xml;
	GtkWidget * window;
	GnomenuGlobalMenu * globalmenu;
	gnomenu_global_menu_get_type();
	xml = glade_xml_new("GnomenuServerWindow.glade", NULL, NULL);
	window = glade_xml_get_widget(xml, "GnomenuServerWindow");
	globalmenu = glade_xml_get_widget(xml, "globalmenu");
	globalmenu->auto_switch = TRUE;
	glade_xml_signal_autoconnect(xml);
	gtk_widget_show(window);
	gtk_main();
	g_object_unref(xml);
	return 0;
}
/*
vim:ts=4:sw=4
*/
