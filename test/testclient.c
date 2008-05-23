#include <gtk/gtk.h>
#include <libgnomenu/socket.h>
#include <libgnomenu/clienthelper.h>
#include <libgnomenu/messages.h>

GtkWidget * create, * destroy, * size, * bgcolor;
GnomenuClientHelper * client;
GnomenuSocket * server;
GnomenuSocket * service;

static void window_destroy_event_cb(GtkWidget * window, GdkEvent * ev, gpointer user_data){
	gtk_main_quit();
}
static void service_data_arrival(GnomenuSocket * socket, gpointer data, gint bytes, gpointer userdata){
	g_message("ding: %d", *(gint*)data);
}
static void server_connect_req(GnomenuSocket * socket, GnomenuSocketNativeID target, gpointer userdata){
	service = gnomenu_socket_new("service", 5);
	gnomenu_socket_accept(socket, service, target);
	g_signal_connect(G_OBJECT(service), "data", service_data_arrival, 0);	
}
static void button_clicked(GtkButton * button, gpointer usrdata){
	if(button == create){
		GnomenuMessage msg;
		server = gnomenu_socket_new(GNOMENU_SERVER_NAME, 10);
		g_signal_connect(G_OBJECT(server), "request",
				G_CALLBACK(server_connect_req), NULL);
		gnomenu_socket_listen(server);
		msg.any.type = GNOMENU_MSG_SERVER_NEW;
		msg.server_new.socket_id = gnomenu_socket_get_native(server);
		gnomenu_socket_broadcast(server, &msg, sizeof(msg));
	}
	if(button == size){
		GnomenuMessage msg;
		msg.any.type = GNOMENU_MSG_SIZE_QUERY;
		gnomenu_socket_send(service, &msg, sizeof(msg));
	}
	if(button == destroy){
		gnomenu_socket_shutdown(service);
	}
	if(button == bgcolor){
		GnomenuMessage msg;
		msg.any.type = GNOMENU_MSG_BGCOLOR_SET;
		msg.bgcolor_set.red = g_random_int();
		msg.bgcolor_set.blue = g_random_int();
		msg.bgcolor_set.green = g_random_int();
		
		gnomenu_socket_send(service, &msg, sizeof(msg));
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
