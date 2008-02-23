#ifndef _MENU_SERVER_H_
#define _MENU_SERVER_H_
#include <libgnomenu/serverhelper.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

typedef void KDEmenuServerHelper; /* TODO: write this helper*/
typedef struct _MenuClient MenuClient;
typedef struct {
	GnomenuServerHelper * gtk_helper;
	KDEmenuServerHelper * kde_helper;
	GHashTable * clients;
	GtkWidget * window;
	WnckScreen * screen;
	MenuClient * active;
} MenuServer;
MenuServer * menu_server_new(GtkWidget * window);
void menu_server_destroy(MenuServer * server);
#endif
