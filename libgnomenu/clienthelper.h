#ifndef GNOMENU_CLIENT_H
#define GNOMENU_CLIENT_H

#include "gdksocket.h"
#include "gnomenumessage.h"

G_BEGIN_DECLS

/**
 * SECTION: clienthelper
 * @short_description: Menu client Helper.
 * @see_also: #GtkMenuBar, #GtkGlobalMenuBar, #GdkSocket,
 * @stablility: Unstable
 * @include: libgnomenu/clienthelper.h
 *
 * GnomenuClientHelper provides fundanmental messaging mechanism for a menu client
 */

#define GNOMENU_TYPE_CLIENT_HELPER	(gnomenu_client_helper_get_type())
#define GNOMENU_CLIENT_HELPER(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_CLIENT_HELPER, GnomenuClientHelper))
#define GNOMENU_CLIENT_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_CLIENT_HELPER, GnomenuClientHelperClass))
#define GNOMENU_IS_CLIENT_HELPER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_CLIENT_HELPER))
#define GNOMENU_IS_CLIENT_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_CLIENT_HELPER))
#define GNOMENU_CLIENT_HELPER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_CLIENT_HELPER, GnomenuClientHelperClass))

typedef struct _GnomenuClientHelperClass GnomenuClientHelperClass;
typedef struct _GnomenuClientHelper GnomenuClientHelper;

/**
 * GnomenuServerInfo:
 * @socket_id: the native id of the socket which the server is using.
 *
 * GnomenuServerInfo is used by GnomenuClientHelper to store server info.
 */
typedef struct _GnomenuServerInfo {
	GdkSocketNativeID socket_id;
} GnomenuServerInfo;
/**
 * GnomenuClientHelper:
 *
 * GnomenuClientHelper provides fundanmental messaging mechanism for a menu server
 */
struct _GnomenuClientHelper {
	GdkSocket parent;
	GnomenuServerInfo * server_info;
/*< public >*/
};


/*< private >*/
enum {
	GMC_SIGNAL_SERVER_NEW,
	GMC_SIGNAL_SERVER_DESTROY,
	GMC_SIGNAL_SIZE_ALLOCATE,
	GMC_SIGNAL_SIZE_QUERY,
	GMC_SIGNAL_MAX
};
/**
 * GnomenuClientHelperClass:
 * @menu_create: the virtual function invoked.
 */
struct _GnomenuClientHelperClass {
	GdkSocketClass parent;
/*< private >*/	
	guint signals[GMC_SIGNAL_MAX];
	GType * type_gnomenu_message_type;

	void (*server_new)(GnomenuClientHelper * self, GnomenuServerInfo * server_info);
	void (*server_destroy)(GnomenuClientHelper * self, GnomenuServerInfo * server_info);
	void (*size_allocate)(GnomenuClientHelper * self, GtkAllocation * allocation);
	void (*size_query)(GnomenuClientHelper * self, GtkRequisition * req);
};

/**
 * GNOMENU_CLIENT_NAME:
 *
 * Name of the socket
 */
#define GNOMENU_CLIENT_NAME "GNOME MENU CLIENT"

G_END_DECLS
#endif
