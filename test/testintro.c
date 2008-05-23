#include <gtk/gtk.h>
#include <glade/glade.h>
#include <libgnomenu/introspector.h>



int main (int argc, char **argv){ 
	    GladeXML *xml;
	GtkWidget *widget;
	GtkMenuBar * menubar;
	gtk_init(&argc, &argv);
	xml = glade_xml_new("testintro.glade", NULL, NULL);

	/* get a widget (useful if you want to change something) */
	widget = glade_xml_get_widget(xml, "window1");
	menubar = glade_xml_get_widget(xml, "menubar1");
	/* connect signal handlers */
//	glade_xml_signal_autoconnect(xml);
	gtk_widget_show_all(widget);
	printf("%s", gtk_widget_introspect(menubar));

	return 0;
}
