#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/gdksocket.h>

	GdkSocket * server = NULL;
	GdkSocket * service = NULL;
	GdkSocket * socket1 = NULL;
	GdkSocket * socket2 = NULL;
	GtkWindow * window;
	GtkButton * create, * send, * send_by_name, * broadcast, * quit, * connect, * shutdown;

static void socket_data_arrival_cb(GdkSocket * socket, 
	gpointer data, guint bytes, gpointer userdata){
	g_message("%s(%p)::data_arrival event, bytes = %d", socket->name, socket, bytes);
	g_message("Message is %*s", bytes, data);
}
static void socket_connect_req_cb(GdkSocket * socket,
	GdkSocketNativeID target){
	service = gdk_socket_accept(socket, target);
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
			gdk_socket_send(socket1, buffer, sizeof(buffer));
			g_sprintf(buffer, "%s%d", MSG2, i);
			gdk_socket_send(service, buffer, sizeof(buffer));
		}
	}
	if(button == create){
		server = gdk_socket_new("server");
		gdk_socket_listen(server);

		socket1 = gdk_socket_new("test socket 1");
		socket2 = gdk_socket_new("test socket 2");

		g_signal_connect(G_OBJECT(server), "data-arrival",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(socket1), "data-arrival",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(socket2), "data-arrival",
				G_CALLBACK(socket_data_arrival_cb), NULL);
		g_signal_connect(G_OBJECT(server), "connect-request",
				G_CALLBACK(socket_connect_req_cb), NULL);
	}
	if(button == connect){
		gdk_socket_connect(socket1, gdk_socket_get_native(server));
	}
	if(button == quit){
	gtk_widget_destroy(GTK_WIDGET(window));

	}
	if(button == shutdown){
		gdk_socket_shutdown(socket1);
	}
}
static void broadcast_clicked_cb(GtkButton * button, gpointer user_data){
//	if(socket1)
		//gdk_socket_broadcast_by_name(socket1, "test socket 2", MSG, 4);
}
static void send_by_name_clicked_cb(GtkButton * button, gpointer user_data){
//	if(socket1)
//		gdk_socket_send_by_name(socket1, "test socket 2", MSG, 4);
}
static void window_destroy_event_cb(GtkWidget * widget, GdkEvent * event, gpointer userdata){
	gtk_main_quit();
}
int main(int argc, char* argv[]){
	GdkSocket * socket ;
	GtkBox * vbox;


	gtk_init(&argc, &argv);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));

	create = GTK_BUTTON(gtk_button_new_with_label("create"));
	g_signal_connect(G_OBJECT(create), "clicked", 
			G_CALLBACK(button_clicked_cb), NULL);

	connect = GTK_BUTTON(gtk_button_new_with_label("connect"));
	g_signal_connect(G_OBJECT(connect), "clicked",
			G_CALLBACK(button_clicked_cb), NULL);

	shutdown = GTK_BUTTON(gtk_button_new_with_label("shutdown"));
	g_signal_connect(G_OBJECT(shutdown), "clicked", 
			G_CALLBACK(button_clicked_cb), NULL);

	send = GTK_BUTTON(gtk_button_new_with_label("send"));
	g_signal_connect(G_OBJECT(send), "clicked", 
			G_CALLBACK(button_clicked_cb), NULL);

	send_by_name = GTK_BUTTON(gtk_button_new_with_label("send by name"));
	g_signal_connect(G_OBJECT(send_by_name), "clicked", 
			G_CALLBACK(button_clicked_cb), NULL);

	broadcast = GTK_BUTTON(gtk_button_new_with_label("broadcast"));
	g_signal_connect(G_OBJECT(broadcast), "clicked", 
			G_CALLBACK(button_clicked_cb), NULL);

	quit = GTK_BUTTON(gtk_button_new_with_label("quit"));
	g_signal_connect(G_OBJECT(quit), "clicked", 
			G_CALLBACK(button_clicked_cb), NULL);

	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(create));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(send));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(connect));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(shutdown));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(send_by_name));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(broadcast));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(quit));

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
	return 0;
}
