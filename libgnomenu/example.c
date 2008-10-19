#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gnomenu.h>
#include <gmarkupdoc-parser.h>

static void realized(GtkWidget * window, GnomenuClient * client){
	GMarkupDocDocument * document = gmarkup_doc_view_get_document(client);
	gchar * xid = g_strdup_printf("%u", GDK_WINDOW_XID(window->window));
	gnomenu_client_register_window(client, "mywindow", xid);
	g_free(xid);
}
static void activated(GMarkupDocDocument * document, GMarkupDocNode * node, GQuark detail, gpointer data){
	gchar * name = gmarkup_doc_node_get_name(node);

	if(g_str_equal(name, "file")) {
		g_message("File is clicked do file stuff");
	}
}
int main(int argc, char* argv) {
	gtk_init(&argc, &argv);
	GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GMarkupDocDocument * document = gmarkup_doc_document_new();
	GMarkupDocParser * parser = gmarkup_doc_parser_new(document);
	GnomenuClient * client = gnomenu_client_new(document);
	GHashTable * dict;
	gmarkup_doc_parser_parse(parser, 
			"<window name=\"mywindow\">"
			"<menubar>"
			"<item name=\"file\" label=\"_File\"/>"
			"</menubar>"
			"</window>");
	g_signal_connect(window, "realize", realized, client);
	g_signal_connect(document, "activated", activated, NULL);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
