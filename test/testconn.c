#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/connection.h>
GnomenuConnection * connection;
GtkButton * Connect, * QueryMethods, * QueryObjects, * Disconnect;
static void button_clicked_cb(GtkButton * button, gpointer user_data){
	if(button == Connect){
		if(!gnomenu_connection_connect(connection)){
			g_print("fail to connect");
		}
	}
	if(button == QueryMethods){
		g_print(gnomenu_connection_invoke(connection, NULL, "QueryMethods", NULL));
		GList * list = gnomenu_connection_query_methods(connection );
		GList * node;
		for(node = list; node; node=node->next){
			GnomenuConnectionMethodInfo * mi = node->data;
			g_print("%s( %s )\n", mi->name, mi->fmt);
		}
		g_list_free(list);
	}
	if(button == QueryObjects){
		g_print(gnomenu_connection_invoke(connection, NULL, "QueryObjects", NULL));
	}
	if(button == Disconnect){
		gnomenu_connection_disconnect(connection);
	}
}
int main(int argc, char * argv){
	GtkWindow * window;
	GtkBox * vbox;

	gtk_init(&argc, &argv);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));
	connection = gnomenu_connection_new("org/gnome/globalmenu/foo");
#define ADD_BUTTON(buttonname) \
	buttonname = GTK_BUTTON(gtk_button_new_with_label(#buttonname)); \
	g_signal_connect(G_OBJECT(buttonname), "clicked",  \
			G_CALLBACK(button_clicked_cb), NULL); \
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(buttonname));
	ADD_BUTTON(Connect);
	ADD_BUTTON(QueryMethods);
	ADD_BUTTON(QueryObjects);
	ADD_BUTTON(Disconnect);
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();

	return 0;
}
