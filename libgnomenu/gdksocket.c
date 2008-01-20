#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <string.h>

#include "gdksocket.h"
#include "gdksocket-marshall.h"

#define LOG_FUNC_NAME g_message(__func__)

#define GDK_SOCKET_ATOM_STRING "GDK_SOCKET_MESSAGE"

typedef struct _GdkSocketPrivate GdkSocketPrivate;

struct _GdkSocketPrivate {
	int foo;
};

#define GDK_SOCKET_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GDK_TYPE_SOCKET, GdkSocketPrivate))

static GdkFilterReturn 
	gdk_socket_window_filter_cb			(GdkXEvent* xevent, GdkEvent * event, gpointer data);
static void 
	cleanup_data_arrival_signal_handler	(GdkSocket * socket, gpointer data, guint size);

G_DEFINE_TYPE (GdkSocket, gdk_socket, G_TYPE_OBJECT)

static void
gdk_socket_init(GdkSocket * self){
	LOG_FUNC_NAME;
}

/**
 * gdk_socket_new:
 * @name: the name of the newly created socket, do not need to be unique.
 *
 * Returns: a newly created the #GdkSocket in #GDK_SOCKET_NEW state.
 */
GdkSocket *
gdk_socket_new(char * name){
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;
	GdkSocket * socket = NULL;

	LOG_FUNC_NAME;
	socket = g_object_new(GDK_TYPE_SOCKET, NULL);

	socket->name = g_strdup(name);
	socket->display = gdk_display_get_default();

	attr.title = name;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;
	mask = GDK_WA_TITLE;
	
	socket->window = gdk_window_new(NULL, &attr, mask);
	socket->status = GDK_SOCKET_NEW;
	gdk_window_add_filter(socket->window, gdk_socket_window_filter_cb, socket);
	return socket;
}
/**
 * gdk_socket_dispose:
 * 	@object: The #GdkSocket to be disposed.
 *
 *  Should release all of the ref counts and set a disposed state of the object
 *  TODO: unref all the resources and set state to #GDK_SOCKET_DISPOSED.
 */
static void
gdk_socket_dispose(GObject * object){
	GdkSocket * self = GDK_SOCKET(object);
	LOG_FUNC_NAME;
	G_OBJECT_CLASS(gdk_socket_parent_class)->dispose(object);
}
/**
 * gdk_socket_finalize:
 * @object: the #GdkSocket to be finalized.
 *
 *  free all the resoursed occupied by @object
 **/
static void
gdk_socket_finalize(GObject * object){
	GdkSocket * self = GDK_SOCKET(object);
	LOG_FUNC_NAME;
	g_free(self->name);
	G_OBJECT_CLASS(gdk_socket_parent_class)->finalize(object);
}
/**
 * gdk_socket_class_init:
 *
 * Initialize the class structure of #GdkSocket
 */
static void
gdk_socket_class_init(GdkSocketClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	g_type_class_add_private(gobject_class, sizeof (GdkSocketPrivate));

	gobject_class->dispose = gdk_socket_dispose;
	gobject_class->finalize = gdk_socket_finalize;

	klass->data_arrival_cleanup = cleanup_data_arrival_signal_handler;
	klass->data_arrival_signal_id = 
/**
 * GdkSocket::data-arrival:
 * @self: the #GdkSocket that receives this signal.
 * @data: the received data. It is owned by @self and the signal handler 
 * 		should not free it.
 * @bytes: the length of received data.
 *
 * The ::data-arrival signal is emitted each time a message arrives to
 * the socket.
 */
		g_signal_new ("data-arrival",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GdkSocketClass, data_arrival_cleanup),
			NULL /* accumulator */,
			NULL /* accu_data */,
			gdk_socket_marshall_VOID__POINTER_UINT,
			G_TYPE_NONE /* return_type */,
			2     /* n_params */,
			G_TYPE_POINTER,
			G_TYPE_UINT
			);
}

/**
 * gdk_socket_get_native:
 * 	@self: the #GdkSocket this method acts on.
 *
 * Returns: the native id of this socket. It happens to be the Window ID of the
 * 	@GdkWindow the GdkSocket wraps, in current implement.
 */
GdkNativeWindow gdk_socket_get_native(GdkSocket * self){
	return GDK_WINDOW_XWINDOW(self->window);
}

/**
 * gdk_socket_send:
 * @self: the #GdkSocket this method acts on.
 * @target: the native window id of target socket. In current implement, this
 * 		identifies the native window id of #GdkSocket::window.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * This method of #GdkSocket sends a messange to the target socket whose id 
 * is @target. Whether or not the target is a GdkSocket is not checked
 * (and impossible to check). The length of data has is limited by XClientMessage
 * to be 20 bytes. (or even fewer if in the future the socket adds a header to the
 * message.
 *
 * Returns: if successful, return TRUE else return FALSE.
 */
gboolean gdk_socket_send(GdkSocket * self, GdkNativeWindow target, gpointer data, guint bytes){
#ifndef GDK_WINDOWING_X11
#error ONLY X11 is supported. other targets are not yet supported.
#endif
	LOG_FUNC_NAME;
    XClientMessageEvent xclient;
	if(bytes > 20){
		g_error("GdkSocket: Can not send more than 20 bytes");
		return FALSE; /*X can not send more information*/
	}

    memset (&xclient, 0, sizeof (xclient));
    xclient.window = target; /*Though X11 places no interpretation of this field, GDK interpretes this field at the target window.*/
    xclient.type = ClientMessage;
    xclient.message_type = gdk_x11_get_xatom_by_name_for_display (self->display, GDK_SOCKET_ATOM_STRING);
    xclient.format = 32;
	memcpy(&xclient.data.l, data, bytes);
    gdk_error_trap_push ();
    XSendEvent (GDK_DISPLAY_XDISPLAY(self->display),
          target,
          False, NoEventMask, (XEvent *)&xclient);
    gdk_display_sync (self->display); /*resolve the message*/
    return gdk_error_trap_pop () == 0;
}

static GdkFilterReturn 
	gdk_socket_window_filter_cb(GdkXEvent* gdkxevent, GdkEvent * event, gpointer user_data){
	GdkSocket * self = GDK_SOCKET(user_data);
	LOG_FUNC_NAME;
#ifndef GDK_WINDOWING_X11
#error other targets are not yet supported.
#endif
	XEvent * xevent = gdkxevent;
	if(xevent->type == ClientMessage){
		if(xevent->xclient.message_type ==
            gdk_x11_get_xatom_by_name_for_display(self->display, GDK_SOCKET_ATOM_STRING)){
			guint bytes = 16; /*on x11 we always round off to 16 bytes*/
			gpointer data = g_memdup(xevent->xclient.data.l, bytes); /*Dup the memory, since we don't know whether the xevent is still available when the signal will propagate to the handler. The buffer is freed in the clean-up signal handler, virtual function: data_arrival_cleanup*/
			g_signal_emit(G_OBJECT(self), 
				GDK_SOCKET_GET_CLASS(self)->data_arrival_signal_id,
				0,
				data,
				bytes);
			return GDK_FILTER_REMOVE;
        }
	} 
	return GDK_FILTER_CONTINUE;
}

static void cleanup_data_arrival_signal_handler(GdkSocket * socket,
	gpointer data, guint size){
	g_free(data);
	g_message("GdkSocket(%s)::%s ", socket->name, __func__);
}
