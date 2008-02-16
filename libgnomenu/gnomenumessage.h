#ifndef GNOMENU_MESSAGE_H
#define GNOMENU_MESSAGE_H
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "gdksocket.h"
G_BEGIN_DECLS
/**
 * SECTION: gnomenumessage
 * @title: GnomenuMessage
 * @short_description: The underground messages for libgnomenu.
 * @see_also:	#GnomenuServer, #GnomenuClient #GdkSocket
 * @stablility: Unstable
 * @include:	libgnomenu/gnomenumessage.h
 *
 * gnomenumessage is the data type for the messages passed around in libgnomenu.
 */

/**
 * GnomenuMessageType:
 * @GNOMENU_MSG_ANY: 			#GnomenuMessageAny
 * @GNOMENU_MSG_CLIENT_NEW: 	#GnomenuMessageClientNew
 * @GNOMENU_MSG_CLIENT_DESTROY: #GnomenuMessageClientDestroy
 * @GNOMENU_MSG_SERVER_NEW: 	#GnomenuMessageServerNew
 * @GNOMENU_MSG_CLIENT_REALIZE: #GnmenuMessageClientRealize
 * @GNOMENU_MSG_CLIENT_REPARENT: #GnomenuMessageClientReparent
 * @GNOMENU_MSG_CLIENT_UNREALIZE: #GnomenuMessageClientUnrealize
 * @GNOMENU_MSG_SERVER_DESTROY: #GnomenuMessageServerDestroy
 * @GNOMENU_MSG_SIZE_REQUEST: 	#GnomenuMessageSizeRequest
 * @GNOMENU_MSG_SIZE_ALLOCATE:	#GnomenuMessageSizeAllocate
 * @GNOMENU_MSG_SIZE_QUERY:		#GnomenuMessageSizeQuery
 * @GNOMENU_MSG_MAX:		no meaning
 *
 * type of a libgnomenu message.
 */
typedef enum { /*< prefix=GNOMENU >*/
	GNOMENU_MSG_ANY,
	GNOMENU_MSG_CLIENT_NEW,
	GNOMENU_MSG_CLIENT_REALIZE,
	GNOMENU_MSG_CLIENT_REPARENT,
	GNOMENU_MSG_CLIENT_UNREALIZE,
	GNOMENU_MSG_CLIENT_DESTROY,
	GNOMENU_MSG_SERVER_NEW,
	GNOMENU_MSG_SERVER_DESTROY,
	GNOMENU_MSG_SIZE_ALLOCATE,
	GNOMENU_MSG_SIZE_REQUEST,
	GNOMENU_MSG_SIZE_QUERY,
	GNOMENU_MSG_ORIENTATION_CHANGE,
	GNOMENU_MSG_MAX,
} GnomenuMessageType;

/**
 * GnomenuMessageAny:
 * @type:	#GNOMENU_MSG_ANY
 * @data:	detailed data of the message;
 *
 * An generic message. useless if not debugging.
 */
typedef struct {
	GnomenuMessageType type;
	gulong data[2];
} GnomenuMessageAny;

/**
 * GnomenuMessageClientNew:
 * @type: 	#GNOMENU_MSG_CLIENT_NEW
 * @socket_id:
 *
 * A client socket is created.
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageClientNew;

/**
 * GnomenuMessageClientRealize:
 * @type: #GNOMENU_MSG_CLIENT_REALIZE
 * @socket_id:
 * @ui_window:
 *
 * A client has been realized;
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow ui_window;
} GnomenuMessageClientRealize;

/** GnomenuMessageClientReparent:
 * @type: #GNOMENU_MSG_CLIENT_REPARENT
 * @socket_id:
 * @parent_window:
 *
 * A client has been reparented
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow parent_window;
} GnomenuMessageClientReparent;

/** GnomenuMessageClientUnrealize:
 * @type: #GNOMENU_MSG_CLIENT_UNREALIZE
 * @socket_id:
 *
 * A client has been realized;
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageClientUnrealize;
/**
 * GnomenuMessageClientDestroy:
 * @type: #GNOMENU_MSG_CLIENT_DESTROY
 * @socket_id:
 *
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageClientDestroy;
/**
 * GnomenuMessageServerNew:
 * 	@type: #GNOMENU_MSG_SERVER_NEW
 * 	@socket_id:
 * 	@container_window:	REMOVED. should be in ServerRealize, which is not implemented yet
 * 		perhaps this is useless in current implement;
 * 		However, if we are moving to Etolite's WildMenu alike implementation(
 * 		i.e. the client takes care of everything)
 * 		This field will be useful.
 *
 *  This message is only intended to be a response of ClientNew message.
 */
typedef struct {
	GnomenuMessageType type;
	GdkSocketNativeID socket_id;
} GnomenuMessageServerNew;
/**
 * GnomenuMessageServerDestroy:
 * @type: #GNOMENU_MSG_SERVER_DESTROY
 * @socket_id:
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageServerDestroy;

/**
 * GnomenuMessageSizeQuery:
 * @type: #GNOMENU_MSG_SIZE_QUERY
 * @socket_id: native socket id of the server.
 *
 * A size query message begins a sequency of size allocation operation
 * between the server and client.
 *
 * 1. server sends a #GnomenuMessageSizeQuery to the client.
 *
 * 2. client receives the message, calculates its requisition, then send
 *    a #GnomenuMessageSizeRequest to server.
 *
 * 3. server receives the #GnomenuMessageSizeRequest message, allocate
 * 	  the allocation for the client, and send a #GnomenuMessageSizeAllocate
 * 	  to the client.
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageSizeQuery;

/**
 * GnomenuMessageSizeRequest:
 * @type: #GNOMENU_MSG_SIZE_REQUEST
 * @socket_id: the native socket id for the client's socket.
 * @width:
 * @height:
 *
 * See #GnomenuMessageQuerySize for a complete description of a size allocation chain.
 */
typedef struct {
	GnomenuMessageType type;
	gint	width;
	gint 	height;
} GnomenuMessageSizeRequest;

/**
 * GnomenuMessageSizeAllocate:
 * @type: #GNOMENU_MSG_SIZE_ALLOCATE
 * @socket_id: the native socket id for the server socket.
 * @width:
 * @height:
 *
 * See #GnomenuMessageQuerySize for a complete description of a size allocation chain.
 */
typedef struct {
	GnomenuMessageType type;
	gint width;
	gint height;
} GnomenuMessageSizeAllocate;

/**
 * GnomenuMessageOrientationChange:
 * @type: #GNOMENU_MSG_ORIENTATION_CHANGE
 * @socket_id: the native socket id for the server socket.
 * @orientation: new orientation
 *
 */
typedef struct {
	GnomenuMessageType type;
	GtkOrientation orientation;
} GnomenuMessageOrientationChange;
/**
 * GnomenuMessage:
 *
 * This structure wraps every kind of gnomenu message into a union.
 */
typedef struct _GnomenuMessage GnomenuMessage;
struct _GnomenuMessage {
	union {
		GnomenuMessageAny any;
		GnomenuMessageClientNew client_new;
		GnomenuMessageClientRealize client_realize;
		GnomenuMessageClientReparent client_reparent;
		GnomenuMessageClientUnrealize client_unrealize;
		GnomenuMessageClientDestroy client_destroy;
		GnomenuMessageServerNew server_new;
		GnomenuMessageServerDestroy server_destroy;
		GnomenuMessageSizeRequest	size_request;
		GnomenuMessageSizeAllocate	size_allocate;
		GnomenuMessageSizeQuery	size_query;
		GnomenuMessageOrientationChange orientation_change;
	};
};
#define GNOMENU_TYPE_MESSAGE gnomenu_message_get_type()
GType gnomenu_message_get_type (void) ;

/**
 * GNOMENU_CLIENT_NAME:
 *
 * Name of the socket for client helper
 */
#define GNOMENU_CLIENT_NAME "GNOME MENU CLIENT"

/**
 * GNOMENU_SERVER_NAME:
 *
 * Name of the socket for server helper
 */
#define GNOMENU_SERVER_NAME "GNOME MENU SERVER"


G_END_DECLS
#endif
