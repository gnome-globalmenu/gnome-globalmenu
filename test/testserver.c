#include <gtk/gtk.h>
#include <libgnomenu/gdksocket.h>
#include <libgnomenu/gnomenuserver.h>

static void button_clicked_event_cb(GtkWidget * button, gpointer user_data){
	GdkSocket * socket;
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_ANY;
	socket = gdk_socket_new("test socket");
	gdk_socket_send_by_name(socket, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
	g_object_unref(socket);
}
static void window_destroy_event_cb(GtkWidget * window, GdkEvent * ev, gpointer user_data){
	gtk_main_quit();
}

int main(int argc, char* argv[]){
	GtkWindow * window;
	GnomenuServer * server;
	GtkButton * button;
	
	gtk_init(&argc, &argv);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	server = gnomenu_server_new();
	button = GTK_BUTTON(gtk_button_new_with_label("send message"));

	g_signal_connect(G_OBJECT(window), "destroy",
			window_destroy_event_cb, NULL);

	g_signal_connect(G_OBJECT(button), "clicked",
			button_clicked_event_cb, NULL);

	gtk_container_add(window, button);

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
	return 0;
}
