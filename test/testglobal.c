#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <glade/glade.h>
#include <libgnomenu/builder.h>
#include <libgnomenu/tools.h>

GtkWidget * notebook;
void _s_activate(GtkMenuItem * menu_item, gpointer data){
	char * handle = g_object_get_data(menu_item, "introspect-handle");
	char * sms;
	g_message("menu item activated, handle = %s", handle);
	sms = g_strdup_printf("item%s", handle);
	gdkx_tools_send_sms(sms, strlen(sms)+1);
	g_free(sms);
}
void setup_handler(gchar * id, GtkWidget * widget, gpointer data){
	if(GTK_IS_MENU_ITEM(widget)){
		g_signal_connect(widget,
				"activate",
				_s_activate, data);
	}
}
void sms_filter(gpointer no_use, gchar * sms, gint size){
	Window xwindow;
	GdkWindow * window;
	gchar action[20];
	sscanf(sms, "%s %p", action, &xwindow);
	if(!g_str_equal(action, "menu")) return;
	window = gdk_window_lookup(xwindow);
	if(!window) window = gdk_window_foreign_new(xwindow);
	if(window){
		Builder * builder;
		GtkMenuBar * built_menubar;
		builder = builder_new();
		gchar * introspection = gdkx_tools_get_window_prop(window, gdk_atom_intern("GNOMENU_MENU_BAR", FALSE), NULL);
		g_print("%s", introspection);
		builder_parse(builder, introspection);
		built_menubar = builder_get_object(builder, sms);
		builder_foreach(builder, setup_handler, NULL);
		gtk_container_add(notebook, built_menubar);
		g_free(introspection);
		builder_destroy(builder);
	}
}
int main (int argc, char **argv){ 
	    GladeXML *xml;
	GtkBox * box;
	GtkWidget * window;
	gtk_init(&argc, &argv);

	//built_menubar = builder_get_object(builder, "globalmenubar");
	gdkx_tools_add_sms_filter(sms_filter, NULL);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	notebook = gtk_notebook_new();
	box = gtk_vbox_new(0, FALSE);
	gtk_container_add(window, box);
	gtk_container_add(box, notebook);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
