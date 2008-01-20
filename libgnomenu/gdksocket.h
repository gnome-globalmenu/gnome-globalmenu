
#ifndef GDK_SOCKET_H
#define GDK_SOCKET_H


#define GDK_TYPE_SOCKET 	(gdk_socket_get_type())
#define GDK_SOCKET(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GDK_TYPE_SOCKET, GdkSocket))
#define GDK_SOCKET_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GDK_TYPE_SOCKET, GdkSocketClass))
#define GDK_IS_SOCKET(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDK_TYPE_SOCKET))
#define GDK_IS_SOCKET_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GDK_TYPE_SOCKET))
#define GDK_SOCKET_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GDK_TYPE_SOCKET, GdkSocketClass))

typedef struct _GdkSocket GdkSocket;
typedef struct _GdkSocketClass GdkSocketClass;

typedef enum _GdkSocketStatus {
	GDK_SOCKET_NEW,
	GDK_SOCKET_SHUTDOWN,
	GDK_SOCKET_DISPOSED,
} GdkSocketStatus;

struct _GdkSocket {
	GObject parent;
	gchar * name;
	GdkWindow * window;
	GdkDisplay * display;
	GdkSocketStatus status;
};

struct _GdkSocketClass {
	GObjectClass parent;
	guint data_arrival_signal_id;
	void (*data_arrival_cleanup) (GdkSocket * self, gpointer data, guint length);
};
GType gdk_socket_get_type (void);
GdkSocket * gdk_socket_new (gchar * name);
gboolean gdk_socket_send(GdkSocket * self, 
	GdkNativeWindow target, 
	gpointer data, 
	guint bytes);
GdkNativeWindow gdk_socket_get_native(GdkSocket * self);
#define GDK_SOCKET_ATOM_STRING "GDK_SOCKET_MESSAGE"
#endif

