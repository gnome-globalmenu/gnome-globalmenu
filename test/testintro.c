#include <gtk/gtk.h>
#include <glade/glade.h>
#include <libgnomenu/introspector.h>
#include <libgnomenu/builder.h>



int main (int argc, char **argv){ 
	    GladeXML *xml;
	GtkWidget * window;
	GtkBox * box;
	GtkMenuBar * menubar;
	GtkMenuBar * menubar2;
	Builder * builder;

	gchar * intro;
	gchar * intro2;
	gtk_init(&argc, &argv);
	xml = glade_xml_new("testintro.glade", NULL, NULL);

	/* get a widget (useful if you want to change something) */
	menubar = glade_xml_get_widget(xml, "menubar1");
	/* connect signal handlers */
//	glade_xml_signal_autoconnect(xml);
	intro = gtk_widget_introspect(menubar);
	g_print("%s", intro);

	builder = builder_new();
	builder_parse(builder, intro);
	menubar2 = builder_get_object(builder, "menubar1");
	intro2 = gtk_widget_introspect(menubar2);
	g_print("%s", intro2);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	box = gtk_vbox_new(0, FALSE);
	gtk_container_add(window, box);
	gtk_container_add(box, menubar);
	gtk_container_add(box, menubar2);
	if(g_str_equal(intro2, intro)){
		g_print ("OK\n");
	} else {
	g_print ("Failed\n");
	}
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
