#include <gtk/gtk.h>
#include <libgnomenu/gdksocket.h>
#include <libgnomenu/serverhelper.h>


static void create_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_NEW;
	msg.client_new.socket_id = gdk_socket_get_native(socket);
	gdk_socket_send_by_name(socket, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
}
static void realize_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_REALIZE;
	msg.client_realize.socket_id = gdk_socket_get_native(socket);
	msg.client_realize.ui_window =0xdeadbeaf;
	msg.client_realize.parent_window =0xbeafdead;
	gdk_socket_send_by_name(socket, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
}
static void unrealize_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_UNREALIZE;
	msg.client_unrealize.socket_id = gdk_socket_get_native(socket);
	gdk_socket_send_by_name(socket, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
}
static void destroy_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_DESTROY;
	msg.client_destroy.socket_id = gdk_socket_get_native(socket);
	gdk_socket_send_by_name(socket, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
}
static void size_clicked_event_cb(GtkWidget * button, GdkSocket * socket){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SIZE_REQUEST;
	msg.size_request.socket_id = gdk_socket_get_native(socket);
	msg.size_request.width = 123;
	msg.size_request.height = 45;
	gdk_socket_send_by_name(socket, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
}
static void window_destroy_event_cb(GtkWidget * window, GdkEvent * ev, gpointer user_data){
	gtk_main_quit();
}
static void socket_data_arrival_cb(GdkSocket * socket, gpointer data, gint bytes, gpointer userdata){
	g_message("\n\n\n\n\n ding");
}
int main(int argc, char* argv[]){
	GtkWindow * window;
	GnomenuServerHelper * server;
	GtkButton * create, * realize, * unrealize, * destroy, * size;
	GdkSocket * socket;
	GtkBox * box;

	gtk_init(&argc, &argv);

	socket = gdk_socket_new("test socket");

	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	server = gnomenu_server_helper_new();
	create = GTK_BUTTON(gtk_button_new_with_label("create fake client"));
	realize = GTK_BUTTON(gtk_button_new_with_label("realize fake client"));
	unrealize = GTK_BUTTON(gtk_button_new_with_label("unrealize fake client"));
	destroy = GTK_BUTTON(gtk_button_new_with_label("destroy fake client"));
	size = GTK_BUTTON(gtk_button_new_with_label("size request"));


	box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	g_signal_connect(G_OBJECT(create), "clicked",
			G_CALLBACK(create_clicked_event_cb), socket);
	g_signal_connect(G_OBJECT(realize), "clicked",
			G_CALLBACK(realize_clicked_event_cb), socket);
	g_signal_connect(G_OBJECT(unrealize), "clicked",
			G_CALLBACK(unrealize_clicked_event_cb), socket);
	g_signal_connect(G_OBJECT(destroy), "clicked",
			G_CALLBACK(destroy_clicked_event_cb), socket);
	g_signal_connect(G_OBJECT(size), "clicked",
			G_CALLBACK(size_clicked_event_cb), socket);
	g_signal_connect(G_OBJECT(socket), "data-arrival",
			G_CALLBACK(socket_data_arrival_cb), NULL);

	gtk_box_pack_start_defaults(box, GTK_WIDGET(create));
	gtk_box_pack_start_defaults(box, GTK_WIDGET(realize));
	gtk_box_pack_start_defaults(box, GTK_WIDGET(unrealize));
	gtk_box_pack_start_defaults(box, GTK_WIDGET(size));
	gtk_box_pack_start_defaults(box, GTK_WIDGET(destroy));

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
//	g_object_unref(server);
//	g_object_unref(socket);

	return 0;
}
