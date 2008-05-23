#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/socket.h>

	GnomenuSocket * server = NULL;
	GnomenuSocket * service = NULL;
	GnomenuSocket * socket1 = NULL;
	GnomenuSocket * socket2 = NULL;
	GtkWindow * window;
	GtkButton * create_server, * create_clients, * send, * send_by_name, * broadcast, * quit, * connect, * shutdown, * sudden;

static void socket_data_arrival_cb(GnomenuSocket * socket, 
	gpointer data, guint bytes, gpointer userdata){
	g_message("%s(%p)::data_arrival event, bytes = %d", socket->name, socket, bytes);
	g_message("Message is %*s", bytes, data);
}
static void service_shutdown(GnomenuSocket * socket,
	gpointer data){
	service = NULL;
}	
static void socket_connect_req_cb(GnomenuSocket * socket,
	GnomenuSocketNativeID target){
	service = gnomenu_socket_new("service", 1);

	g_signal_connect(G_OBJECT(service), "data",
			G_CALLBACK(socket_data_arrival_cb), NULL);
	g_signal_connect(G_OBJECT(service), "shutdown",
			G_CALLBACK(service_shutdown), NULL);

	gnomenu_socket_accept(socket, service, target);
}

static void button_clicked_cb(GtkButton * button, gpointer user_data){
	gchar * buffer[]={
			"A",
			"bb",
			"cccc",
			"dd",
			NULL
	};
	if(button == send) {
		char ** p;
		for(p = buffer; *p; p++){
			if(service)
				gnomenu_socket_send(service, *p, strlen(*p)+1);
			else 
				gnomenu_socket_send(socket1, *p, strlen(*p)+1);
		}
	}
	if(button == create_server){
		server = gnomenu_socket_new("server", 1);
		server->timeout = 1;
		gnomenu_socket_listen(server);
		g_signal_connect(G_OBJECT(server), "data",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(server), "request",
				G_CALLBACK(socket_connect_req_cb), NULL);
	}
	if(button == create_clients){
		socket1 = gnomenu_socket_new("test socket 1", 1);
		socket2 = gnomenu_socket_new("test socket 2", 1);
		socket1->timeout = 1;
		g_signal_connect(G_OBJECT(socket1), "data::peer",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(socket2), "data",
				G_CALLBACK(socket_data_arrival_cb), NULL);
	}
	if(button == broadcast){
		gnomenu_socket_broadcast_by_name(server, NULL, "broad", sizeof("broad"));
	}
	if(button == connect){
		GnomenuSocketNativeID server;
		server = gnomenu_socket_lookup(socket1, "server");
		gnomenu_socket_connect(socket1, server);
	}
	if(button == quit){
	gtk_widget_destroy(GTK_WIDGET(window));
	}
	if(button == shutdown){
		gnomenu_socket_shutdown(socket1);
		
	}
	if(button == sudden){
		socket1->status = GNOMENU_SOCKET_DISCONNECTED;
		g_object_unref(socket1);
	}
}
static void broadcast_clicked_cb(GtkButton * button, gpointer user_data){
//	if(socket1)
		//gnomenu_socket_broadcast_by_name(socket1, "test socket 2", MSG, 4);
}
static void send_by_name_clicked_cb(GtkButton * button, gpointer user_data){
//	if(socket1)
//		gnomenu_socket_send_by_name(socket1, "test socket 2", MSG, 4);
}
static void window_destroy_event_cb(GtkWidget * widget, GdkEvent * event, gpointer userdata){
	gtk_main_quit();
}
int main(int argc, char* argv[]){
	GnomenuSocket * socket ;
	GtkBox * vbox;


	gtk_init(&argc, &argv);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));
#define ADD_BUTTON(buttonname) \
	buttonname = GTK_BUTTON(gtk_button_new_with_label(#buttonname)); \
	g_signal_connect(G_OBJECT(buttonname), "clicked",  \
			G_CALLBACK(button_clicked_cb), NULL); \
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(buttonname));

	ADD_BUTTON(create_server);
	ADD_BUTTON(create_clients);
	ADD_BUTTON(broadcast);
	ADD_BUTTON(connect);
	ADD_BUTTON(send);
	ADD_BUTTON(shutdown);
	ADD_BUTTON(sudden);
	ADD_BUTTON(quit);


	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
	return 0;
}
