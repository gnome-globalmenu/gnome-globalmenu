#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#include "typedefs.h"
#include "application.h"
#include "menuclients.h"
#include "menuserver.h"
#define INCLUDE_SOURCE
#include "gtkmenuembed-x11.h"
#undef INCLUDE_SOURCE
static void menu_server_client_new_cb(GlobalMenuSocket * socket, 
	GlobalMenuNotify * notify,
	MenuServer * server){
	MenuClient client;
	MenuClient * newclient;
	client.client_xid = notify->ClientNew.client_xid;
	client.float_xid = notify->ClientNew.float_xid;
	client.master_xid = notify->ClientNew.master_xid;

	g_message("ClientNew: client: %p, float: %p, master: %p",
			(gpointer) client.client_xid, 
			(gpointer) client.float_xid, 
			(gpointer) client.master_xid);
	newclient = menu_clients_add(server->clients, &client);
	if(server->client_new_cb)
		(*(server->client_new_cb))(server, newclient, server->userdata);
}

static void menu_server_client_destroy_cb(GlobalMenuSocket * socket,
	GlobalMenuNotify * notify,
	MenuServer * server){
	MenuClient client;
	MenuClient * oldclient;
	client.client_xid = notify->ClientDestroy.client_xid;
	client.float_xid = notify->ClientDestroy.float_xid;
	client.master_xid = notify->ClientDestroy.master_xid;

	g_message("ClientDestroy: client: %p, float: %p, master: %p",
			(gpointer) client.client_xid, 
			(gpointer) client.float_xid, 
			(gpointer) client.master_xid);

	oldclient = menu_clients_find(server->clients, &client);
	if(server->client_destroy_cb)
		(*(server->client_destroy_cb))(server, oldclient, server->userdata);
	menu_clients_remove(server->clients, &client);
}

MenuServer * menu_server_new(){
	MenuServer * server;
	server = g_new0(MenuServer, 1);
	server->clients = menu_clients_new(server);
	server->socket = global_menu_socket_new(MENU_SERVER_NAME, server);
	global_menu_socket_set_callback(server->socket, 
		GM_NOTIFY_NEW, 
		(GlobalMenuCallback) menu_server_client_new_cb);
	global_menu_socket_set_callback(server->socket, 
		GM_NOTIFY_DESTROY, 
		(GlobalMenuCallback) menu_server_client_destroy_cb);

	server->userdata = NULL;
	server->client_new_cb = NULL;
	server->client_destroy_cb = NULL;

	return server;
}
void menu_server_set_user_data(MenuServer * server, gpointer userdata){
	server->userdata = userdata;
}
void menu_server_set_callback(MenuServer * server, 
	MenuServerCallbackType type, 
	MenuServerCallback callback){
	switch(type){
		case MS_CB_CLIENT_NEW:
		server->client_new_cb = callback;
		break;
		case MS_CB_CLIENT_DESTROY:
		server->client_destroy_cb = callback;
		break;
	}
	
}
struct menu_server_broadcast_cb_struct{
	GlobalMenuNotify * message;
	MenuServer * server;
};
void menu_server_send_to(MenuServer * server, MenuClient * client, GlobalMenuNotify * message){
	global_menu_socket_send_to(server->socket, client->client_xid, message);
}
static void menu_server_broadcast_cb(MenuClient * client, struct menu_server_broadcast_cb_struct * data){
	g_message("menu server broadcasting to %p, with type: %s, data: %lu, %lu, %lu", 
		(gpointer) client->client_xid, global_menu_notify_get_name(data->message->type),
		data->message->param1,
		data->message->param2,
		data->message->param3
		);
	menu_server_send_to(data->server, client, data->message);
}
void menu_server_broadcast(MenuServer * server, GlobalMenuNotify * message){
	struct menu_server_broadcast_cb_struct data;
	data.message = message;
	data.server = server;
	menu_clients_foreach(server->clients, 
		(GFunc)menu_server_broadcast_cb, 
		&data);
}
void menu_server_start(MenuServer * server){
	GlobalMenuNotify message;
	message.type = GM_NOTIFY_SERVER_NEW;
	message.ServerNew.server_xid = global_menu_socket_get_xid(server->socket);
	global_menu_socket_broadcast_by_name(server->socket, MENU_CLIENT_NAME, &message);
}
void menu_server_shutdown(MenuServer * server){
	GlobalMenuNotify message;
	message.type = GM_NOTIFY_SERVER_DESTROY;
	message.ServerDestroy.server_xid = global_menu_socket_get_xid(server->socket);
	menu_server_broadcast(server, &message);
}
void menu_server_free(MenuServer * server){
	global_menu_socket_free(server->socket);
	g_free(server);
}
