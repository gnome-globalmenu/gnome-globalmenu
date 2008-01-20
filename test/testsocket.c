#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/gdksocket.h>

	GdkSocket * socket1 = NULL;
	GdkSocket * socket2 = NULL;
	GtkWindow * window;
	gchar MSG[] = "HELO";

static void socket_data_arrival_cb(GdkSocket * socket, 
	gpointer data, guint bytes, gpointer userdata){
	g_message("%s::data_arrival event, bytes = %d", socket->name, bytes);
	g_message("Message is %*s", bytes, data);
}
static void create_clicked_cb(GtkButton * button, gpointer user_data){
	g_message("button pressed");
	socket1 = gdk_socket_new("test socket 1");
	socket2 = gdk_socket_new("test socket 2");
	g_signal_connect(G_OBJECT(socket1), "data-arrival",
			socket_data_arrival_cb, NULL);
	g_signal_connect(G_OBJECT(socket2), "data-arrival",
			socket_data_arrival_cb, NULL);
}
static void send_clicked_cb(GtkButton * button, gpointer user_data){
	gdk_socket_send(socket1, gdk_socket_get_native(socket2), MSG, 4);
}
static void quit_clicked_cb(GtkButton * button, gpointer user_data){
	gtk_widget_destroy(window);
}
static void window_destroy_event_cb(GtkWidget * widget, GdkEvent * event, gpointer userdata){
	gtk_main_quit();
}
int main(int argc, char* argv[]){
	GdkSocket * socket ;
	GtkButton * create, * send, * quit;
	GtkBox * vbox;

	gtk_init(&argc, &argv);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	g_signal_connect(G_OBJECT(window), "destroy",
			window_destroy_event_cb, NULL);

	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));

	create = GTK_BUTTON(gtk_button_new_with_label("create"));
	g_signal_connect(G_OBJECT(create), "clicked", 
			create_clicked_cb, NULL);

	send = GTK_BUTTON(gtk_button_new_with_label("send"));
	g_signal_connect(G_OBJECT(send), "clicked", 
			send_clicked_cb, NULL);

	quit = GTK_BUTTON(gtk_button_new_with_label("quit"));
	g_signal_connect(G_OBJECT(quit), "clicked", 
			quit_clicked_cb, NULL);

	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(create));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(send));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(quit));

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
	return 0;
}
