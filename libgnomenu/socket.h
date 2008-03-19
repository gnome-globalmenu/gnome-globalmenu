
#ifndef GDK_SOCKET_H
#define GDK_SOCKET_H
#include <gdk/gdk.h>

G_BEGIN_DECLS

/**
 * SECTION: socket
 * @short_description: Socket Communication for GNOMEU
 * @see_also: #GdkWindow, #GnomenuClientHelper, #GnomenuServerHelper
 * @stability: Unstable
 * @include: libgnomenu/socket.h
 *
 * GdkSocket handles inter-process communication of GTK applications. 
 * It is the fundanmental communication mechanism for #libgnomenu.
 */

#define GNOMENU_TYPE_SOCKET 	(gnomenu_socket_get_type())
#define GNOMENU_SOCKET(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_SOCKET, GnomenuSocket))
#define GNOMENU_SOCKET_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_SOCKET, GnomenuSocketClass))
#define GNOMENU_IS_SOCKET(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_SOCKET))
#define GNOMENU_IS_SOCKET_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_SOCKET))
#define GNOMENU_SOCKET_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_SOCKET, GnomenuSocketClass))


typedef struct _GnomenuSocketClass GnomenuSocketClass;
typedef struct _GnomenuSocket GnomenuSocket;
/**
 * GnomenuSocketNativeID:
 *
 * The native id for GnomenuSocket, used for uniquely labelling a socket.
 * In current implement of #GnomenuSocket, it is #GdkNativeWindow.
 *
 * TODO: perhaps we'll move to dbus in the future.
 */
typedef GdkNativeWindow GnomenuSocketNativeID;
/** 
 * GnomenuSocketStatus:
 *   @GNOMENU_SOCKET_DISCONNECTED: A newly created GnomenuSocket, the associated GnomenuWindow is created.
 *   @GNOMENU_SOCKET_CONNECTED: The socket is connected to somewhere.
 *   @GNOMENU_SOCKET_LISTEN: The socket is a server and is waiting for CONNECT_REQ
 * 	@GNOMENU_SOCKET_STATUS_MAX: max value, no meaning.
 *
 * The status of a #GnomenuSocket.
 * TODO: write more about how status changes.
 */
typedef enum { /*< prefix = GNOMENU_SOCKET >*/
	GNOMENU_SOCKET_DISCONNECTED,
	GNOMENU_SOCKET_CONNECTED,
	GNOMENU_SOCKET_LISTEN,
	GNOMENU_SOCKET_STATUS_MAX
} GnomenuSocketStatus;

/**
 * GnomenuSocket:
 * 	@name:	name of the socket.
 * 	@window: the #GdkWindow used to receive messages. Its #GdkWindow::title will be @name.
 * 	@display: the #GdkDisplay this socket belongs to. Though we can always obtain 
 * 		this information from @window, we cache it here for saving code lines.
 * 	@status: the status. See #GnomenuSocketStatus.
 *  @target: to whom this socket is connected
 *  @queue: message buffer, 
 *	@acks: number of ACKs received. (and without send a DATA) if @acks > 0, 
 *			it is ok to directly send message to the server without push it to the queue
 *			and wait for a ACK.
 *	       if we send message to the server decrease @acks by 1.
 *  @timeout: number of seconds for a connection to timeout.
 *  @alives: number of replied ISALIVE messages.
 *  
 *  The GnomenuSocket object.
 */
struct _GnomenuSocket {
	GObject parent;
/*< public >*/
	gchar * name;
	GnomenuSocketStatus status;
	gint timeout;
};

/**
 * GnomenuSocketClass:
 *   @data_arrival_signal_id: the signal id for ::data-arrival.
 *   @data_arrival_cleanup: the cleanup call back for ::data-arrival signal
 */
struct _GnomenuSocketClass {
	GObjectClass parent;
/* < private >*/
/* closures*/
	void (*c_data) (GnomenuSocket * socket, gpointer data, guint length);
	void (*c_request) (GnomenuSocket * socket, GnomenuSocketNativeID target);
	void (*c_shutdown) (GnomenuSocket * socket);
	void (*c_connected) (GnomenuSocket * socket, GnomenuSocketNativeID target);

/* methods */
	gboolean (*connect) (GnomenuSocket * socket, GnomenuSocketNativeID target);
	gboolean (*listen) (GnomenuSocket * socket);
	gboolean (*accept) (GnomenuSocket * socket, GnomenuSocket * service, GnomenuSocketNativeID target);
	gboolean (*send) (GnomenuSocket * socket, gpointer data, guint bytes);
	gboolean (*broadcast) (GnomenuSocket * socket, gpointer data, guint bytes);
	void (*shutdown) (GnomenuSocket * socket);	
	gboolean (*flush) (GnomenuSocket * socket);	

	gpointer reserved1;
	gpointer reserved2;
	gpointer reserved3;
	gpointer reserved4;
	gpointer reserved5;
	gpointer reserved6;
	gpointer reserved7;
	gpointer reserved8;
	gpointer reserved9;
	gpointer reserved10;
};


GType gnomenu_socket_get_type (void);

GnomenuSocket * gnomenu_socket_new (gchar * name, gint timeout);
GnomenuSocketNativeID gnomenu_socket_get_native(GnomenuSocket * socket);

gboolean gnomenu_socket_listen(GnomenuSocket * socket);
gboolean gnomenu_socket_accept(GnomenuSocket * socket, GnomenuSocket * service, GnomenuSocketNativeID target);

gboolean gnomenu_socket_send(GnomenuSocket * socket, gpointer data, guint bytes);

void gnomenu_socket_shutdown(GnomenuSocket * socket);
gboolean gnomenu_socket_flush(GnomenuSocket * socket);
gboolean gnomenu_socket_broadcast_by_name(GnomenuSocket * socket, gchar * name, gpointer data, guint bytes);
GnomenuSocketNativeID gnomenu_socket_lookup(gchar * name);
G_END_DECLS
#endif

