#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <string.h>

#include "gdksocket.h"
#include "gnomenu-marshall.h"

#define LOG_FUNC_NAME g_message(__func__)

#define GDK_SOCKET_ATOM_STRING "GDK_SOCKET_MESSAGE"

enum {
	PROP_0,
	PROP_NAME
};

typedef struct _GdkSocketPrivate GdkSocketPrivate;
struct _GdkSocketPrivate {
	gboolean disposed;
	int foo;
};

#define GDK_SOCKET_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GDK_TYPE_SOCKET, GdkSocketPrivate))

static GdkFilterReturn 
	gdk_socket_window_filter_cb			(GdkXEvent* xevent, GdkEvent * event, gpointer data);
static void 
	cleanup_data_arrival_signal_handler	(GdkSocket * socket, gpointer data, guint size);
static void gdk_socket_set_property
			(GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void gdk_socket_get_property
			(GObject * object, guint property_id, GValue * value, GParamSpec * pspec);

static GObject* gdk_socket_constructor
			(GType type, guint n_construct_properties, GObjectConstructParam *construct_params);

G_DEFINE_TYPE (GdkSocket, gdk_socket, G_TYPE_OBJECT)

static void
gdk_socket_init(GdkSocket * self){
	LOG_FUNC_NAME;
}

/**
 * gdk_socket_new:
 * @name: the name of the newly created socket, do not need to be unique.
 *
 * Creates a new #GdkSocket.
 *
 * Returns: a newly created the #GdkSocket in #GDK_SOCKET_NEW state.
 */
GdkSocket *
gdk_socket_new(char * name){
	GdkSocket * socket = NULL;
	GdkSocketPrivate * priv;

	LOG_FUNC_NAME;
	socket = g_object_new(GDK_TYPE_SOCKET, "name", name, NULL);

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
	GdkSocketPrivate * priv;
	LOG_FUNC_NAME;
	priv = GDK_SOCKET_GET_PRIVATE(self);
	if(! priv->disposed){
		gdk_window_remove_filter(self->window, gdk_socket_window_filter_cb, self);
		g_object_unref(self->window);
		priv->disposed = TRUE;	
	}
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
	GParamSpec * pspec;

	LOG_FUNC_NAME;

	g_type_class_add_private(gobject_class, sizeof (GdkSocketPrivate));

	gobject_class->dispose = gdk_socket_dispose;
	gobject_class->constructor = gdk_socket_constructor;
	gobject_class->finalize = gdk_socket_finalize;
	gobject_class->get_property = gdk_socket_get_property;
	gobject_class->set_property = gdk_socket_set_property;

	klass->data_arrival_cleanup = cleanup_data_arrival_signal_handler;
	klass->data_arrival_signal_id = 
/**
 * GdkSocket::data-arrival:
 * @self: the #GdkSocket that receives this signal.
 * @data: the received data. It is owned by @self and the signal handler 
 * 		should not free it.
 Gdk* @bytes: the length of received data.
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
			gnomenu_marshall_VOID__POINTER_UINT,
			G_TYPE_NONE /* return_type */,
			2     /* n_params */,
			G_TYPE_POINTER,
			G_TYPE_UINT
			);

/**
 * GdkSocket:name:
 *
 * the name of the socket
 */
	g_object_class_install_property (gobject_class, 
			PROP_NAME,
			g_param_spec_string ("name",
						"GdkSocket name prop",
						"Set GdkSocket's name",
						"GdkSocket",
						G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
			
}

/**
 * gdk_socket_get_native:
 * 	@self: the #GdkSocket this method acts on.
 *
 * find out the native id of the socket
 *
 * Returns: the native id of this socket. It happens to be the Window ID of the
 * 	@GdkWindow the GdkSocket wraps, in current implement.
 */
GdkSocketNativeID gdk_socket_get_native(GdkSocket * self){
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
 * SeeAlso: #gdk_socket_send_nosync
 */
gboolean gdk_socket_send(GdkSocket * self, GdkNativeWindow target, gpointer data, guint bytes){

	gboolean rt;
	rt = gdk_socket_send_nosync(self, target, data, bytes);
    gdk_display_sync (self->display); /*resolve the message, sync the state*/
	return rt;
}
/**
 * gdk_socket_send_nosync:
 * @self: the #GdkSocket this method acts on.
 * @target: the native window id of target socket. In current implement, this
 * 		identifies the native window id of #GdkSocket::window.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * This function don't call #gdk_display_sync at the end. See #gdk_socket_send. 
 *
 * Returns: if sucessful, TRUE; else FALSE.
 */
gboolean gdk_socket_send_nosync(GdkSocket * self, GdkNativeWindow target, gpointer data, guint bytes){
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
    return gdk_error_trap_pop () == 0;
}

/**
 * _gdk_socket_find_targets:
 * @self: self
 * @name: the name for the targets
 * 
 * Find every possible @GdkSocket on this display with the name @name.
 *
 * Returns: a GList contains the list of those sockets' native ID. 
 * It is the caller's obligation to free the list.
 */
static GList * _gdk_socket_find_targets(GdkSocket * self, gchar * name){
	GList * window_list = NULL;
	GdkScreen * screen;
	GdkWindow * root_window;

    Window root_return;
    Window parent_return;
    Window * children_return;
    unsigned int nchildren_return;
    unsigned int i;

	screen = gdk_drawable_get_screen(self->window);
	g_return_val_if_fail(screen != NULL, NULL);

	root_window = gdk_screen_get_root_window(screen);
	g_return_val_if_fail(root_window != NULL, NULL);

    gdk_error_trap_push();
    XQueryTree(GDK_DISPLAY_XDISPLAY(self->display),
        GDK_WINDOW_XWINDOW(root_window),
        &root_return,
        &parent_return,
        &children_return,
        &nchildren_return);
    if(gdk_error_trap_pop()){
        g_warning("%s: XQueryTree Failed", __func__);
        return NULL;
    }
	g_return_val_if_fail(nchildren_return != 0, NULL);
		
	for(i = 0; i < nchildren_return; i++){
        Atom type_return;
        Atom type_req = gdk_x11_get_xatom_by_name_for_display (self->display, "UTF8_STRING");
        gint format_return;
        gulong nitems_return;
        gulong bytes_after_return;
        guchar * wm_name;
        gint rt;
        gdk_error_trap_push();
        rt = XGetWindowProperty (GDK_DISPLAY_XDISPLAY (self->display), children_return[i],
                          gdk_x11_get_xatom_by_name_for_display (self->display, "_NET_WM_NAME"),
                          0, G_MAXLONG, False, type_req, &type_return,
                          &format_return, &nitems_return, &bytes_after_return,
                          &wm_name);
		if(!gdk_error_trap_pop()){
			if(rt == Success && type_return == type_req){
			if(g_str_equal(name, wm_name)){
				window_list = g_list_append(window_list, (gpointer) children_return[i]);
			}
		}
		}else{
			g_warning("%s:XGetWindowProperty Failed",__func__);
		}
	}
	XFree(children_return);
	g_message("%s:length=%d", __func__, g_list_length(window_list));
	return window_list;
}

/**
 * gdk_socket_send_by_name:
 * @self: you understand.
 * @name: the target socket's name.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * this method find out the all the #GdkSocket with name @name and calls 
 * #gdk_socket_send_nosync to the * first (successfully sended) target
 *
 * Returns:  TRUE if the message is successfully sent to a target.
 * 	FALSE if the message is not sent to anywhere.
 */
gboolean 
	gdk_socket_send_by_name(GdkSocket * self, gchar * name, gpointer data, guint bytes){
	GList * window_list = _gdk_socket_find_targets(self, name);
	GList * node;
	gboolean rt = FALSE;
	for(node = g_list_first(window_list); node; node = g_list_next(node)){
		if(gdk_socket_send_nosync(self, (GdkNativeWindow)node->data, data, bytes)){
			rt = TRUE;
			break;
		}
	}
    gdk_display_sync (self->display); /*resolve the message, sync the state*/
	g_list_free(window_list);
	return rt;
}

/**
 * gdk_socket_broadcast_by_name:
 * @self: you understand.
 * @name: the target socket's name.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * this method find out the all the #GdkSocket with name @name and calls #gdk_socket_send_nosync
 * on first target.
 *
 * Returns:  TRUE if the message is successfully sent to at least one target.
 * 	FALSE if the message is not sent to anywhere.
 *
 * FIXME: 
 *    seems won't work properly if I don't trace the g_message. 
 *    Messages are sent but except for the first socket, #gdk_socket_window_filter 
 *    don't receive them. Perhaps this means we need to get rid of the GdkWindow.
 *
 * TODO:
 *    Figure out why. 
 */
gboolean
gdk_socket_broadcast_by_name(GdkSocket * self, gchar * name, gpointer data, guint bytes){
	GList * window_list = _gdk_socket_find_targets(self, name);
	GList * node;
	gboolean rt = FALSE;
	gboolean rt1;
	for(node = g_list_first(window_list); node; node = g_list_next(node)){
		rt1 =  gdk_socket_send_nosync(self, (GdkNativeWindow)node->data, data, bytes);
		g_message("%s, sent to one target, %d", __func__, rt1);
		rt = rt || rt1;
	}
    gdk_display_sync (self->display); /*resolve the message, sync the state*/
	g_list_free(window_list);
	return rt;
}

/**
 * gdk_socket_window_filter_cb
 *
 * internal filter function related to this X11 implement of #GdkSocket
 */
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

static void 
gdk_socket_get_property( GObject * object, guint property_id, GValue * value, GParamSpec * pspec){
	GdkSocket * self = GDK_SOCKET(object);
	switch (property_id){
		case PROP_NAME:
			g_value_set_string(value, self->name);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	}
}

static void 
gdk_socket_set_property( GObject * object, guint property_id, GValue * value, GParamSpec * pspec){
	GdkSocket * self = GDK_SOCKET(object);
	switch (property_id){
		case PROP_NAME:
			g_free(self->name);
			self->name = g_value_dup_string(value);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	}
}


static GObject* gdk_socket_constructor(GType type, guint n_construct_properties,
		GObjectConstructParam *construct_params){
	GObject *obj;
	GdkSocket *socket;
	GdkSocketPrivate * priv;
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;
	gchar *name;

	LOG_FUNC_NAME;

	obj = ( *G_OBJECT_CLASS(gdk_socket_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);
	socket = GDK_SOCKET(obj);

	priv = GDK_SOCKET_GET_PRIVATE(socket);

	socket->display = gdk_display_get_default();
	attr.title = socket->name;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;
	mask = GDK_WA_TITLE;
	
	socket->window = gdk_window_new(NULL, &attr, mask);
	socket->status = GDK_SOCKET_NEW;
	gdk_window_add_filter(socket->window, gdk_socket_window_filter_cb, socket);
	priv->disposed = FALSE;

	return obj;
}
