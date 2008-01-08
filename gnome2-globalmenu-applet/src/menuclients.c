#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
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

#include "gtkmenuembed-x11.h"

#include "typedefs.h"
#include "menuserver.h"
#include "menuclients.h"

MenuClients * menu_clients_new(MenuServer * server) {
	MenuClients * clients;
	clients = g_new0(MenuClients, 1);
	clients->server = server;
	clients->list = NULL;
	return clients;
}
MenuClient * menu_clients_add(MenuClients * clients,
	MenuClient * client_info){
	MenuClient * client;

	client = g_new0(MenuClient, 1);
	client->client_xid = client_info->client_xid;
	client->float_xid = client_info->float_xid;
	client->master_xid = client_info->master_xid;
	clients->list = g_list_append(clients->list, client);
	return client;	
}
static void menu_clients_free_1(MenuClient * client){
		g_free(client);
}
MenuClient * menu_clients_find(MenuClients * clients, MenuClient * clientinfo){
	GList * node;
	MenuClient * client;
	for(node = g_list_first(clients->list);
		node;
		node = g_list_next(node)){
		client = (MenuClient *) node->data;
		if(client->client_xid == clientinfo->client_xid
		&& client->float_xid == clientinfo->float_xid 
		&& client->master_xid == clientinfo->master_xid){
			return client;
		}
	}
	return NULL;
}
void menu_clients_remove(MenuClients * clients,
	MenuClient * clientinfo){
	GList * node;
	MenuClient * client;
	for(node = g_list_first(clients->list);
		node;
		node = g_list_next(node)){
		client = (MenuClient *) node->data;
		if(client->client_xid == clientinfo->client_xid
		&& client->float_xid == clientinfo->float_xid 
		&& client->master_xid == clientinfo->master_xid){
			break;
		}
	}
	if(node){
		menu_clients_free_1(node->data);
		clients->list = g_list_remove_all(clients->list, node->data);
	}	
}

void menu_clients_free(MenuClients * clients){
	GList * node;
	for(node = g_list_first(clients->list);
		node;
		node = g_list_next(node)){
		menu_clients_free_1(node->data);
	}
	g_list_free(clients->list);
	clients->list = NULL;
	g_free(clients);
}
void menu_clients_foreach(MenuClients * clients, GFunc callback, gpointer userdata){
	GList * node;
	for(node = g_list_first(clients->list);
		node;
		node = g_list_next(node)){
		(*callback)(node->data, userdata);
	}
}
