
#ifndef GDK_SOCKET_H
#define GDK_SOCKET_H

G_BEGIN_DECLS

/**
 * SECTION: gdksocket
 * @short_description: Connectionless socket for GTK.
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
 * GdkSocketStatus:
 *   @GDK_SOCKET_NEW: A newly created GdkSocket, the associated GdkWindow is created.
 *   @GDK_SOCKET_SHUTDOWN: The socket was shutdown. LACK DEFINITION.
 *   @GDK_SOCKET_DISPOSED: The socket was disposed. Can not receive or
 *   			send any message
 *
 * The status of a #GdkSocket.
 * TODO: write more about how status changes.
 */
typedef enum {
	GDK_SOCKET_NEW,
	GDK_SOCKET_SHUTDOWN,
	GDK_SOCKET_DISPOSED
} GdkSocketStatus;

/**
 * GdkSocket:
 * 	@name:	name of the socket.
 * 	@window: the #GdkWindow used to receive messages. Its #GdkWindow::title will be @name.
 * 	@display: the #GdkDisplay this socket belongs to. Though we can always obtain 
 * 		this information from @window, we cache it here for saving code lines.
 * 	@status: the status. See #GdkSocketStatus.
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
};

/**
 * GdkSocketClass:
 *   @data_arrival_signal_id: the signal id for ::data-arrival.
 *   @data_arrival_cleanup: the cleanup call back for ::data-arrival signal
 */
struct _GdkSocketClass {
	GObjectClass parent;
/* < private >*/
	guint data_arrival_signal_id;
	void (*data_arrival_cleanup) (GdkSocket * self, gpointer data, guint length);
};

GType gdk_socket_get_type (void);

GdkSocket * gdk_socket_new (gchar * name);

gboolean gdk_socket_send(GdkSocket * self, GdkNativeWindow target, gpointer data, guint bytes);
gboolean gdk_socket_send_by_name(GdkSocket * self, gchar * name, gpointer data, guint bytes);
gboolean gdk_socket_broadcast_by_name(GdkSocket * self, gchar * name, gpointer data, guint bytes);

GdkNativeWindow gdk_socket_get_native(GdkSocket * self);

G_END_DECLS
#endif

