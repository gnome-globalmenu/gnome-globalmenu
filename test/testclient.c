#include <gtk/gtk.h>
#include <libgnomenu/gdksocket.h>
#include <libgnomenu/clienthelper.h>


static void create_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SERVER_NEW;
	msg.server_new.socket_id = gdk_socket_get_native(socket);
	msg.server_new.container_window =0xbeafdead;
	gdk_socket_send_by_name(socket, GNOMENU_CLIENT_NAME, &msg, sizeof(msg));
}
static void destroy_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SERVER_DESTROY;
	msg.server_destroy.socket_id = gdk_socket_get_native(socket);
	gdk_socket_send_by_name(socket, GNOMENU_CLIENT_NAME, &msg, sizeof(msg));

}
static void size_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
/* TODO: simulate a size-request allocate-size period*/
/*	msg.any.type = GNOMENU_MSG_SIZE_REQUEST;
	msg.size_request.socket_id = gdk_socket_get_native(socket);
	msg.size_request.width = 123;
	msg.size_request.height = 45;
	gdk_socket_send_by_name(socket, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
*/
}
static void window_destroy_event_cb(GtkWidget * window, GdkEvent * ev, gpointer user_data){
	gtk_main_quit();
}

int main(int argc, char* argv[]){
	GtkWindow * window;
	GnomenuClientHelper * client;
	GtkButton * create, * destroy, * size;
	GdkSocket * socket;
	GtkBox * box;

	gtk_init(&argc, &argv);

	socket = gdk_socket_new("test socket");

	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	client = gnomenu_client_helper_new();
	create = GTK_BUTTON(gtk_button_new_with_label("create fake server"));
	destroy = GTK_BUTTON(gtk_button_new_with_label("destroy fake server"));
	size = GTK_BUTTON(gtk_button_new_with_label("size request"));


	box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	
	g_signal_connect(G_OBJECT(window), "destroy",
			window_destroy_event_cb, NULL);

	g_signal_connect(G_OBJECT(create), "clicked",
			create_clicked_event_cb, socket);
	g_signal_connect(G_OBJECT(destroy), "clicked",
			destroy_clicked_event_cb, socket);
	g_signal_connect(G_OBJECT(size), "clicked",
			size_clicked_event_cb, socket);

	gtk_box_pack_start_defaults(box, create);
	gtk_box_pack_start_defaults(box, size);
	gtk_box_pack_start_defaults(box, destroy);

	gtk_container_add(window, box);

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
//	g_object_unref(client);
//	g_object_unref(socket);

	return 0;
}
