#include <gtk/gtk.h>
#include <libgnomenu/gdksocket.h>
#include <libgnomenu/clienthelper.h>


GtkWidget * create, * destroy, * size, * bgcolor;
GnomenuClientHelper * client;
GdkSocket * server;
GdkSocket * service;

static void window_destroy_event_cb(GtkWidget * window, GdkEvent * ev, gpointer user_data){
	gtk_main_quit();
}
static void service_data_arrival(GdkSocket * socket, gpointer data, gint bytes, gpointer userdata){
	g_message("ding: %d", *(gint*)data);
}
static void server_connect_req(GdkSocket * socket, GdkSocketNativeID target, gpointer userdata){
	service = gdk_socket_accept(socket, target);
	g_signal_connect(G_OBJECT(service), "data-arrival", service_data_arrival, 0);	
}
static void button_clicked(GtkButton * button, gpointer usrdata){
	if(button == create){
		GnomenuMessage msg;
		server = gdk_socket_new(GNOMENU_SERVER_NAME);
		g_signal_connect(G_OBJECT(server), "connect-request",
				G_CALLBACK(server_connect_req), NULL);
		gdk_socket_listen(server);
		msg.any.type = GNOMENU_MSG_SERVER_NEW;
		msg.server_new.socket_id = gdk_socket_get_native(server);
		gdk_socket_broadcast_by_name(server, GNOMENU_CLIENT_NAME, &msg, sizeof(msg));
	}
	if(button == size){
		GnomenuMessage msg;
		msg.any.type = GNOMENU_MSG_SIZE_QUERY;
		gdk_socket_send(service, &msg, sizeof(msg));
	}
	if(button == destroy){
		gdk_socket_shutdown(service);
	}
	if(button == bgcolor){
		GnomenuMessage msg;
		msg.any.type = GNOMENU_MSG_BGCOLOR_SET;
		msg.bgcolor_set.red = g_random_int();
		msg.bgcolor_set.blue = g_random_int();
		msg.bgcolor_set.green = g_random_int();
		
		gdk_socket_send(service, &msg, sizeof(msg));
	}
}
int main(int argc, char* argv[]){
	GtkWindow * window;
	GtkBox * box;

	gtk_init(&argc, &argv);

	client = gnomenu_client_helper_new();
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

#define ADD_BUTTON(bn) \
	bn = gtk_button_new_with_label(#bn);\
	g_signal_connect(G_OBJECT(bn), "clicked", \
			G_CALLBACK(button_clicked), client);\
	gtk_box_pack_start_defaults(box, GTK_WIDGET(bn));
	ADD_BUTTON(create)
	ADD_BUTTON(bgcolor);
	ADD_BUTTON(size)
	ADD_BUTTON(destroy);
	
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
//	g_object_unref(client);
//	g_object_unref(socket);

	return 0;
}
