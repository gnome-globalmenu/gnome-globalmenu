#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/socket.h>

	GnomenuSocket * server = NULL;
	GnomenuSocket * service = NULL;
	GnomenuSocket * socket1 = NULL;
	GnomenuSocket * socket2 = NULL;
	GtkWindow * window;
	GtkButton * create, * send, * send_by_name, * broadcast, * quit, * connect, * shutdown, * sudden;

static void socket_data_arrival_cb(GnomenuSocket * socket, 
	gpointer data, guint bytes, gpointer userdata){
	g_message("%s(%p)::data_arrival event, bytes = %d", socket->name, socket, bytes);
	g_message("Message is %*s", bytes, data);
}
static void socket_connect_req_cb(GnomenuSocket * socket,
	GnomenuSocketNativeID target){
	service = gnomenu_socket_accept(socket, target);
	g_signal_connect(G_OBJECT(service), "data-arrival",
			G_CALLBACK(socket_data_arrival_cb), NULL);
}
	
static void create_clicked_cb(GtkButton * button, gpointer user_data){
	g_message("button pressed");

}
static void button_clicked_cb(GtkButton * button, gpointer user_data){
	gchar MSG1[] = "SHELO";
	gchar MSG2[] = "CHELO";
	gchar buffer[12]="            ";
	if(button == send) {
		int i;
		for(i=0; i< 10; i++){
			g_sprintf(buffer, "%s%d", MSG1, i);
			gnomenu_socket_send(socket1, buffer, sizeof(buffer));
			g_sprintf(buffer, "%s%d", MSG2, i);
			if(service)
			gnomenu_socket_send(service, buffer, sizeof(buffer));
		}
	}
	if(button == create){
		server = gnomenu_socket_new("server");
		server->timeout = 1;
		gnomenu_socket_listen(server);

		socket1 = gnomenu_socket_new("test socket");
		socket2 = gnomenu_socket_new("test socket");
		socket1->timeout = 10;
		g_signal_connect(G_OBJECT(server), "data-arrival",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(socket1), "data-arrival::peer",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(socket2), "data-arrival",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(server), "connect-request",
				G_CALLBACK(socket_connect_req_cb), NULL);
	}
	if(button == broadcast){
		gnomenu_socket_broadcast_by_name(server, "test socket", MSG1, sizeof(MSG1));
	}
	if(button == connect){
		gnomenu_socket_connect(socket1, gnomenu_socket_get_native(server));
	}
	if(button == quit){
	gtk_widget_destroy(GTK_WIDGET(window));
	}
	if(button == shutdown){
		gnomenu_socket_shutdown(socket1);
	}
	if(button == sudden){
		socket1->status = GNOMENU_SOCKET_DISCONNECTED;
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

	ADD_BUTTON(create);
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
