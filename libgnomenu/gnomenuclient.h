#ifndef GNOMENU_CLIENT_H
#define GNOMENU_CLIENT_H
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "gdksocket.h"
#include "gnomenumessage.h"

G_BEGIN_DECLS
/**
 * SECTION: GnomenuClient
 * @short_description: Menu client class
 * @see_also: #GtkMenuBar, #GtkGlobalMenuBar, #GdkSocket, #GnomenuClient
 * @stablility: Unstable
 * @include: libgnomenu/gnomenuclient.h
 *
 * GnomenuClient provides fundanmental messaging mechanism for a menu client
 * 
 */

#define GNOMENU_TYPE_CLIENT	(gnomenu_server_get_type())
#define GNOMENU_CLIENT(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_CLIENT, GnomenuClient))
#define GNOMENU_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_CLIENT, GnomenuClientClass))
#define GNOME_IS_MENU_CLIENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_CLIENT))
#define GNOME_IS_MENU_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_CLIENT))
#define GNOMENU_CLIENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_CLIENT, GnomenuClientClass))

typedef struct _GnomenuClientClass GnomenuClientClass;
typedef struct _GnomenuClient GnomenuClient;

/**
 * GnomenuClientServerInfo:
 * @socket_id: the native id of the socket which the server is using.
 *
 * GnomenuClientServerInfo is used by GnomenuClient to store server info.
 */
typedef struct _GnomenuClientServerInfo {
	GdkNativeWindow socket_id;
} GnomenuClientServerInfo;
/**
 * GnomenuClient:
 *  @clients: A List of all the clients, each is GnomenuClientClientInfo
 *
 * GnomenuClient provides fundanmental messaging mechanism for a menu server
 */
struct _GnomenuClient{
	GObject parent;
	GdkSocket * socket;
	GnomenuClientServerInfo * server_info;
/*< public >*/
};


/*< private >*/
enum {
	GMC_SIGNAL_SERVER_NEW,
	GMC_SIGNAL_SERVER_DESTROY,
	GMC_SIGNAL_ALLOCATE_SIZE,
	GMC_SIGNAL_QUERY_REQUISITION,
	GMC_SIGNAL_MAX
};
/**
 * GnomenuClientClass:
 * @menu_create: the virtual function invoked.
 */
struct _GnomenuClientClass {
	GObjectClass parent;
/*< private >*/	
	guint signals[GMC_SIGNAL_MAX];
	GType * type_gnomenu_message_type;

	void (*server_new)(GnomenuClient * self, GnomenuClientServerInfo * server_info);
	void (*server_destroy)(GnomenuClient * self, GnomenuClientServerInfo * server_info);
	void (*allocate_size)(GnomenuClient * self, GnomenuClientServerInfo * server_info, GtkAllocation * allocation);
	void (*query_requisition)(GnomenuClient * self, GnomenuClientServerInfo * server_info, GtkRequisition * req);
};

/**
 * GNOMENU_CLIENT_NAME:
 *
 * Name of the socket
 */
#define GNOMENU_CLIENT_NAME "GNOME MENU CLIENT"

G_END_DECLS
#endif
