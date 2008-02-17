#include <gtk/gtk.h>
#include <libgnomenu/serverhelper.h>
#include <libgnomenu/gtkglobalmenubar.h>


GtkMenuBar * create_menu_bar(){
	GtkMenuBar * menu_bar;
	GtkMenuItem * new_item, * new_sub_item;
	GtkMenu * new_menu;
	struct s_sub_items {
		gchar * name;
		GFunc * func;
	};
	struct s_sub_items * s_p, file_items[] = {
		{"_New", NULL},
		{"_Open", NULL},
		{"_Close", NULL},
		{"_Save", NULL},
		{"Q_uit", NULL},
		{NULL, NULL}
	};
	struct s_items {
		gchar * name;
		struct s_sub_items * items;	
		GFunc * func;
	} * p, menus[] =  {
		{"_File", file_items, NULL},
		{NULL, NULL, NULL}
	};

	menu_bar = gtk_global_menu_bar_new();
	
	for(p = menus; p->name; p++){
		new_item = gtk_menu_item_new_with_mnemonic (p->name);
		if(p->func) 
			g_signal_connect(new_item, "activate",
			p->func, NULL);
		new_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(new_item, new_menu);
		for(s_p = p->items; s_p->name; s_p++){
			new_sub_item = gtk_menu_item_new_with_mnemonic(s_p->name);
			if(s_p->func) 
				g_signal_connect(new_sub_item, "activate",
				s_p->func, NULL);
			gtk_menu_append(new_menu, new_sub_item);
		}
		gtk_menu_shell_append(menu_bar, new_item);
	}

	return menu_bar;
}
static void window_destroy(GtkWidget * widget, GdkEvent * event, gpointer userdata){
	gtk_main_quit();
}
static GtkButton * create, * allocate, * show, * hide, * move;
GnomenuServerHelper * server;
GtkWidget * menu_window;	
GtkBox * box;

static void button_clicked(GtkWidget * button, gpointer ddddd){
	GList * node;
	if(button == create){
		gtk_box_pack_start_defaults(box, create_menu_bar());
		gtk_widget_show_all(box);		
		return;
	}
	for(node = g_list_first(server->clients);
		node;
		node = g_list_next(node)){
		if(button == allocate)
			gnomenu_server_helper_client_queue_resize(server, node->data);
		if(button == show)
			gnomenu_server_helper_client_set_visibility(server, node->data, TRUE);
		if(button == hide)
			gnomenu_server_helper_client_set_visibility(server, node->data, FALSE);
		if(button == move){
			GdkPoint pt = {10, 100};
			gnomenu_server_helper_client_set_position(server, node->data, &pt);
		}
	}	
}
static void server_size_request(GnomenuServerHelper * server, GnomenuClientInfo * ci, gpointer userdata){
	g_message("size_request!!");
}
static void server_client_realize(GnomenuServerHelper * server, GnomenuClientInfo * ci, gpointer userdata){
	GdkWindow * foreign = gdk_window_foreign_new(ci->ui_window);
	g_assert(foreign);
	gdk_window_reparent(foreign, GTK_WIDGET(menu_window)->window, 10, 10);
	//g_object_unref(foreign);
}
int main(int argc, char* argv[]){
	GtkWidget * window;

	gtk_init(&argc , &argv);
	menu_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
	gtk_widget_show_all(menu_window);

	server = gnomenu_server_helper_new();
	g_signal_connect(server, "size-request", server_size_request, NULL);
	g_signal_connect(server, "client-realize", server_client_realize, NULL);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(window_destroy), NULL);
	box = gtk_vbox_new(0, FALSE);
	
#define ADD_BUTTON(bn) \
	bn = gtk_button_new_with_label(#bn);\
	g_signal_connect(G_OBJECT(bn), "clicked", \
			G_CALLBACK(button_clicked), NULL);\
	gtk_box_pack_start_defaults(box, GTK_WIDGET(bn));

	gtk_container_add(window, box);

	ADD_BUTTON(create);
	ADD_BUTTON(allocate);
	ADD_BUTTON(show);
	ADD_BUTTON(hide);
	ADD_BUTTON(move);

	gtk_widget_show_all(window);
	gtk_main();
}
