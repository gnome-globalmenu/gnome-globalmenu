
struct _MenuClient {
	Window client_xid; /*for communication*/
	Window float_xid; /*Floating menu bar's xid*/
	Window master_xid;
};
struct _MenuClients{
	GList * list;
	struct _MenuServer * server;
};

typedef struct _MenuClients MenuClients;
typedef struct _MenuClient MenuClient;

MenuClients * menu_clients_new(struct _MenuServer * server) ;
MenuClient * menu_clients_add(MenuClients * clients, MenuClient * client_info);
MenuClient * menu_clients_find(MenuClients * clients, MenuClient * client_info);
void menu_clients_remove(MenuClients * clients, MenuClient * clientinfo);
void menu_clients_free(MenuClients * clients);
void menu_clients_foreach(MenuClients * clients, GFunc callback, gpointer userdata);
