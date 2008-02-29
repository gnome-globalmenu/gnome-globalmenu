#include <gtk/gtk.h>
#include <libgnomenu/serverhelper.h>
#include <libgnomenu/clienthelper.h>

enum {
	CREATE_SERVER,
	CREATE_CLIENT,
	DESTROY_SERVER,
	DESTROY_CLIENT,
	QUEUE_RESIZE,
	SET_ORIENT,
	REALIZE,
	UNREALIZE,
	REPARENT,
	BUTTONS_MAX
};
GtkWindow * window;
GnomenuServerHelper * server;
GnomenuClientHelper * client;
GtkButton * buttons[BUTTONS_MAX];

#if 0
static void create_clicked_event_cb(GtkWidget * button, gpointer * p){
	server = gnomenu_server_helper_new();	
	client = gnomenu_client_helper_new();
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
static void orient_clicked_event_cb(GtkWidget * button, Application * App){
	GnomenuClientInfo * ci;
	gnomenu_server_helper_client_set_orientation(App->server, NULL, 1);
	ci =  gnomenu_server_helper_find_client_by_socket_id(App->server,
			gdk_socket_get_native(App->client));
	gnomenu_server_helper_client_set_orientation(App->server, ci, 0);
}
#endif
static void window_destroy_event_cb(GtkWidget * widget, GdkEvent * ev, gpointer pp){
	gtk_main_quit();
}
static void server_client_new(GnomenuServerHelper * server, GnomenuClientInfo * ci, gpointer ud){
}
static void button_clicked(GtkWidget * button, gpointer data){
	int btn;
	for(btn =0; btn< BUTTONS_MAX; btn++){
		if(buttons[btn] == button) break;
	}
	switch(btn){
		case CREATE_SERVER:
			server = gnomenu_server_helper_new();
			g_signal_connect(G_OBJECT(server),
				"client-new", server_client_new, NULL);
		break;
		case CREATE_CLIENT:
			client = gnomenu_client_helper_new();
		break;
		case DESTROY_SERVER:
			g_object_unref(server);
		break;
		case DESTROY_CLIENT:
			g_object_unref(client);
		break;
		case QUEUE_RESIZE:{
			GList * node;
			for(node = g_list_first(server->clients);
				node;
				node = g_list_next(node)){
				gnomenu_server_helper_queue_resize(server, node->data);
			}
		}
		break;
		case SET_ORIENT:{
			GList * node;
			for(node = g_list_first(server->clients);
				node;
				node = g_list_next(node)){
				gnomenu_server_helper_set_orientation(server, node->data, 0);
			}
		}
		break;
		case REALIZE:
			gnomenu_client_helper_send_realize(client, GTK_WIDGET(window)->window);
		break;
		case UNREALIZE:
			gnomenu_client_helper_send_unrealize(client);
		break;
		case REPARENT:
			gnomenu_client_helper_send_reparent(client, GTK_WIDGET(window)->window);
		break;	
		default:
		g_error("un handled button");
	}
}
int main(int argc, char* argv[]){
	GtkBox * box;
	gtk_init(&argc, &argv);

	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	
#define ADD_BUTTON(bn) \
	buttons[bn] = GTK_BUTTON(gtk_button_new_with_label(#bn));\
	g_signal_connect(G_OBJECT(buttons[bn]), "clicked", \
			G_CALLBACK(button_clicked), client);\
	gtk_box_pack_start_defaults(box, GTK_WIDGET(buttons[bn]));

	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(window_destroy_event_cb), NULL);

	ADD_BUTTON(CREATE_SERVER);
	ADD_BUTTON(CREATE_CLIENT);
	ADD_BUTTON(DESTROY_SERVER);
	ADD_BUTTON(DESTROY_CLIENT);
	ADD_BUTTON(QUEUE_RESIZE);
	ADD_BUTTON(SET_ORIENT);
	ADD_BUTTON(REALIZE);
	ADD_BUTTON(UNREALIZE);
	ADD_BUTTON(REPARENT);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
//	g_object_unref(server);
//	g_object_unref(socket);

	return 0;
}
