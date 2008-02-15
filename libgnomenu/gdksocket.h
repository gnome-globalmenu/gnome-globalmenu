
#ifndef GDK_SOCKET_H
#define GDK_SOCKET_H
#include <gdk/gdk.h>

G_BEGIN_DECLS

/**
 * SECTION: gdksocket
 * @short_description: Socket Communication for GTK.
 * @see_also: #GdkWindow, #GtkGlobalMenuBar
 * @stability: Unstable
 * @include: libgnomenu/gdksocket.h
 *
 * GdkSocket handles inter-process communication of GTK applications. 
 * It is the fundanmental communication mechanism for #libgnomenu.
 */

#define GDK_TYPE_SOCKET 	(gdk_socket_get_type())
#define GDK_SOCKET(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GDK_TYPE_SOCKET, GdkSocket))
#define GDK_SOCKET_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GDK_TYPE_SOCKET, GdkSocketClass))
#define GDK_IS_SOCKET(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDK_TYPE_SOCKET))
#define GDK_IS_SOCKET_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GDK_TYPE_SOCKET))
#define GDK_SOCKET_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GDK_TYPE_SOCKET, GdkSocketClass))


typedef struct _GdkSocketClass GdkSocketClass;
typedef struct _GdkSocket GdkSocket;
/**
 * GdkSocketNativeID:
 *
 * The native id for GdkSocket, used for uniquely labelling a socket.
 * In current implement of #GdkSocket, it is #GdkNativeWindow.
 *
 * TODO: perhaps we'll move to dbus in the future.
 */
typedef GdkNativeWindow GdkSocketNativeID;
/** 
 * GdkSocketStatus:
 *   @GDK_SOCKET_DISCONNECTED: A newly created GdkSocket, the associated GdkWindow is created.
 *   @GDK_SOCKET_CONNECTED: The socket is connected to somewhere.
 *   @GDK_SOCKET_LISTEN: The socket is a server and is waiting for CONNECT_REQ
 *
 * The status of a #GdkSocket.
 * TODO: write more about how status changes.
 */
typedef enum { /*< prefix = GDK_SOCKET >*/
	GDK_SOCKET_DISCONNECTED,
	GDK_SOCKET_CONNECTED,
	GDK_SOCKET_LISTEN,
	GDK_SOCKET_STATUS_MAX
} GdkSocketStatus;

/**
 * GdkSocket:
 * 	@name:	name of the socket.
 * 	@window: the #GdkWindow used to receive messages. Its #GdkWindow::title will be @name.
 * 	@display: the #GdkDisplay this socket belongs to. Though we can always obtain 
 * 		this information from @window, we cache it here for saving code lines.
 * 	@status: the status. See #GdkSocketStatus.
 *  @target: to whom this socket is connected
 *  @queue: message buffer, 
 *	@acks: number of ACKs received. (and without send a DATA) if @acks > 0, 
 *			it is ok to directly send message to the server without push it to the queue
 *			and wait for a ACK.
 *	       if we send message to the server decrease @acks by 1.
 *  @timeout: number of seconds for a connection to timeout.
 *  @alives: number of replied ISALIVE messages.
 *  
 *  The GdkSocket object.
 */
struct _GdkSocket {
	GObject parent;
/*< public >*/
	gchar * name;
	GdkWindow * window;
	GdkDisplay * display;
	GdkSocketStatus status;
	GdkSocketNativeID target;
	GQueue * queue;
	gint acks;
	gint timeout;
	gint alives;
};

/**
 * GdkSocketClass:
 *   @data_arrival_signal_id: the signal id for ::data-arrival.
 *   @data_arrival_cleanup: the cleanup call back for ::data-arrival signal
 */
struct _GdkSocketClass {
	GObjectClass parent;
/* < private >*/
	void (*data_arrival) (GdkSocket * self, gpointer data, guint length);
	void (*connect_req) (GdkSocket * self, GdkSocketNativeID target);
	void (*shutdown) (GdkSocket * self);
	void (*connected) (GdkSocket * self, GdkSocketNativeID target);
};


GType gdk_socket_get_type (void);

GdkSocket * gdk_socket_new (gchar * name);
GdkSocketNativeID gdk_socket_get_native(GdkSocket * self);

gboolean gdk_socket_listen(GdkSocket * self);
GdkSocket * gdk_socket_accept(GdkSocket * self, GdkSocketNativeID target);

gboolean gdk_socket_send(GdkSocket * self, gpointer data, guint bytes);

void gdk_socket_shutdown(GdkSocket * self);
//gboolean gdk_socket_broadcast_by_name(GdkSocket * self, gchar * name, gpointer data, guint bytes);



G_END_DECLS
#endif

