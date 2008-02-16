#include <gtk/gtk.h>
#include <libgnomenu/gdksocket.h>
#include <libgnomenu/serverhelper.h>

GtkButton * create, * realize, * unrealize, * destroy, * size, * reparent, * quit;

static void size_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
}
static void window_destroy_event_cb(GtkWidget * window, GdkEvent * ev, gpointer user_data){
	gtk_main_quit();
}
static void socket_data_arrival_cb(GdkSocket * socket, gpointer data, gint bytes, gpointer userdata){
	g_message("\n\n\n\n\n ding");
}
static void button_clicked(GtkButton * button, GdkSocket * client){
	GnomenuMessage msg;
	if(button == create){
		gdk_socket_connect_by_name(client, GNOMENU_SERVER_NAME);
	}
	if(button == destroy){
		gdk_socket_shutdown(client);
	}
	if(button == realize){
		msg.any.type = GNOMENU_MSG_CLIENT_REALIZE;
		msg.client_realize.ui_window =0xdeadbeaf;
		gdk_socket_send(client, &msg, sizeof(msg));
	}
	if(button == reparent){
		msg.any.type = GNOMENU_MSG_CLIENT_REPARENT;
		msg.client_reparent.parent_window =0xbeefbeef;
		gdk_socket_send(client, &msg, sizeof(msg));
	}
	if(button == unrealize){
		msg.any.type = GNOMENU_MSG_CLIENT_UNREALIZE;
		gdk_socket_send(client, &msg, sizeof(msg));
	}
	if(button == size){
		msg.any.type = GNOMENU_MSG_SIZE_REQUEST;
		msg.size_request.width = 123;
		msg.size_request.height = 45;
		gdk_socket_send(client, &msg, sizeof(msg));
	}
}
int main(int argc, char* argv[]){
	GtkWindow * window;
	GnomenuServerHelper * server;
	GdkSocket * client;
	GtkBox * box;

	gtk_init(&argc, &argv);

	client = gdk_socket_new(GNOMENU_CLIENT_NAME);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	server = gnomenu_server_helper_new();

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

#define ADD_BUTTON(bn) \
	bn = gtk_button_new_with_label(#bn);\
	g_signal_connect(G_OBJECT(bn), "clicked", \
			G_CALLBACK(button_clicked), client);\
	gtk_box_pack_start_defaults(box, GTK_WIDGET(bn));
	
	ADD_BUTTON(create);
	ADD_BUTTON(realize);
	ADD_BUTTON(reparent);
	ADD_BUTTON(unrealize);
	ADD_BUTTON(destroy);
	ADD_BUTTON(size);
	ADD_BUTTON(quit);

	
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	g_signal_connect(G_OBJECT(client), "data-arrival",
			G_CALLBACK(socket_data_arrival_cb), NULL);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
//	g_object_unref(server);
//	g_object_unref(socket);

	return 0;
}
