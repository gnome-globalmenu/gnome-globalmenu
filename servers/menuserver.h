#ifndef _MENU_SERVER_H_
#define _MENU_SERVER_H_
#include <libgnomenu/serverhelper.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE
G_BEGIN_DECLS

#define TYPE_MENU_SERVER	(menu_server_get_type())
#define MENU_SERVER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_MENU_SERVER, MenuServer))
#define MENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_MENU_SERVER, MenuServerClass))
#define IS_MENU_SERVER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_MENU_SERVER))
#define IS_MENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_MENU_SERVER))
#define MENU_SERVER_GET_CLASS(klass) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_MENU_SERVER, MenuServerClass))

typedef void KDEmenuServerHelper; /* TODO: write this helper*/
typedef struct _MenuClient MenuClient;
typedef struct {
	GObject parent;
	GnomenuServerHelper * gtk_helper;
	KDEmenuServerHelper * kde_helper;
	GHashTable * clients;
	GtkWidget * window;
	WnckScreen * screen;
	MenuClient * active;
	GdkColor * bgcolor;
	GdkPixmap * bgpixmap;
} MenuServer;
typedef struct {
	GObjectClass parent;
	void (*active_client_changed)(MenuServer * server);
} MenuServerClass;
GType menu_server_get_type (void);

MenuServer * menu_server_new(GtkWidget * window);
WnckWindow * menu_server_get_client_parent(MenuServer * server, MenuClient * client);
void menu_server_set_background(MenuServer * server, GdkColor * color, GdkPixmap * pixmap);
void menu_server_destroy(MenuServer * server);

G_END_DECLS
#endif
/*
vim:ts=4:sw=4
*/
