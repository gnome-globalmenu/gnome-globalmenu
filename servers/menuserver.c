#include <gtk/gtk.h>
#include "menuserver.h"
#include "log.h"

typedef struct {
	enum {
		MENU_CLIENT_GTK,
		MENU_CLIENT_KDE,
	} type;
	gpointer handle; /*handle for helper to locate this client, is also the key*/
	union{
		GdkNativeWindow transient;
		GdkNativeWindow parent;  /* for GTK I use parent, for KDE I use transient*/
	};
	GdkWindow * window; /*the window for the menu bar*/
} MenuClient;

static MenuClient * _add_client		(MenuServer * _self);
static void _free_client			(MenuClient * client);
static void _s_client_realize		(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper);
static void _s_client_reparent(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper);
static void _s_client_unrealize		(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper);
	

MenuServer * menu_server_new(GdkWindow * window){
	MenuServer * server = g_new0(MenuServer, 1);
	server->gtk_helper = gnomenu_server_helper_new();
	server->kde_helper = NULL;
	server->clients = g_hash_table_new_full(NULL, NULL, NULL, _free_client);
	server->window = window;

	g_signal_connect_swapped(server->gtk_helper,
			"client-realize", _s_client_realize, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-reparent", _s_client_reparent, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-unrealize", _s_client_unrealize, server);
	return server;
}
void menu_server_destroy(MenuServer * server){
	g_hash_table_destroy(server->clients);
	g_object_unref(server->gtk_helper);
	g_free(server);
}
static void _free_client(MenuClient * client){
	g_free(client);
}
static void _s_client_realize(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_new0(MenuClient, 1);
	c->type = MENU_CLIENT_GTK;
	c->handle = ci;
	//c->window = gdk_window_foreign_new(ci->ui_window);
	c->parent = NULL;
	g_hash_table_insert(_self->clients, ci, c);
}
static void _s_client_reparent(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_hash_table_lookup(_self->clients, ci);
	g_assert(c);
	c->parent = ci->parent_window;
}
static void _s_client_unrealize(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	g_hash_table_remove(_self->clients, ci);
}
