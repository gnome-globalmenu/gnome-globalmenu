#ifndef _MENU_SERVER_H_
#define _MENU_SERVER_H_
#include <libgnomenu/serverhelper.h>

typedef void KDEmenuServerHelper; /* TODO: write this helper*/
typedef struct {
	GnomenuServerHelper * gtk_helper;
	KDEmenuServerHelper * kde_helper;
	GHashTable * clients;
	GdkWindow * window;
} MenuServer;
MenuServer * menu_server_new(GdkWindow * window);
void menu_server_destroy(MenuServer * server);
#endif
