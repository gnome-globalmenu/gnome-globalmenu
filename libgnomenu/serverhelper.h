#ifndef GNOMENU_SERVER_H
#define GNOMENU_SERVER_H

#include "socket.h"

G_BEGIN_DECLS
/**
 * SECTION: serverhelper
 * @short_description: Menu server helper class.
 * @see_also: #GnomenuClientHelper, #GtkGlobalMenuBar, #GdkSocket.
 * @stablility: Unstable
 * @include: libgnomenu/serverhelper.h
 *
 * GnomenuServerHelper provides fundanmental messaging 
 * mechanism for a menu server
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
 *  @clients: A List of all the clients, the node data is
 *  	always a #GnomenuClientInfo struct.
 *
 * Server helper object.
 */
typedef struct _GnomenuServerHelper GnomenuServerHelper;
struct _GnomenuServerHelper{
	GnomenuSocket parent;
/*< public >*/
	GList * clients;
};

/**
 * GnomenuClientInfo:
 * 	@service: the #GnomenuSocket used to serve this client.
 * 	@ui_window: the container window which contains 
 * 			all the ui elements of the menu;
 * 			A menu server application will reparent it, but nothing more.
 * 			Anything else is done in the owner process of the menu bar,
 * 			upon the request by the server.
 * 	@parent_window: the parent window of the menubar. 
 * 		The menu server application keeps track of 
 * 		the changing of the focused window, and takes care of switching 
 * 		the active menu. 
 *	@requisition: the size requisition.
 * 	@allocation: the size allocation. both x, y, width, height are defined.
 * 	@stage: indicates the most recent state the server knows that the
 * 		client is in.
 * 	@size_stage: a typical sizing chain is defined in #GnomenuMessageSizeQuery.
 *
 * This structure stores client infomation;
 */
typedef struct _GnomenuClientInfo GnomenuClientInfo;

struct _GnomenuClientInfo {
	GnomenuSocket * service;
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
	GnomenuSocketClass parent;
/*< private >*/	

	void (*client_new) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_destroy) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_size_request) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_realize) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_unrealize) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_reparent) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
	void (*client_parent_focus) (GnomenuServerHelper * self, GnomenuClientInfo * client_info);
};

GnomenuServerHelper *gnomenu_server_helper_new(void);
GnomenuClientInfo * gnomenu_server_helper_find_client_by_socket_id(
		GnomenuServerHelper * self,
		GnomenuSocketNativeID socket_id);
GnomenuClientInfo * gnomenu_server_helper_find_client_by_parent_window(
		GnomenuServerHelper * self,
		GdkNativeWindow parent_window);

void gnomenu_server_helper_queue_resize(GnomenuServerHelper * self, GnomenuClientInfo * ci);
void gnomenu_server_helper_allocate_size(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GtkAllocation * allocation);
void gnomenu_server_helper_set_orientation(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GtkOrientation ori);
void gnomenu_server_helper_set_position(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GdkPoint * position);
void gnomenu_server_helper_set_visibility(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			gboolean vis);
void gnomenu_server_helper_set_background(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GdkColor * color, GdkPixmap * pixmap);
gboolean gnomenu_server_helper_start(GnomenuServerHelper * self);
G_END_DECLS
#endif
