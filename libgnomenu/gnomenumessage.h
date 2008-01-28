#ifndef GNOMENU_MESSAGE_H
#define GNOMENU_MESSAGE_H
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
G_BEGIN_DECLS
/**
 * SECTION: gnomenumessage
 * @short_description: The underground messages for gnomenu.
 * @see_also:	#GnomenuServer, #GnomenuClient #GdkSocket
 * @stablility: Unstable
 * @include:	libgnomenu/gnomenumessage.h
 *
 * gnomenumessage is the data type for the messages passed around in libgnomenu.
 */

/**
 * GnomenuMessageType:
 * GNOMENU_MSG_ANY: any type of message
 * GNOMENU_MSG_CLIENT_NEW: if a client notifies this server its creation.
 * GNOMENU_MSG_CLIENT_REPARENT: if a client's logical parent window is changed.
 * type of a libgnomenu message.
 */
typedef enum { /*< prefix=GNOMENU >*/
	GNOMENU_MSG_ANY,
	GNOMENU_MSG_CLIENT_NEW,
	GNOMENU_MSG_CLIENT_DESTROY,
	GNOMENU_MSG_SIZE_REQUEST,
	GNOMENU_MSG_SERVER_NEW,
	GNOMENU_MSG_SERVER_DESTROY,
	GNOMENU_MSG_MAX,
} GnomenuMessageType;

/**
 * GnomenuMessageAny:
 * @type:	type of the message;
 * @data:	detailed data of the message;
 *
 * An generic message. useless if not debugging.
 */
typedef struct {
	GnomenuMessageType type;
	gulong data[3];
} GnomenuMessageAny;

/**
 * GnomenuMessageClientNew:
 * @type: 
 * @socket_id:
 *
 * FIXME: should seperate into three different messages.
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow socket_id;
	GdkNativeWindow ui_window;
	GdkNativeWindow parent_window;
} GnomenuMessageClientNew;

/**
 * GnomenuMessageSizeRequest:
 * @type:
 * @width:
 * @height:
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow socket_id;
	gint	width;
	gint 	height;
} GnomenuMessageSizeRequest;
/**
 * GnomenuMessageClientDestroy:
 * @type:
 * @socket_id:
 *
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow socket_id;
} GnomenuMessageClientDestroy;
/**
 * GnomenuMessageServerNew:
 * 	@type:
 * 	@socket_id:
 * 	@container_window:	perhaps this is useless in current implement;
 * 		However, if we are moving to Etolite's WildMenu alike implementation(
 * 		i.e. the client takes care of everything)
 * 		This field will be useful.
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow socket_id;
	GdkNativeWindow container_window;
} GnomenuMessageServerNew;
/**
 * GnomenuMessageServerDestroy:
 * @type:
 * @socket_id:
 */
typedef struct {
	GnomenuMessageType type;
	GdkNativeWindow socket_id;
} GnomenuMessageServerDestroy;
/**
 * GnomenuMessage:
 * @any: general form of message;
 *
 * Message.
 */
struct _GnomenuMessage {
	union {
		GnomenuMessageAny any;
		GnomenuMessageClientNew client_new;
		GnomenuMessageClientDestroy client_destroy;
		GnomenuMessageSizeRequest	size_request;
		GnomenuMessageServerNew server_new;
		GnomenuMessageServerDestroy server_destroy;
	};
};
typedef struct _GnomenuMessage GnomenuMessage;
G_END_DECLS
#endif
