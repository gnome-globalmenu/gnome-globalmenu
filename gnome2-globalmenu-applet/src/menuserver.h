typedef void (*MenuServerCallback)(struct _MenuServer * server, struct _MenuClient * client, gpointer data);
typedef enum {
	MS_CB_CLIENT_NEW,
	MS_CB_CLIENT_DESTROY,
} MenuServerCallbackType;
struct _MenuServer{
	struct _GlobalMenuSocket * socket;
	struct _MenuClients * clients;
	gpointer userdata;
	struct {
		MenuServerCallback client_new_cb;
		MenuServerCallback client_destroy_cb;
	};
};
typedef struct _MenuServer MenuServer;
MenuServer * menu_server_new();
void menu_server_set_user_data(MenuServer * server, gpointer userdata);
void menu_server_destroy(MenuServer * server);
void menu_server_discover_clients(MenuServer * server);
void menu_server_send_to(MenuServer * server, struct _MenuClient * client, struct _GlobalMenuNotify * message);
void menu_server_broadcast(MenuServer * server, struct _GlobalMenuNotify * message);
void menu_server_start(MenuServer * server);
void menu_server_shutdown(MenuServer * server);
void menu_server_set_callback(MenuServer * server, MenuServerCallbackType type, MenuServerCallback callback);
void menu_server_free(MenuServer * server);
