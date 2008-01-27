#ifndef GNOMENU_SERVER_H
#define GNOMENU_SERVER_H
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "gdksocket.h"
#include "gnomenumessage.h"

G_BEGIN_DECLS
/**
 * SECTION: gnomenuserver
 * @short_description: Gnome Global Menu Server.
 * @see_also: #GtkMenuBar, #GtkGlobalMenuBar, #GdkSocket.
 * @stablility: Unstable
 * @include: libgnomenu/gnomenuserver.h
 *
 * GnomenuServer is the widget for a menu server.
 * 
 */

#define GNOMENU_TYPE_SERVER	(gnomenu_server_get_type())
#define GNOMENU_SERVER(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_SERVER, GnomenuServer))
#define GNOMENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_SERVER, GnomenuServerClass))
#define GNOME_IS_MENU_SERVER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_SERVER))
#define GNOME_IS_MENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_SERVER))
#define GNOMENU_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_SERVER, GnomenuServerClass))

typedef struct _GnomenuServerClass GnomenuServerClass;
typedef struct _GnomenuServer GnomenuServer;

/**
 * GnomenuServer:
 *  @clients: A List of all the clients, each is GnomenuServerClientInfo
 *
 *  GnomenuServer is the parent class for user defined menu server. 
 *  It provides basic operations and interface a menu server should have.
 */
struct _GnomenuServer{
	GtkWidget parent;
	GdkSocket * socket;
/*< public >*/
	GList * clients;
};

/**
 * GnomenuServerClientInfo:
 * @socket_id: the native id of the #GdkSocket the client is using. This value is
 * 			returned by #gdk_socket_get_native;
 * @ui_window: the container window which contains all the ui elements of the menu;
 * 			A menu server application can steal it and arrange its position.
 * @parent_window: the parent window of the menubar. The menu server application take
 * 		care of the changing of the focused window, and takes care of switching 
 * 		the active menu. 
 *
 * This structure is how GnomenuServer stores the client infomations;
 */
typedef struct _GnomenuServerClientInfo GnomenuServerClientInfo;

struct _GnomenuServerClientInfo {
	GdkNativeWindow socket_id;
	GdkNativeWindow ui_window;
	GdkNativeWindow parent_window;
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
struct _GnomenuServerClass{
	GtkWidgetClass parent;
/*< private >*/	
	guint signals[GMS_SIGNAL_MAX];
	void (*client_new)(GnomenuServer * self, GnomenuServerClientInfo * client_info);
	void (*client_destroy)(GnomenuServer * self, GnomenuServerClientInfo * client_info);
	void (*client_size_request)(GnomenuServer * self, GnomenuServerClientInfo * client_info, GtkRequisition * req);
};
/**
 * GNOMENU_SERVER_NAME:
 *
 * Name of the socket
 */
#define GNOMENU_SERVER_NAME "GNOME MENU SERVER"

G_END_DECLS
#endif
