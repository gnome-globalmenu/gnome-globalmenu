#ifndef GNOMENU_SERVER_H
#define GNOMENU_SERVER_H
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "gdksocket.h"
G_BEGIN_DECLS
/**
 * SECTION: gnomenuserver
 * @short_description: Gnome Global Menu Server.
 * @see_also: #GtkMenuBar, #GtkGlobalMenuBar, #GdkSocket.
 * @stablility: Unstable
 * @include: libgnome/gnomenuserver.h
 *
 * GnomeMenuServer is the virtual class for menu server. 
 * If you want to write a menu server, inherit from this class.
 */

#define GNOME_TYPE_MENU_SERVER	(gnome_menu_server_get_type())
#define GNOME_MENU_SERVER(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOME_TYPE_MENU_SERVER, GnomeMenuServer))
#define GNOME_MENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GNOME_TYPE_MENU_SERVER, GnomeMenuServerClass))
#define GNOME_IS_MENU_SERVER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_TYPE_MENU_SERVER))
#define GNOME_IS_MENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOME_TYPE_MENU_SERVER))
#define GNOME_MENU_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GNOME_TYPE_MENU_SERVER, GnomeMenuServerClass))

typedef struct _GnomeMenuServerClass GnomeMenuServerClass;
typedef struct _GnomeMenuServer GnomeMenuServer;

/**
 * GnomeMenuServer:
 *  @clients: A List of all the clients, each is GnomeMenuServerClient
 *
 *  GnomeMenuServer is the parent class for user defined menu server. 
 *  It provides basic operations and interface a menu server should have.
 */
struct _GnomeMenuServer{
	GObject * parent;
	GdkSocket * socket;
/*< public >*/
	GList * clients;
};

/*< private >*/
enum {
	GMS_SIGNAL_CLIENT_NEW,
	GMS_SIGNAL_CLIENT_DESTROY,
	GMS_SIGNAL_CLIENT_SIZE_REQUEST,
	GMS_SIGNAL_MAX
};
/**
 * GdkSocketClass:
 * @menu_create: the virtual function invoked.
 */
struct _GnomeMenuServerClass{
	GObjectClass parent;
/*< private >*/	
	guint signals[GMS_SIGNAL_MAX];
	void (*signal_client_new)(GnomeMenuServer * self, GdkNativeWindow client);
	void (*signal_client_destroy)(GnomeMenuServer * self, GdkNativeWindow client);
	void (*signal_client_size_request)(GnomeMenuServer * self, GdkNativeWindow client, GtkRequisition * req);
};
/**
 * GNOME_MENU_SERVER_NAME:
 *
 * Name of the socket
 */
#define GNOME_MENU_SERVER_NAME "GNOME MENU SERVER"

G_END_DECLS
#endif
