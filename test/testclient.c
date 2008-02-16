#include <gtk/gtk.h>
#include <libgnomenu/gdksocket.h>
#include <libgnomenu/clienthelper.h>


GtkWidget * create, * destroy, * size;

static void create_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SERVER_NEW;
	gdk_socket_send_by_name(socket, GNOMENU_CLIENT_NAME, &msg, sizeof(msg));
}
static void destroy_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SERVER_DESTROY;
	gdk_socket_send_by_name(socket, GNOMENU_CLIENT_NAME, &msg, sizeof(msg));

}
static void size_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SIZE_QUERY;
	gdk_socket_send_by_name(socket, GNOMENU_CLIENT_NAME, &msg, sizeof(msg));
}
static void button_click(GtkButton * button, gpointer usrdata){

}
static void window_destroy_event_cb(GtkWidget * window, GdkEvent * ev, gpointer user_data){
	gtk_main_quit();
}
static void socket_data_arrival_cb(GdkSocket * socket, gpointer data, gint bytes, gpointer userdata){
	g_message("ding: %d", *(gint*)data);
}

int main(int argc, char* argv[]){
	GtkWindow * window;
	GnomenuClientHelper * client;
	GtkBox * box;
	GdkSocket * server;

	gtk_init(&argc, &argv);

	socket = gdk_socket_new("test socket");

	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	client = gnomenu_client_helper_new();
	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

#define ADD_BUTTON(bn) \
	bn = gtk_button_new_with_label(#bn);\
	g_signal_connect(G_OBJECT(bn), "clicked", \
			G_CALLBACK(button_clicked), client);\
	gtk_box_pack_start_defaults(box, GTK_WIDGET(bn));
	ADD_BUTTON(create)
	ADD_BUTTON(size)
	ADD_BUTTON(destroy);

	
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	g_signal_connect(G_OBJECT(socket), "data-arrival",
			G_CALLBACK(socket_data_arrival_cb), NULL);

	gtk_box_pack_start_defaults(box, create);
	gtk_box_pack_start_defaults(box, size);
	gtk_box_pack_start_defaults(box, destroy);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
//	g_object_unref(client);
//	g_object_unref(socket);

	return 0;
}
