#ifndef GNOMENU_MESSAGE_H
#define GNOMENU_MESSAGE_H
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "socket.h"
G_BEGIN_DECLS
/**
 * SECTION: messages
 * @title: Message Types
 * @short_description: Messaging is the way Menu Servers talks to MenuBars.
 * @see_also:	#GnomenuServerHelper, #GnomenuClientHelper, #GnomenuSocket
 * @stablility: Unstable
 * @include:	libgnomenu/messages.h
 *
 * Here is the declears of the messages passed around in libgnomenu. 
 * You should not use it in any application other than developing the global
 * menu itself. They are documented only for clearity. 
 *
 * #GnomenuServerHelper and #GnomenuClientHelper encapsulate
 * these messages, use them.
 */

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

/**
 * GnomenuMessageType:
 * @GNOMENU_MSG_ANY: 			#GnomenuMessageAny
 * @GNOMENU_MSG_CLIENT_NEW: 	#GnomenuMessageClientNew
 * @GNOMENU_MSG_CLIENT_DESTROY: #GnomenuMessageClientDestroy
 * @GNOMENU_MSG_SERVER_NEW: 	#GnomenuMessageServerNew
 * @GNOMENU_MSG_CLIENT_REALIZE: #GnomenuMessageClientRealize
 * @GNOMENU_MSG_CLIENT_REPARENT: #GnomenuMessageClientReparent
 * @GNOMENU_MSG_CLIENT_UNREALIZE: #GnomenuMessageClientUnrealize
 * @GNOMENU_MSG_SERVER_DESTROY: #GnomenuMessageServerDestroy
 * @GNOMENU_MSG_SIZE_REQUEST: 	#GnomenuMessageSizeRequest
 * @GNOMENU_MSG_SIZE_ALLOCATE:	#GnomenuMessageSizeAllocate
 * @GNOMENU_MSG_SIZE_QUERY:		#GnomenuMessageSizeQuery
 * @GNOMENU_MSG_POSITION_SET:	#GnomenuMessagePositionSet
 * @GNOMENU_MSG_VISIBILITY_SET: #GnomenuMessageVisibilitySet
 * @GNOMENU_MSG_ORIENTATION_CHANGE: #GnomenuMessageOrientationChange
 * @GNOMENU_MSG_BGCOLOR_SET:	#GnomenuMessageBgColorSet
 * @GNOMENU_MSG_BGPIXMAP_SET:	#GnomenuMessageBgPixmapSet
 * @GNOMENU_MSG_PARENT_FOCUS:		#GnomenuMessageToplevelFocus
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
	GNOMENU_MSG_POSITION_SET,
	GNOMENU_MSG_VISIBILITY_SET,
	GNOMENU_MSG_ORIENTATION_CHANGE,
	GNOMENU_MSG_BGCOLOR_SET,
	GNOMENU_MSG_BGPIXMAP_SET,
	GNOMENU_MSG_PARENT_FOCUS,
	GNOMENU_MSG_MAX,
} GnomenuMessageType;
#define GnomenuMessageType gchar
/**
 * GnomenuMessageAny:
 * @type:	#GNOMENU_MSG_ANY
 * @data:	detailed data of the message;
 *
 * An generic message. useless if not debugging.
 */
typedef struct {
	GnomenuMessageType type;
	guint32 data[2];
} GnomenuMessageAny;

/**
 * GnomenuMessageClientNew:
 * @type: 	#GNOMENU_MSG_CLIENT_NEW
 *
 * A client socket is created.
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageClientNew;

/**
 * GnomenuMessageClientRealize:
 * 	@type: #GNOMENU_MSG_CLIENT_REALIZE
 * 	@ui_window: window to grab.
 *
 * The client has been realized;
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow ui_window;
} GnomenuMessageClientRealize;

/** 
 * GnomenuMessageClientReparent:
 * 	@type: #GNOMENU_MSG_CLIENT_REPARENT
 * 	@parent_window: new parent window.
 *
 * The client has been reparented
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow parent_window;
} GnomenuMessageClientReparent;

/** 
 * GnomenuMessageClientUnrealize:
 * 	@type: #GNOMENU_MSG_CLIENT_UNREALIZE
 *
 * The client has been unrealized;
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageClientUnrealize;
/**
 * GnomenuMessageClientDestroy:
 * 	@type: #GNOMENU_MSG_CLIENT_DESTROY
 *
 * A client is destroyed. This message is not used at all. Because
 * GnomenuSocket handles shutdown.
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageClientDestroy;
/**
 * GnomenuMessageServerNew:
 * 	@type: #GNOMENU_MSG_SERVER_NEW
 * 	@socket_id: the server 
 *
 * When a server is launched it broadcast this message; then the 
 * client can response by making connections.
 */
typedef struct {
	GnomenuMessageType type;
	GnomenuSocketNativeID socket_id;
} GnomenuMessageServerNew;
/**
 * GnomenuMessageServerDestroy:
 * @type: #GNOMENU_MSG_SERVER_DESTROY
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageServerDestroy;

/**
 * GnomenuMessageSizeQuery:
 * @type: #GNOMENU_MSG_SIZE_QUERY
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
 * @width:
 * @height:
 *
 * See #GnomenuMessageQuerySize for a complete description of a size allocation chain.
 */
typedef struct {
	GnomenuMessageType type;
	gint16	width;
	gint16 	height;
} GnomenuMessageSizeRequest;

/**
 * GnomenuMessageSizeAllocate:
 * @type: #GNOMENU_MSG_SIZE_ALLOCATE
 * @width: width
 * @height: height
 *
 * See #GnomenuMessageQuerySize for a complete description of a size allocation chain.
 */
typedef struct {
	GnomenuMessageType type;
	gint16 width;
	gint16 height;
} GnomenuMessageSizeAllocate;

typedef enum {
	GNOMENU_ORIENT_TOP,
	GNOMENU_ORIENT_LEFT,
	GNOMENU_ORIENT_RIGHT,
	GNOMENU_ORIENT_BOTTOM,
} GnomenuOrientation;
/**
 * GnomenuMessageOrientationChange:
 * 	@type: #GNOMENU_MSG_ORIENTATION_CHANGE
 * 	@orientation: new orientation
 *
 *	Server requests the client to change its orientation.
 */
#define GnomenuOrientation gchar
typedef struct {
	GnomenuMessageType type;
	GnomenuOrientation orientation;
} GnomenuMessageOrientationChange;
#undef GnomenuOrientation 
/**
 * GnomenuMessagePositionSet:
 *  @type: #GNOMENU_MSG_POSITION_SET
 *  @x: x
 *  @y: y
 *
 * To move the menubar. This messeage is implemented. 
 * 	There is no reason use it at all.
 */
typedef struct {
	GnomenuMessageType type;
	gint16 x;
	gint16 y;
} GnomenuMessagePositionSet;
/**
 * GnomenuMessageVisibilitySet:
 *  @type: #GNOMENU_MSG_VISIBILITY_SET
 *  @visibility: TRUE=show, FALSE=hide
 *
 *  To show or hide the menu bar.
 */
typedef struct {
	GnomenuMessageType type;
	gboolean visibility;
} GnomenuMessageVisibilitySet;

/**
 * GnomenuMessageBgColorSet:
 *	@type: 
 *	@red: red component;
 *	@green: green component;
 *	@blue: blue component;
 *
 * To set the bg color of the menubar.
 */
typedef struct {
	GnomenuMessageType type;
	guint16 red;
	guint16 green;
	guint16 blue;
} GnomenuMessageBgColorSet;

/**
 * GnomenuMessageBgPixmapSet:
 * @type: 
 * @pixmap: XID of the pixmap.
 *
 * To set the bg color of the menubar.
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow pixmap;
} GnomenuMessageBgPixmapSet;

/**
 * GnomenuMessageParentFocus:
 * @type: 
 *
 * To set the bg color of the menubar.
 */
typedef struct {
	GnomenuMessageType type;
} GnomenuMessageParentFocus;

#undef GnomenuMessageType
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
		GnomenuMessageVisibilitySet visibility_set;
		GnomenuMessagePositionSet position_set;
		GnomenuMessageBgColorSet bgcolor_set;
		GnomenuMessageBgPixmapSet bgpixmap_set;
		GnomenuMessageParentFocus parent_focus;
	};
};
#define GNOMENU_TYPE_MESSAGE gnomenu_message_get_type()
GType gnomenu_message_get_type (void) ;



G_END_DECLS
#endif
