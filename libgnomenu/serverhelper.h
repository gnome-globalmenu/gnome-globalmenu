#ifndef GNOMENU_SERVER_H
#define GNOMENU_SERVER_H

#include "gdksocket.h"
#include "gnomenumessage.h"

G_BEGIN_DECLS
/**
 * SECTION: serverhelper
 * @short_description: Menu server herlper class.
 * @see_also: #GtkMenuBar, #GtkGlobalMenuBar, #GdkSocket.
 * @stablility: Unstable
 * @include: libgnomenu/serverhelper.h
 *
 * GnomenuServerHelper provides fundanmental messaging mechanism for a menu server
 * 
 */

#define GNOMENU_TYPE_SERVER_HELPER	(gnomenu_server_helper_get_type())
#define GNOMENU_SERVER_HELPER(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_SERVER_HELPER, GnomenuServerHelper))
#define GNOMENU_SERVER_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_SERVER_HELPER, GnomenuServerHelperClass))
#define GNOMENU_IS_SERVER_HELPER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_SERVER_HELPER))
#define GNOMENU_IS_SERVER_HELPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_SERVER_HELPER))
#define GNOMENU_SERVER_HELPER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_SERVER_HELPER, GnomenuServerHelperClass))

typedef struct _GnomenuServerHelperClass GnomenuServerHelperClass;

/**
 * GnomenuServerHelper:
 *  @clients: A List of all the clients, each of which is GnomenuClientInfo
 *
 * GnomenuServerHelper provides fundanmental messaging mechanism for a menu server
 */
typedef struct _GnomenuServerHelper GnomenuServerHelper;
struct _GnomenuServerHelper{
	GdkSocket parent;
/*< public >*/
	GList * clients;
};

/**
 * GnomenuServerHelperClientInfo:
 * @socket_id: the native id of the #GdkSocket the client is using. This value is
 * 			returned by #gdk_socket_get_native;
 * @ui_window: the container window which contains all the ui elements of the menu;
 * 			A menu server application can steal it and arrange its position.
 * @parent_window: the parent window of the menubar. The menu server application take
 * 		care of the changing of the focused window, and takes care of switching 
 * 		the active menu. 
 *
 * This structure is where #GnomenuServerHelper stores client infomation;
 */
typedef struct _GnomenuClientInfo GnomenuClientInfo;

struct _GnomenuClientInfo {
	GdkSocket * service;
	GdkNativeWindow ui_window;
	GdkNativeWindow parent_window;
	GtkRequisition requisition;
	GtkAllocation allocation;
	enum {
		GNOMENU_CI_STAGE_NEW,
		GNOMENU_CI_STAGE_REALIZED,
		GNOMENU_CI_STAGE_UNREALIZED,
		GNOMENU_CI_STAGE_DESTROYED,
	} stage;
	enum {
		GNOMENU_CI_STAGE_QUERYING,
		GNOMENU_CI_STAGE_RESPONSED,
		GNOMENU_CI_STAGE_RESOLVED
	} size_stage;
};

/**
 * GnomenuServerHelperClass:
 * @menu_create: the virtual function invoked.
 */
struct _GnomenuServerHelperClass {
	GdkSocketClass parent;
/*< private >*/	
	GType * type_gnomenu_message_type;

	void (*client_new) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_destroy) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_size_request) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_realize) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_unrealize) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_reparent) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
};

GnomenuServerHelper *gnomenu_server_helper_new(void);
GnomenuClientInfo * gnomenu_server_helper_find_client_by_socket_id(
		GnomenuServerHelper * self,
		GdkSocketNativeID socket_id);
GnomenuClientInfo * gnomenu_server_helper_find_client_by_parent_window(
		GnomenuServerHelper * self,
		GdkNativeWindow parent_window);
void gnomenu_server_helper_client_queue_resize(GnomenuServerHelper * self, GnomenuClientInfo * ci);
void gnomenu_server_helper_client_set_orientation(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GtkOrientation ori);
G_END_DECLS
#endif
