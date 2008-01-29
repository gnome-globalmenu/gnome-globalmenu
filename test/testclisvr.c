#include <gtk/gtk.h>
#include <libgnomenu/serverhelper.h>
#include <libgnomenu/clienthelper.h>

typedef struct {
	GtkWindow * window;
	GnomenuServerHelper * server;
	GnomenuClientHelper * client;
	GtkButton * create, * destroy, * size, *connect;
} Application;

static void create_clicked_event_cb(GtkWidget * button, Application * App){
	App->server = gnomenu_server_helper_new();	
	App->client = gnomenu_client_helper_new();
}
static void destroy_clicked_event_cb(GtkWidget * button, Application * App){
	g_object_unref(App->server);
	g_object_unref(App->client);
}
static void connect_clicked_event_cb(GtkWidget * button, Application *App){
	/*TODO: finish gnomenu_client_helper and gnomenu_server_helper first,
 * 		perhaps need to use inheritance to make use of gdk_socket_get_native.
 * 		Should the native ID of a server helper identify with the wrapped socket's? */
	//gnomenu_client_helper_send_client_new(App->client,
	//	gnomenu_server_helper_get_native(App->server));
}
static void size_clicked_event_cb(GtkWidget * button, Application * App){
	
}

static void window_destroy_event_cb(GtkWidget * widget, GdkEvent * ev, Application * App){
	gtk_main_quit();
}
int main(int argc, char* argv[]){
	GtkBox * box;
	Application App;
	gtk_init(&argc, &argv);

	App.window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	App.create = GTK_BUTTON(gtk_button_new_with_label("create them"));
	App.destroy = GTK_BUTTON(gtk_button_new_with_label("destroy them"));
	App.size = GTK_BUTTON(gtk_button_new_with_label("test size chain"));
	App.connect = GTK_BUTTON(gtk_button_new_with_label("connect"));

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	
	g_signal_connect(G_OBJECT(App.window), "destroy",
			window_destroy_event_cb, NULL);

	g_signal_connect(G_OBJECT(App.create), "clicked",
			create_clicked_event_cb, &App);
	g_signal_connect(G_OBJECT(App.destroy), "clicked",
			destroy_clicked_event_cb, &App);
	g_signal_connect(G_OBJECT(App.size), "clicked",
			size_clicked_event_cb, &App);
	g_signal_connect(G_OBJECT(App.connect), "clicked",
			connect_clicked_event_cb, &App);

	gtk_box_pack_start_defaults(box, App.create);
	gtk_box_pack_start_defaults(box, App.size);
	gtk_box_pack_start_defaults(box, App.destroy);
	gtk_box_pack_start_defaults(box, App.connect);

	gtk_container_add(App.window, box);

	gtk_widget_show_all(GTK_WIDGET(App.window));
	gtk_main();
//	g_object_unref(server);
//	g_object_unref(socket);

	return 0;
}
