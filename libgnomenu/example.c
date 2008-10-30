#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gnomenu.h>
#include <gmarkupdoc-parser.h>

static void realized(GtkWidget * window, GnomenuClient * client){
	GMarkupDocument * document = g_markup_view_get_document(client);
	gchar * xid = g_strdup_printf("%u", GDK_WINDOW_XID(window->window));
	gnomenu_client_register_window(client, "mywindow", xid);
	g_free(xid);
}
static void activated(GnomenuClient * client, GMarkupNode * window, GMarkupNode * node,  gpointer data){
	gchar * name = g_markup_node_get_name(node);
	gchar * windowname = g_markup_node_get_name(window);
	g_message("%s is clicked on window %s",name, windowname);
}
int main(int argc, char* argv) {
	gtk_init(&argc, &argv);
	GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GMarkupDocument * document = g_markup_document_new();
	GMarkupDocumentParser * parser = g_markup_document_parser_new(document);
	GnomenuClient * client = gnomenu_client_new(document);
	GHashTable * dict;
	g_markup_document_parser_parse(parser, 
			"<window name=\"mywindow\">"
			"<menubar>"
				"<item name=\"file\" label=\"_File\"/>"
				"<item name=\"edit\" label=\"_Edit\"/>"
			"</menubar>"
			"</window>");
	g_signal_connect(window, "realize", realized, client);
	g_signal_connect(client, "activated", activated, NULL);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
