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
 *
 * type of a libgnomenu message.
 */
typedef enum { /*< prefix=GNOMENU >*/
	GNOMENU_MSG_ANY,
	GNOMENU_MSG_MAX,
} GnomenuMessageType;
/**
 * GnomenuMessageAny:
 * @type:	type of the message;
 * @data:	detailed data of the message;
 *
 * An generic message. useless if not debugging.
 */
typedef struct _GnomenuMessageAny {
	GnomenuMessageType type;
	gulong data[3];
} GnomenuMessageAny;

/**
 * GnomenuMessage:
 * @any: general form of message;
 *
 * Message.
 */
struct _GnomenuMessage {
	union {
		GnomenuMessageAny any;
		
	};
};
typedef struct _GnomenuMessage GnomenuMessage;
G_END_DECLS
#endif
