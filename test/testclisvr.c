#include <gtk/gtk.h>
#include <libgnomenu/serverhelper.h>
#include <libgnomenu/clienthelper.h>

typedef struct {
	GtkWindow * window;
	GnomenuServerHelper * server;
	GnomenuClientHelper * client;
	GtkButton * create, * destroy, * size;
} Application;

static void create_clicked_event_cb(GtkWidget * button, Application * App){
	App->server = gnomenu_server_helper_new();	
	App->client = gnomenu_client_helper_new();
}
static void destroy_clicked_event_cb(GtkWidget * button, Application * App){
	g_object_unref(App->client);
	g_object_unref(App->server);
}
static void size_clicked_event_cb(GtkWidget * button, Application * App){
	GnomenuClientInfo * ci;
	ci =  gnomenu_server_helper_find_client_by_socket_id(App->server,
			gdk_socket_get_native(App->client));
	
	gnomenu_server_helper_client_queue_resize(App->server, ci);
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

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	
	g_signal_connect(G_OBJECT(App.window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	g_signal_connect(G_OBJECT(App.create), "clicked",
			G_CALLBACK(create_clicked_event_cb), &App);
	g_signal_connect(G_OBJECT(App.destroy), "clicked",
			G_CALLBACK(destroy_clicked_event_cb), &App);
	g_signal_connect(G_OBJECT(App.size), "clicked",
			G_CALLBACK(size_clicked_event_cb), &App);

	gtk_box_pack_start_defaults(box, GTK_WIDGET(App.create));
	gtk_box_pack_start_defaults(box, GTK_WIDGET(App.size));
	gtk_box_pack_start_defaults(box, GTK_WIDGET(App.destroy));

	gtk_container_add(GTK_CONTAINER(App.window), GTK_WIDGET(box));

	gtk_widget_show_all(GTK_WIDGET(App.window));
	gtk_main();
//	g_object_unref(server);
//	g_object_unref(socket);

	return 0;
}
