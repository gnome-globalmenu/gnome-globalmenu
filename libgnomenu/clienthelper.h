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
 * GnomenuClientHelper:
 *
 * GnomenuClientHelper provides fundanmental messaging mechanism for a menu server
 */
struct _GnomenuClientHelper {
	GdkSocket parent;
/*< public >*/
};


/**
 * GnomenuClientHelperClass:
 * @menu_create: the virtual function invoked.
 */
struct _GnomenuClientHelperClass {
	GdkSocketClass parent;
/*< private >*/	

	void (*server_new)(GnomenuClientHelper * self);
	void (*server_destroy)(GnomenuClientHelper * self);
	void (*size_allocate)(GnomenuClientHelper * self, GtkAllocation * allocation);
	void (*size_query)(GnomenuClientHelper * self, GtkRequisition * req);
	void (*orientation_change)(GnomenuClientHelper * self, GtkOrientation ori);
};



GnomenuClientHelper * gnomenu_client_helper_new(void);
void gnomenu_client_helper_send_realize(GnomenuClientHelper * self, GdkWindow * ui_window);
void gnomenu_client_helper_send_reparent(GnomenuClientHelper * self, GdkWindow * parent_window);
void gnomenu_client_helper_send_unrealize(GnomenuClientHelper * self); 

G_END_DECLS
#endif
