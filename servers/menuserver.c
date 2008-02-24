#include <gtk/gtk.h>
#include "menuserver.h"

#include "log.h"

struct _MenuClient {
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
};

static MenuClient * _add_client		( MenuServer * _self);
static void _free_client			( MenuClient * client);
static void 
	_s_client_realize				( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_client_reparent				( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_client_unrealize				( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_screen_active_window_changed	( MenuServer * _self, 
									  WnckWindow * previous, 
									  WnckScreen * screen);
static void 
	_s_window_size_allocate			( MenuServer * _self, 
									  GtkAllocation * allocation, 
									  GtkWidget * widget);
static void 
	_s_gtk_helper_size_request		( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
			
static void 
	_update_active_menu_bar 		( MenuServer * _self);
static MenuClient * 
	_find_client_by_parent			( MenuServer * _self, GdkNativeWindow parent);

MenuServer * menu_server_new(GtkWidget * window){
	MenuServer * server = g_new0(MenuServer, 1);
	g_assert(!GTK_WIDGET_NO_WINDOW(window));
    
	server->gtk_helper = gnomenu_server_helper_new();
	server->kde_helper = NULL;
	server->clients = g_hash_table_new_full(NULL, NULL, NULL, _free_client);
	server->window = window;
	server->screen = wnck_screen_get_default();

	g_signal_connect_swapped(server->gtk_helper,
			"client-realize", _s_client_realize, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-reparent", _s_client_reparent, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-unrealize", _s_client_unrealize, server);

	g_signal_connect_swapped(server->screen,
			"active-window-changed", _s_screen_active_window_changed, server);
	g_signal_connect_swapped(server->window,
			"size-allocate", _s_window_size_allocate, server);
	g_signal_connect_swapped(server->gtk_helper,
			"size-request", _s_gtk_helper_size_request, server);
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
	c->window = gdk_window_foreign_new(ci->ui_window);
	c->parent = NULL;
	g_hash_table_insert(_self->clients, ci, c);
	gdk_window_reparent(c->window, (_self->window)->window, 0, 0);
}
static void _s_client_reparent(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_hash_table_lookup(_self->clients, ci);
	g_assert(c);
	c->parent = ci->parent_window;
	_update_active_menu_bar(_self);
}
static void _s_client_unrealize(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_hash_table_lookup(_self->clients, ci);
	g_assert(c);
	if( _self->active == c ){
		_self->active = NULL;
	}
	g_hash_table_remove(_self->clients, ci);
}
static MenuClient * _find_client_by_parent(MenuServer * _self, GdkNativeWindow parent){
	GList * node = NULL;
	GList * list = g_hash_table_get_values(_self->clients);
	MenuClient * rt = NULL;
	for(node = g_list_first(list); node; node = g_list_next(node)){
		if(((MenuClient *)node->data)->parent == parent) rt = node->data;
	}
	g_list_free(list);
	return rt;
}
static void _update_active_menu_bar (MenuServer * _self){
	GdkNativeWindow parent_transient ;
	GdkNativeWindow parent;
	WnckWindow * active = wnck_screen_get_active_window(_self->screen);
	MenuClient * c = NULL;
	if(active){
		WnckWindow * active_transient = wnck_window_get_transient(active);
 		parent = wnck_window_get_xid(active);
		if(active_transient)
			parent_transient = wnck_window_get_xid(active_transient);

		c =_find_client_by_parent(_self, parent);
		if(!c) 
			c = _find_client_by_parent(_self, parent_transient);
		LOG("find client at %p", c);
	} else {
		LOG("active is nil");
	}
	if(_self->active){
		switch(_self->active->type){
			case MENU_CLIENT_GTK:
				gnomenu_server_helper_set_visibility(_self->gtk_helper, _self->active->handle, FALSE);
			break;
			case MENU_CLIENT_KDE:
				/*try to hide it*/
			break;
		}
	}
	if(c){
		switch(c->type){
			case MENU_CLIENT_GTK:
				gdk_window_reparent(c->window, (_self->window)->window, 0, 0);
				gnomenu_server_helper_queue_resize(_self->gtk_helper, c->handle);
				gnomenu_server_helper_set_visibility(_self->gtk_helper, c->handle, TRUE);
			break;
			case MENU_CLIENT_KDE:
				gdk_window_reparent(c->window, (_self->window)->window, 0, 0);
			break;
		}
	}
	_self->active = c;
}
static void 
	_s_screen_active_window_changed	(MenuServer * _self, WnckWindow * previous, WnckScreen * screen){
	WnckWindow * active = wnck_screen_get_active_window(_self->screen);
	if(wnck_window_get_pid(active) == getpid()){
		return;
	}
	_update_active_menu_bar(_self);
}
static void _s_gtk_helper_size_request(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	ci->allocation.width = _self->window->allocation.width;
	ci->allocation.height = _self->window->allocation.height;
}
static void _s_window_size_allocate(MenuServer * _self, GtkAllocation * allocation, GtkWidget * widget){
	GtkAllocation a = * allocation;
	MenuClient * c = _self->active;
	a.x = 0;
	a.y = 0;
	if(c)
	switch(c->type){
		case MENU_CLIENT_GTK:
				gnomenu_server_helper_queue_resize(_self->gtk_helper, c->handle);
		break;
		case MENU_CLIENT_KDE:
		LOG("KDE unhandled");
		break;
	}
}
/*
 vim:ts=4:sw=4
*/
