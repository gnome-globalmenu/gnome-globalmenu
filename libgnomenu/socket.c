#include <config.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <string.h>

#include "socket.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"

#ifndef GDK_WINDOWING_X11
#error ONLY X11 is supported. other targets are not yet supported.
#endif

#define GNOMENU_SOCKET_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_SOCKET, GnomenuSocketPrivate))

#define GET_OBJECT(_s, s, p) \
	GnomenuSocket * s = GNOMENU_SOCKET(_s); \
	GnomenuSocketPrivate * p = GNOMENU_SOCKET_GET_PRIVATE(_s); \

#define GET_OBJECT_LOG(_s, s, p) \
	GET_OBJECT(_s, s, p) \
	LOG("<%s>", s->name);

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("<GnomenuSocket>::%s:" fmt, __func__, ## args)
#else
#define LOG(fmt, args...)
#endif 

#define g_queue_for(queue, ele, job) \
		{ GList * _list = (queue)?g_queue_peek_head_link(queue):NULL;\
		  GList * _node; \
		  for(_node = g_list_first(_list); _node; _node=g_list_next(_node)){ \
			ele = _node->data; \
		  	job; \
		  } \
		}
#define _GNOMENU_DATA_BUFFER gdk_atom_intern("_GNOMENU_SENT_DATA", FALSE) /*For p2p send*/
#define _GNOMENU_BC_BUFFER gdk_atom_intern("_GNOMENU_RECV_DATA", FALSE) /*For receiving broadcasting */
#define _GNOMENU_MESSAGE_TYPE gdk_atom_intern("_GNOMENU_MESSAGE_TYPE", FALSE) /*message*/
/* Properties */
enum {
	PROP_0,
	PROP_NAME,
	PROP_TARGET,
	PROP_STATUS,
	PROP_TIMEOUT
};
/* Signals */
enum {
	DATA_ARRIVAL,
	CONNECT_REQ,
	CONNECTED,
	SHUTDOWN,
	SIGNAL_MAX,
};

typedef struct _GnomenuSocketPrivate GnomenuSocketPrivate;
struct _GnomenuSocketPrivate {
	gboolean disposed;
	guint time_source;
	GQueue * data_queue;
	GdkDisplay * display;
	GdkWindow * window;
	GnomenuSocketNativeID target;
	gint acks;
	gboolean destroy_on_shutdown;
	guint16 seq;
};

/**
 * MessageType:
 * @GNOMENU_SOCKET_BROADCAST: a broadcast (connectionless) mesage;
 * @GNOMENU_SOCKET_CONNECT_REQ: a connect request;
 * @GNOMENU_SOCKET_ACK: ready to accept a new data;
 * @GNOMENU_SOCKET_DATA: send data;
 * @GNOMENU_SOCKET_SHUTDOWN: peer closed, clean up your stuff;
 * @GNOMENU_SOCKET_ISALIVE: are you alive?
 * @GNOMENU_SOCKET_ALIVE: yes i am.
 * @GNOMENU_SOCKET_PING: are you a socket?  NOT DONE
 * @GNOMENU_SOCKET_ECHO: yes I am. NOT DONE
 **/
typedef enum {
	MSG_BROADCAST = 0,
	MSG_CONNECT_REQ = 1,
	MSG_CONNECT_ACK = 2,
	MSG_ACK = 4 ,
	MSG_DATA = 5,
	MSG_SHUTDOWN = 6,
	MSG_ISALIVE = 7,
	MSG_ALIVE = 8 ,
	MSG_PING = 9,
	MSG_ECHO = 10
} MessageType;

/**
 * MessageHeader:
 *	@type: type of the header
 *	@bytes: length of the data(the size of the header is excluded!
 *	@source: source socket of this msg
 *
 *  This struct is passed via XClientMessage.  Limit it to less than 20 bytes.
 *  On 64bit platform sizeof(GnomenuSocketNativeID) == 8
 */
typedef struct {
	GnomenuSocketNativeID source;
	union {
	guint8 bytes;	
	GnomenuSocketNativeID service;
	guint32 version;
	};
	/*MessageType */ guint8 type;
} MessageHeader;

/**
 * DataMessage:
 * 	@header: header of the message, initialized.
 * 	@content: a copy of data buffer of the message content.
 *
 *	When a ::send method is invoked, a message header is intialized, and the data content is
 *	copied to data; This data is send to the receiver when a ACK message is received.
 */
typedef struct {
	MessageHeader header;
	guint16 seq;
	gchar data[];
} DataMessage;

/* GObject interface */
static GObject * _constructor 	( GType type, 
								  guint n_construct_properties, 
								  GObjectConstructParam *construct_params );
static void _dispose 			( GObject * object );
static void _finalize			( GObject * object );
static void _set_property 		( GObject * object, 
								  guint property_id, const GValue * value, GParamSpec * pspec );
static void _get_property 		( GObject * object, 
								  guint property_id, GValue * value, GParamSpec * pspec );

/* Default signal closures */
static void _c_data 		( GnomenuSocket * socket, gpointer data, guint size );
static void _c_request 		( GnomenuSocket * socket, GnomenuSocketNativeID target );
static void _c_connected 			( GnomenuSocket * socket, GnomenuSocketNativeID target );
static void _c_shutdown 			( GnomenuSocket * socket );

/* helper functions  */
static gboolean _send_xclient_message 	( GdkNativeWindow target, gpointer data, guint bytes );
static gboolean _peek_xwindow 			( GdkNativeWindow target );
static GList * _find_native_by_name		( gchar * name );

static gboolean _set_native_buffer ( GnomenuSocketNativeID native, GdkAtom buffer, gpointer data, gint bytes);
static gpointer _get_native_buffer ( GnomenuSocketNativeID native, GdkAtom buffer, gint * bytes, gboolean remove);

static GdkFilterReturn 
_window_filter_cb				(GdkXEvent* xevent, GdkEvent * event, gpointer data);

/* implementations */
gboolean _real_connect (GnomenuSocket * socket, GnomenuSocketNativeID target);
gboolean _real_accept (GnomenuSocket * socket, GnomenuSocket * service, GnomenuSocketNativeID target);
gboolean _real_send (GnomenuSocket * socket, gpointer data, guint bytes);
gboolean _real_flush (GnomenuSocket * socket);
gboolean _real_listen (GnomenuSocket * socket);

gboolean _real_broadcast (GnomenuSocket * socket, gpointer data, guint bytes);
void _real_shutdown (GnomenuSocket * socket);	

static gboolean _test_connection		( GnomenuSocket * socket );

static gulong 
class_signals[SIGNAL_MAX] 		= {0};

G_DEFINE_TYPE 					(GnomenuSocket, gnomenu_socket, G_TYPE_OBJECT)
#define TYPE_DATA_MESSAGE (_data_message_get_type())
static DataMessage * _data_message_copy(DataMessage * src){
	return g_memdup(src, sizeof(DataMessage) + src->header.bytes);
}
static GType _data_message_get_type(){
	static GType type = 0;
	if(G_UNLIKELY(type ==0)){
		type = g_boxed_type_register_static("GnomenuSocketDataMessage",
				(GBoxedCopyFunc) _data_message_copy,
				g_free);
	}
}
/**
 * gnomenu_socket_class_init:
 *
 * Initialize the class structure of #GnomenuSocket
 */
static void
gnomenu_socket_class_init(GnomenuSocketClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec * pspec;

	g_type_class_add_private(gobject_class, sizeof (GnomenuSocketPrivate));

	gobject_class->dispose = _dispose;
	gobject_class->constructor = _constructor;
	gobject_class->finalize = _finalize;
	gobject_class->get_property = _get_property;
	gobject_class->set_property = _set_property;

	klass->c_data = _c_data;
	klass->c_request = _c_request;
	klass->c_connected = _c_connected;
	klass->c_shutdown = _c_shutdown;

	klass->connect = _real_connect;
	klass->listen = _real_listen;
	klass->accept = _real_accept;
	klass->send = _real_send;
	klass->broadcast = _real_broadcast;
	klass->shutdown = _real_shutdown;
	klass->flush = _real_flush;

	class_signals[DATA_ARRIVAL] =
/**
 * GnomenuSocket::data:
 * @self: the #GnomenuSocket that receives this signal.
 * @data: the received data. It is owned by @self and the signal handler 
 * 		should not free it.  
 * @bytes: the length of received data.
 *
 * The ::data-arrival signal is emitted each time a message arrives to
 * the socket. the detail can be ::peer and ::broadcast.
 *
 * ::peer stands for data arrives from the other peer of the connection.
 * ::broadcast stands for data arrives from a broadcasting source.
 */
		g_signal_new ("data",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_DETAILED | G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuSocketClass, c_data),
			NULL /* accumulator */,
			NULL /* accu_data */,
			gnomenu_marshall_VOID__POINTER_UINT,
			G_TYPE_NONE /* return_type */,
			2     /* n_params */,
			G_TYPE_POINTER,
			G_TYPE_UINT
			);

	class_signals[CONNECT_REQ] =
/**
 * GnomenuSocket::request:
 * @self: the #GnomenuSocket that receives this signal.
 *
 * The ::request signal is emitted when a connection request arrives.
 */
		g_signal_new ("request",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuSocketClass, c_request),
			NULL /* accumulator */,
			NULL /* accu_data */,
			gnomenu_marshall_VOID__UINT,
			G_TYPE_NONE /* return_type */,
			1     /* n_params */,
			G_TYPE_UINT
			);
	class_signals[CONNECTED] =
/**
 * GnomenuSocket::connected:
 * @self: the #GnomenuSocket that receives this signal.
 * @target: the other peer which this socket is connect to.
 *
 * The ::connected signal is emitted on both sides
 * when the connection request is
 * resolved and the connection is created.
 */
		g_signal_new ("connected",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuSocketClass, c_connected),
			NULL /* accumulator */,
			NULL /* accu_data */,
			gnomenu_marshall_VOID__UINT,
			G_TYPE_NONE /* return_type */,
			1     /* n_params */,
			G_TYPE_UINT
			);

	class_signals[SHUTDOWN] =
/**
 * GnomenuSocket::shutdown
 * @self: itself
 * 
 * invoked when the socket is shut down. either from this peer or
 * the other peer. the default handler will cleanup the message queue.
 */
		g_signal_new ("shutdown",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuSocketClass, c_shutdown),
			NULL,
			NULL,
			gnomenu_marshall_VOID__VOID,
			G_TYPE_NONE,
			0);

/**
 * GnomenuSocket:name:
 *
 * the name of the socket
 */
	g_object_class_install_property (gobject_class, 
			PROP_NAME,
			g_param_spec_string ("name",
						"GnomenuSocket name prop",
						"Set GnomenuSocket's name",
						"GnomenuSocket",
						G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
			
/**
 * GnomenuSocket::status
 *
 * the status of the socket. #GnomenuSocketNativeID
 */
	g_object_class_install_property (gobject_class, 
			PROP_STATUS,
			g_param_spec_enum ("status",
						"GnomenuSocket status prop",
						"Set GnomenuSocket's status",
						gnomenu_socket_status_get_type(),
						GNOMENU_SOCKET_DISCONNECTED,
						G_PARAM_READABLE));
/**
 * GnomenuSocket::timeout
 *
 * number of idle seconds before the socket try to detect if the connection is lost.
 */
	g_object_class_install_property (gobject_class,
			PROP_TIMEOUT,
			g_param_spec_int ("timeout",
						"GnomenuSocket timeout prop",
						"Set GnomenuSocket's timeout",
						0, 2000, 30,
						G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
}

static void
gnomenu_socket_init (GnomenuSocket * socket){
	GET_OBJECT(socket, self, priv);

	GdkWindowAttr attr;
	GdkWindowAttributesType mask;

	priv->display = gdk_display_get_default();
	priv->time_source = 0;
	priv->disposed = FALSE;
	priv->data_queue = g_queue_new();
	priv->target = 0;
	priv->acks = 0;
	priv->seq = 0;
	self->timeout = 10000;
	self->name = g_strdup("Anonymous Socket");
	priv->destroy_on_shutdown = FALSE;
	attr.title = self->name;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;
	mask = GDK_WA_TITLE;

	priv->window = gdk_window_new(NULL, &attr, mask);
}
static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	static const char string[] = "GnomenuSocket";
	GObject * object = ( *G_OBJECT_CLASS(gnomenu_socket_parent_class)->constructor)
						( type,
						  n_construct_properties,
						  construct_params);
	GET_OBJECT(object, self, priv);
	_set_native_buffer(GDK_WINDOW_XID(priv->window), _GNOMENU_MESSAGE_TYPE, string, sizeof(string));
	gdk_window_add_filter(/*priv->window*/NULL, _window_filter_cb, self);

	return object;
}

/**
 * gnomenu_socket_new:
 * @name: the name of the newly created socket, do not need to be unique.
 *
 * Creates a new #GnomenuSocket.
 *
 * Returns: a newly created the #GnomenuSocket in #GNOMENU_SOCKET_NEW state.
 */
GnomenuSocket *
gnomenu_socket_new (char * name, gint timeout){
	return g_object_new(GNOMENU_TYPE_SOCKET, "name", name,
				"timeout", timeout, NULL);
}

static void 
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec){
	GET_OBJECT(object, self, priv);
	switch (property_id){
		case PROP_NAME:
			g_free(self->name);
			self->name = g_value_dup_string(value);
			gdk_window_set_title(priv->window, self->name);
		break;
		case PROP_TIMEOUT:
			self->timeout = g_value_get_int(value);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}
}
static void 
_get_property( GObject * object, guint property_id, GValue * value, GParamSpec * pspec){
	GET_OBJECT(object, self, priv);
	switch (property_id){
		case PROP_NAME:
			g_value_set_string(value, self->name);
		break;
		case PROP_STATUS:
			g_value_set_enum(value, self->status);
		break;
		case PROP_TIMEOUT:
			g_value_set_int(value, self->timeout);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}
}
static void
_dispose (GObject * object){
	GET_OBJECT(object, self, priv);
	if(!priv->disposed){
		gdk_window_remove_filter(/*priv->window*/ NULL, _window_filter_cb, self);
		if(priv->time_source)
			g_source_remove(priv->time_source);
		priv->disposed = TRUE;	
		g_queue_for(priv->data_queue, DataMessage * ele, g_free(ele));
		g_queue_clear(priv->data_queue);
	}
	G_OBJECT_CLASS(gnomenu_socket_parent_class)->dispose(object);
}

static void
_finalize(GObject * object){
	GET_OBJECT(object, self, priv);
	GList * list, * node;
	gdk_window_destroy(priv->window);
	g_free(self->name);
	g_queue_free(priv->data_queue);
	priv->data_queue = NULL;
	G_OBJECT_CLASS(gnomenu_socket_parent_class)->finalize(object);
}
/* helper functions */
static gboolean _send_xclient_message 	( GdkNativeWindow target, gpointer data, guint bytes ){
	gboolean rt;
    XClientMessageEvent xclient;
	if(bytes > 20){
		g_error("GnomenuSocket: Can not send raw data for more than 20 bytes");
		return FALSE; /*X can not send more information*/
	}

    memset (&xclient, 0, sizeof (xclient));
    xclient.window = target; /*Though X11 places no interpretation of this field, GNOMENU interpretes this field at the target window.*/
    xclient.type = ClientMessage;
    xclient.message_type = gdk_x11_atom_to_xatom(_GNOMENU_MESSAGE_TYPE);
    xclient.format = 8;
	memcpy(&xclient.data.l, data, bytes);
    gdk_error_trap_push ();
    XSendEvent (GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
          target,
          False, NoEventMask, (XEvent *)&xclient);
	gdk_flush();
    gdk_display_sync (gdk_display_get_default()); /*resolve the message, sync the state*/
    return gdk_error_trap_pop () == 0;

}
static gboolean _peek_xwindow 			( GdkNativeWindow target ) {
	GdkNativeWindow root_return;
	GdkNativeWindow parent_return;
	GdkNativeWindow * children_return;
	guint 	nchildren_return;
	gdk_error_trap_push();
	XQueryTree(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
			target, 
			&root_return, &parent_return, 
			&children_return, &nchildren_return);
	if(gdk_error_trap_pop()) return FALSE;
	else {
		XFree(children_return);
	}
	return TRUE;
}
/**
 * _find_native_by_name:
 * 	
 * If name == NULL, return all windows.
 */
static GList * _find_native_by_name		( gchar * name ){
	GList * window_list = NULL;
	GdkScreen * screen;
	GdkWindow * root_window;

    Window root_return;
    Window parent_return;
    Window * children_return;
    unsigned int nchildren_return;
    unsigned int i;
	gchar * gnomenu_type;
	screen = gdk_screen_get_default();
	g_return_val_if_fail(screen, NULL);

	root_window = gdk_screen_get_root_window(screen);
	g_return_val_if_fail(root_window, NULL);

    gdk_error_trap_push();
    XQueryTree(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
        GDK_WINDOW_XWINDOW(root_window),
        &root_return,
        &parent_return,
        &children_return,
        &nchildren_return);
	gdk_flush();
    if(gdk_error_trap_pop()){
        g_warning("%s: XQueryTree Failed", __func__);
        return NULL;
    }
	g_return_val_if_fail(nchildren_return != 0, NULL);
		
	for(i = 0; i < nchildren_return; i++){
        Atom type_return;
        Atom type_req = gdk_x11_atom_to_xatom (gdk_atom_intern("UTF8_STRING", FALSE));
        gint format_return;
        gulong nitems_return;
        gulong bytes_after_return;
        guchar * wm_name;
        gint rt;
        gdk_error_trap_push();
        rt = XGetWindowProperty (GDK_DISPLAY_XDISPLAY (gdk_display_get_default()), children_return[i],
                          gdk_x11_atom_to_xatom (gdk_atom_intern("_NET_WM_NAME", FALSE)),
                          0, G_MAXLONG, False, type_req, &type_return,
                          &format_return, &nitems_return, &bytes_after_return,
                          &wm_name);
		gdk_flush();
		if(!gdk_error_trap_pop()){
			if(rt == Success && type_return == type_req){
			gint bytes;
			gnomenu_type = _get_native_buffer(children_return[i], _GNOMENU_MESSAGE_TYPE, &bytes, FALSE);
			if(gnomenu_type && (!name || g_str_equal(name, wm_name))){
				g_free(gnomenu_type);
				window_list = g_list_append(window_list, (gpointer) children_return[i]);
			}
		}
		}else{
			g_warning("%s:XGetWindowProperty Failed",__func__);
		}
	}
	XFree(children_return);
	return window_list;

}

static gboolean _set_native_buffer ( GnomenuSocketNativeID native, GdkAtom buffer, gpointer data, gint bytes){
	gdk_error_trap_push();	
	XChangeProperty(
			GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), 
			native,
			gdk_x11_atom_to_xatom(buffer), 
			gdk_x11_atom_to_xatom(_GNOMENU_MESSAGE_TYPE), 
			8, 
			PropModeReplace, 
			data, bytes);
	if(gdk_error_trap_pop()){
		return FALSE;
	}
	return TRUE;
}
static gpointer _get_native_buffer ( GnomenuSocketNativeID native, GdkAtom buffer, gint * bytes, gboolean remove){
	Atom actual_type_return;
	gulong actual_format_return;
	gulong bytes_after_return;
	gulong nitems_return;
	gchar * property_return;	

	gdk_error_trap_push();
	XGetWindowProperty(
			GDK_DISPLAY_XDISPLAY(gdk_display_get_default()), 
			native, 
			gdk_x11_atom_to_xatom(buffer), 
/*FIXME: max size is 2048 bytes*/
			0, 2048, 
			remove,
			gdk_x11_atom_to_xatom(_GNOMENU_MESSAGE_TYPE), 
			&actual_type_return,
			&actual_format_return,
			&nitems_return,
			&bytes_after_return,
			&property_return);
	if(gdk_error_trap_pop()){
		return NULL;
	} else {
		*bytes = nitems_return;
		gpointer data = g_memdup(property_return, nitems_return);
		LOG("%10s", data);
		XFree(property_return);		
		return data;	
	}
}
void _monitor_native_buffers(GnomenuSocketNativeID native, gboolean sw) {
		XSelectInput(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
				native, sw?PropertyChangeMask:0);
}
/* public methods */
/**
 * gnomenu_socket_get_native:
 * 	@_self: the #GnomenuSocket this method acts on.
 *
 * find out the native id of the socket
 *
 * Returns: the native id of this socket. It happens to be the Window ID of the
 * 	Window wrapped by #GnomenuSocket.
 */
GnomenuSocketNativeID gnomenu_socket_get_native(GnomenuSocket * socket) {
	GET_OBJECT(socket, self, priv);
	return GDK_WINDOW_XWINDOW(priv->window);
}
/**
 * gnomenu_socket_lookup:
 * 	@name: name of the socket
 *
 * look up for the first socket with the given name.
 */
GnomenuSocketNativeID gnomenu_socket_lookup(gchar * name) {
	GList * list;
	GnomenuSocketNativeID rt = 0;
	list = _find_native_by_name(name);
	if(list) rt = list->data;
	g_list_free(list);
	return rt;
}
/**
 * gnomenu_socket_listen:
 * 	@_self: socket to be listening
 *
 * Set @_self to listening mode. 
 * A GnomenuSocket::connect-req signal is emitted when
 * there is a connection request.
 *
 * Returns: TRUE.
 */
gboolean gnomenu_socket_listen(GnomenuSocket * socket) {
	return GNOMENU_SOCKET_GET_CLASS(socket)->listen(socket);
}
gboolean _real_listen(GnomenuSocket * _self){
	_self->status = GNOMENU_SOCKET_LISTEN;
/* FIXME: should I broadcast some message here or invoke a signal?*/
/* NOTE: don't broadcast any message here. maybe a signal will
 * be useful, but not now.*/
/* FIXME: should I set a flag on the window so that other sockets can
 * test if this socket is listening?*/

	return TRUE;
}
/**
 * gnomenu_socket_send:
 * @_self: self;
 * @data: you can free the data after calling this function
 * @bytes: bytes to send;
 *
 * Send data via a #GnomenuSocket.
 *
 * Returns: TRUE if sent/buffered. FALSE if encounters an error.
 *
 * If current socket is not valid for sending (ACKs too small), data
 * will be pushed into a FIFO queue. 
 * It is impossible to fail a sending in this sense. 
 */
gboolean gnomenu_socket_send(GnomenuSocket * socket, gpointer data, guint bytes) {
	return GNOMENU_SOCKET_GET_CLASS(socket)->send(socket, data, bytes);
}
gboolean _real_send(GnomenuSocket * socket, gpointer data, guint bytes){
	GET_OBJECT_LOG(socket, self, priv);
	DataMessage * msg = g_malloc(sizeof(DataMessage) + bytes);
	gchar buffer[1024];
	gint i;
	gint j;
	if(self->status != GNOMENU_SOCKET_CONNECTED){
		LOG("Not connected, can not send");
		return FALSE;
	}
	g_memmove(msg->data, data, bytes);
	msg->header.type = MSG_DATA;
	msg->header.bytes = bytes;
	msg->header.source = gnomenu_socket_get_native(self);
	msg->seq = priv->seq;
	LOG("status = %s, ACKS=%d, seq = %d", gnomenu_socket_status_get_value(self->status)->value_name, priv->acks, priv->seq);
	priv->seq ++;
	for(i = 0, j=0; i< bytes && j< sizeof(buffer); i++){
		j+=g_sprintf(&buffer[j], "%02hhX ", ((gchar * ) data) [i]);
	}
	LOG("data = %s", buffer);
	g_queue_push_tail(priv->data_queue, msg);
	gnomenu_socket_flush(self);
	LOG("send queue length = %d\n", g_queue_get_length(priv->data_queue));
	return TRUE;
}

/** gnomenu_socket_flush:
 *	@_self: the socket to flush.
 *
 * Flush the sending queue of a socket to catch up with ACKs.
 *
 * Returns:
 * If queue is not long enough, flush everything and return TRUE,
 *
 * If queue is too long, flush GnomenuSocket::acks and return TRUE,
 *
 * If one of the send operation fails, return FALSE.
 * 
 * NOTE: acks <= 1
*/
gboolean gnomenu_socket_flush(GnomenuSocket * socket) {
	return GNOMENU_SOCKET_GET_CLASS(socket)->flush(socket);
}
gboolean _real_flush(GnomenuSocket * socket) {
	GET_OBJECT(socket, self, priv);
	LOG("acks = %d", priv->acks);
	g_assert(priv->acks <=1);
	if(priv->acks == 1 && !g_queue_is_empty(priv->data_queue)){
		DataMessage * data_msg = g_queue_peek_head(priv->data_queue);
		LOG("data size = %d", data_msg->header.bytes);
		priv->acks = 0;
		if(!_set_native_buffer(priv->target, _GNOMENU_DATA_BUFFER, data_msg, data_msg->header.bytes + sizeof(DataMessage))) {
		/*then we send the notify message in PropertyNotify event handler.*/
		LOG("flushing queue failed");
		/*check if the connection is lost.*/
		return FALSE;
		}
	}
	LOG("queue flushed: new length = %d", g_queue_get_length(priv->data_queue));
	return TRUE;
	
}
/**
 * gnomenu_socket_connect:
 * @self: self
 * @target: the native id of the target socket.
 *
 * Connect to a remote socket. Don't check if the target socket is in listening
 * state. To make sure the connection is establish, listen to ::connected signal.
 *
 * If the target socket is listening, a GnomenuSocket::request signal
 * will be emitted on it. 
 *
 * Wait for a CONNECT_ACK to set the state to CONNECTED.
 */
gboolean gnomenu_socket_connect(GnomenuSocket * socket, GnomenuSocketNativeID target){
	return GNOMENU_SOCKET_GET_CLASS(socket)->connect(socket, target);
}

gboolean _real_connect(GnomenuSocket * socket, GnomenuSocketNativeID target){
	GET_OBJECT(socket, self, priv);
	MessageHeader msg;
	if(self->status != GNOMENU_SOCKET_DISCONNECTED){
		g_warning("Can not change to CONNECTED state from other than DISCONNECTED state: %s", 
			gnomenu_socket_status_get_value(self->status)->value_name);
		return FALSE;
	}
	msg.source = gnomenu_socket_get_native(self);	
	msg.type = MSG_CONNECT_REQ;
	msg.version = LIBGNOMENU_VERSION;
	return _send_xclient_message(target, &msg, sizeof(msg));
}
/**
 * gnomenu_socket_accept:
 * 	@_self: self,
 * 	@target: target socket.
 *
 * Accept a connection request.
 *
 * Returns: a new socket to talk with the connected socket.
 */
gboolean
gnomenu_socket_accept (GnomenuSocket * socket, GnomenuSocket * service, GnomenuSocketNativeID target){
	return GNOMENU_SOCKET_GET_CLASS(socket)->accept(socket, service, target);
}

gboolean 
_real_accept (GnomenuSocket * socket, GnomenuSocket * service, GnomenuSocketNativeID target){
	GET_OBJECT(socket, self, priv);
	GnomenuSocketPrivate * s_priv = GNOMENU_SOCKET_GET_PRIVATE(service);
	if(self->status == GNOMENU_SOCKET_LISTEN){
		MessageHeader ack;
		s_priv->target = target;
		service->status = GNOMENU_SOCKET_CONNECTED;
		s_priv->destroy_on_shutdown = TRUE;
		ack.type = MSG_CONNECT_ACK;
		ack.source = gnomenu_socket_get_native(self);
		ack.service = gnomenu_socket_get_native(service);
		_monitor_native_buffers(target, TRUE);
		g_signal_emit(G_OBJECT(service), class_signals[CONNECTED], 0, target);
		return _send_xclient_message(target, &ack, sizeof(ack));
	} else{
		g_error("the socket is not listening, how can you ACCEPT?");
		return NULL;
	}

}

static gboolean _test_connection		( GnomenuSocket * socket ) {
	GET_OBJECT(socket, self, priv);
	if(_peek_xwindow(priv->target))
		return TRUE;
	else {
		priv->time_source = 0;
		gnomenu_socket_shutdown(socket);
		return FALSE;
	}
}

gboolean gnomenu_socket_broadcast(GnomenuSocket * socket, gpointer data, guint bytes){
	return GNOMENU_SOCKET_GET_CLASS(socket)->broadcast(socket, data, bytes);
}
gboolean _real_broadcast(GnomenuSocket * socket, gpointer data, guint bytes){
	GET_OBJECT(socket, self, priv);
	GList * list = _find_native_by_name(NULL);
	GList * node;	
	GnomenuSocketNativeID native;
	DataMessage * data_msg = g_malloc(sizeof(DataMessage) + bytes);
	data_msg->header.source = gnomenu_socket_get_native(self);
	data_msg->header.type = MSG_BROADCAST;
	data_msg->header.bytes = bytes;
	g_memmove(data_msg->data, data, bytes);
	for(node = g_list_first(list); node; node = g_list_next(node)){
		native = node->data;
		_monitor_native_buffers(native, TRUE);
		_set_native_buffer(native, _GNOMENU_BC_BUFFER, data_msg, sizeof(DataMessage) + bytes);
	//	_send_xclient_message (native, &data_msg->header, sizeof(MessageHeader));
	/*above is handled in the filter, when the property is acutally set*/
	}

	g_free(data_msg);
	g_list_free(list);
	return TRUE;
}

/**
 * gnomenu_socket_shutdown:
 *	@_self: the socket to shutdown.
 *
 * Shutdown a socket connection. Note that this doesn't unref the object,
 * unless you have connected GnomenuSocket::shutdown to
 * gnomenu_socket_destroy_on_shutdown();
 */
void gnomenu_socket_shutdown(GnomenuSocket * socket) {
	GNOMENU_SOCKET_GET_CLASS(socket)->shutdown(socket);
}
void _real_shutdown (GnomenuSocket * socket) {
	GET_OBJECT(socket, self, priv);
	MessageHeader msg;
	if(self->status == GNOMENU_SOCKET_CONNECTED){
		self->status = GNOMENU_SOCKET_DISCONNECTED;
		msg.type = MSG_SHUTDOWN;
		msg.source = gnomenu_socket_get_native(socket);
		_send_xclient_message(priv->target, &msg, sizeof(msg));
		/*then tells myself the connection is lost.*/
		g_signal_emit(G_OBJECT(self),
			class_signals[SHUTDOWN], 0);	
	} else{
		g_warning("Not connected, can not shutdown");
	}
}
static GdkFilterReturn 
	_window_filter_cb(GdkXEvent* gdkxevent, GdkEvent * event, gpointer pointer){

	XEvent * xevent = gdkxevent;
	if( xevent->type == PropertyNotify &&
		(xevent->xproperty.atom == gdk_x11_atom_to_xatom(_GNOMENU_DATA_BUFFER))){
		GET_OBJECT(pointer, self, priv);
		if( xevent->xproperty.window == priv->target
			&& xevent->xproperty.state == PropertyNewValue) {
			gint bytes;
			DataMessage * buffer = _get_native_buffer(priv->target,
										_GNOMENU_DATA_BUFFER,
										&bytes, FALSE);
			DataMessage * matched_msg = NULL;
			if(buffer) 
			g_queue_for(priv->data_queue, DataMessage * data_msg,
				if(data_msg->seq == buffer->seq){ /*The right msg*/
					matched_msg = data_msg; break;}
				);
			if(matched_msg) {
				g_queue_remove(priv->data_queue, matched_msg);
				_send_xclient_message(priv->target, &matched_msg->header, sizeof(MessageHeader));
				g_free(matched_msg);
				LOG("data buffer is set, send notify. queue length is %d", g_queue_get_length(priv->data_queue));
				g_free(buffer);
			}
			return GDK_FILTER_CONTINUE;
		}
	}
	if( xevent->type == PropertyNotify &&
		(xevent->xproperty.atom == gdk_x11_atom_to_xatom(_GNOMENU_BC_BUFFER))){
		GET_OBJECT(pointer, self, priv);
		if(xevent->xproperty.state == PropertyNewValue) {
			gint bytes;
			DataMessage * buffer = _get_native_buffer(
										xevent->xproperty.window,
										_GNOMENU_BC_BUFFER,
										&bytes, FALSE);
			if(buffer){
				_send_xclient_message(xevent->xproperty.window, &buffer->header, sizeof(MessageHeader));
				
				_monitor_native_buffers(xevent->xproperty.window, FALSE);
				g_free(buffer);
			}
			return GDK_FILTER_CONTINUE;
		}
	}
	if( xevent->type != ClientMessage ||
		xevent->xclient.message_type !=
		gdk_x11_atom_to_xatom(_GNOMENU_MESSAGE_TYPE)){
		return GDK_FILTER_CONTINUE;
	}
	GET_OBJECT(pointer, self, priv);
	if(xevent->xclient.window != gnomenu_socket_get_native(self)){
		return GDK_FILTER_CONTINUE;
	}
	MessageHeader * msg =(MessageHeader*) xevent->xclient.data.l;
	switch(msg->type){
		case MSG_BROADCAST:
			LOG("MSG_BROADCAST");
			/* obtain data */
			{ gint bytes;
			  DataMessage * buffer = _get_native_buffer(gnomenu_socket_get_native(self),
										_GNOMENU_BC_BUFFER,
										&bytes, FALSE);
			  g_assert(buffer);
			  if(bytes!= msg->bytes + sizeof(DataMessage)){
				g_warning("broadcast message doesn't fit, ignore it");
			  } else {
			/* emit signal ::data::broadcast*/
			  g_signal_emit(G_OBJECT(self), class_signals[DATA_ARRIVAL],
						g_quark_from_string("broadcast"), buffer->data, bytes);
			  }
			}
			return GDK_FILTER_REMOVE;
		case MSG_CONNECT_REQ:
			LOG("MSG_CONNECT_REQ");
			/* if not listening, warn and ignore */
			/* emit signal ::request*/
			if(self->status != GNOMENU_SOCKET_LISTEN){
				LOG("not listening. ");
				break;
			}
			if(msg->version != LIBGNOMENU_VERSION) {
				LOG("wrong version, don't connect! my=%d, client=%d", LIBGNOMENU_VERSION, msg->version);
				break;
			}
			g_signal_emit(G_OBJECT(self), class_signals[CONNECT_REQ],
						0, msg->source);
			return GDK_FILTER_REMOVE;
		case MSG_CONNECT_ACK:
			LOG("MSG_CONNECT_ACK");
			/* set state to GNOMENU_SOCKET_CONNECTED*/
			self->status = GNOMENU_SOCKET_CONNECTED;
			priv->target = msg->service;
			_monitor_native_buffers(msg->service, TRUE);
			{ MessageHeader ack;
			  ack.type = MSG_ACK;
			  ack.source = gnomenu_socket_get_native(self);
			  _send_xclient_message(priv->target, &ack, sizeof(ack));
			}
			g_signal_emit(G_OBJECT(self), class_signals[CONNECTED], 0, priv->target);
			msg->type = MSG_ACK;
			msg->source = msg->service;
	}
	if(msg->source != priv->target){
			g_warning("received a message from a different peer.");
			/* test if the connection is lost */
			return GDK_FILTER_REMOVE;
	} else {
		switch (msg->type){
			case MSG_DATA:
				LOG("MSG_DATA");
				/* Obtain the data, invoke ::data::peer */
				/* send ACK*/
					MessageHeader ack;
					DataMessage * buffer;
					gint bytes;
					ack.type = MSG_ACK;
					ack.source = gnomenu_socket_get_native(self);	
					buffer = _get_native_buffer(gnomenu_socket_get_native(self), 
							_GNOMENU_DATA_BUFFER, 
							&bytes, TRUE);
					_send_xclient_message(priv->target, &ack, sizeof(ack));
					g_assert(buffer);
					g_assert(bytes == msg->bytes + sizeof(DataMessage));
					g_signal_emit(self, class_signals[DATA_ARRIVAL], 
						g_quark_from_string("peer"), buffer->data, msg->bytes);
			break;
			case MSG_ACK:
				LOG("MSG_ACK");
				/* if priv->acks ==1 panic/test connection*/
				/* by definition there should not be more than 1 acks. */
				if(priv->acks >= 1) {
					g_error("protocal fails. priv->ack >=1.(=%d)", priv->acks);
				}
				/* priv->acks++*/
				priv->acks ++;
				/* resolve the ack*/
				gnomenu_socket_flush(self);
			break;
			case MSG_SHUTDOWN:
				LOG("MSG_SHUTDOWN");
				/* if not connected, warn and ignore */
				g_signal_emit(self,
						class_signals[SHUTDOWN],
						0);
			break;
			default:
				g_error("Unhandled GnomenuSocket message type.");
		}
		return GDK_FILTER_REMOVE;
	}
	return GDK_FILTER_CONTINUE;
}

/* Default signal closures */
static void _c_data 		( GnomenuSocket * socket, gpointer data, guint size ) {
	if(data)
	g_free( ((gchar*)data)- G_STRUCT_OFFSET(DataMessage, data));
}
static void _c_request 		( GnomenuSocket * socket, GnomenuSocketNativeID target ) {
	
}
static void _c_connected 			( GnomenuSocket * socket, GnomenuSocketNativeID target ) {
	GET_OBJECT(socket, self, priv);
	priv->time_source = g_timeout_add_seconds(self->timeout, _test_connection, self);
}
static void _c_shutdown 			( GnomenuSocket * socket ) {
	GET_OBJECT_LOG(socket, self, priv);
	if(priv->time_source)
		g_source_remove(priv->time_source);
	priv->time_source = 0;
	g_queue_for(priv->data_queue, DataMessage * msg, g_free(msg));
	g_queue_clear(priv->data_queue);
	priv->acks = 0;
	self->status = GNOMENU_SOCKET_DISCONNECTED;
	if(priv->destroy_on_shutdown) g_object_unref(self);
	LOG("_c_shutdown done");
}
/*
 * vim:ts=4:sw=4
 * */
