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

	menu_bar = GTK_MENU_BAR(gtk_global_menu_bar_new());
	
	for(p = menus; p->name; p++){
		new_item = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic (p->name));
		if(p->func) 
			g_signal_connect(G_OBJECT(new_item), "activate",
			G_CALLBACK(p->func), NULL);
		new_menu = GTK_MENU(gtk_menu_new());
		gtk_menu_item_set_submenu(new_item, GTK_WIDGET(new_menu));
		for(s_p = p->items; s_p->name; s_p++){
			new_sub_item = GTK_MENU_ITEM(gtk_menu_item_new_with_mnemonic(s_p->name));
			if(s_p->func) 
				g_signal_connect(G_OBJECT(new_sub_item), "activate",
				G_CALLBACK(s_p->func), NULL);
			gtk_menu_append(new_menu, GTK_WIDGET(new_sub_item));
		}
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), GTK_WIDGET(new_item));
	}

	return menu_bar;
}
static void window_destroy(GtkWidget * widget, GdkEvent * event, gpointer userdata){
	gtk_main_quit();
}
enum {
	CREATE_MENU,
	CREATE_SERVER,
	ALLOCATE,
	SHOW,
	HIDE,
	MOVE,
	DESTROY_SERVER,
	DESTROY_MENU,
	BTN_MAX
};
static GtkButton * buttons[BTN_MAX];

GnomenuServerHelper * server;
GtkWidget * menuwindow;	
GtkMenuBar * menubar;
GtkBox * box;

static void server_size_request(GnomenuServerHelper * server, GnomenuClientInfo * ci, gpointer userdata){
	g_message("size_request!!");
}
static void server_client_realize(GnomenuServerHelper * server, GnomenuClientInfo * ci, gpointer userdata){
	GdkWindow * foreign = gdk_window_foreign_new(ci->ui_window);
	g_assert(foreign);
	gdk_window_reparent(foreign, GTK_WIDGET(menuwindow)->window, 10, 10);
	g_object_unref(foreign);
	gnomenu_server_helper_client_queue_resize(server, ci);
}
static void menuwindow_destroy(GtkWidget * widget, GdkEvent * event, gpointer userdata){
	g_object_unref(server);
}
static void button_clicked(GtkButton * button, gpointer ddddd){
	GList * node;
	int btn;
	for(btn =0; btn< BTN_MAX; btn++){
		if(buttons[btn] == button) break;
	}
	switch(btn){
		case CREATE_MENU:
			menubar = create_menu_bar();
			gtk_box_pack_start_defaults(box, GTK_WIDGET(menubar));
			gtk_widget_show_all(GTK_WIDGET(box));		
		break;
		case CREATE_SERVER:
			server = gnomenu_server_helper_new();
			g_signal_connect(G_OBJECT(server), "size-request", G_CALLBACK(server_size_request), NULL);
			g_signal_connect(G_OBJECT(server), "client-realize", G_CALLBACK(server_client_realize), NULL);
			menuwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
			g_signal_connect(G_OBJECT(menuwindow), "destroy", G_CALLBACK(menuwindow_destroy), NULL);
			gtk_widget_show_all(GTK_WIDGET(menuwindow));
		break;
		case DESTROY_SERVER:
			gtk_widget_destroy(GTK_WIDGET(menuwindow));
		break;
		case DESTROY_MENU:
			gtk_widget_destroy(GTK_WIDGET(menubar));
		break;
		case ALLOCATE:
		case SHOW:
		case HIDE:
		case MOVE:
		for(node = g_list_first(server->clients);
			node;
			node = g_list_next(node)){
			if(btn == ALLOCATE)
				gnomenu_server_helper_client_queue_resize(server, node->data);
			if(btn == SHOW)
				gnomenu_server_helper_client_set_visibility(server, node->data, TRUE);
			if(btn == HIDE)
				gnomenu_server_helper_client_set_visibility(server, node->data, FALSE);
			if(btn == MOVE){
				GdkPoint pt = {10, 100};
				gnomenu_server_helper_client_set_position(server, node->data, &pt);
			}
		}	
		break;	
	}
}
int main(int argc, char* argv[]){
	GtkWidget * window;

	gtk_init(&argc , &argv);


	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(window_destroy), NULL);
	box = GTK_BOX(gtk_vbox_new(0, FALSE));
	
#define ADD_BUTTON(bn) \
	buttons[bn] = gtk_button_new_with_label(#bn);\
	g_signal_connect(G_OBJECT(buttons[bn]), "clicked", \
			G_CALLBACK(button_clicked), NULL);\
	gtk_box_pack_start_defaults(box, GTK_WIDGET(buttons[bn]));

	gtk_container_add(window, box);

	ADD_BUTTON(CREATE_SERVER);
	ADD_BUTTON(CREATE_MENU);
	ADD_BUTTON(ALLOCATE);
	ADD_BUTTON(SHOW);
	ADD_BUTTON(HIDE);
	ADD_BUTTON(MOVE);
	ADD_BUTTON(DESTROY_SERVER);
	ADD_BUTTON(DESTROY_MENU);

	gtk_widget_show_all(window);
	gtk_main();
}
