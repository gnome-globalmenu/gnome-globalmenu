#include <config.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <string.h>

#include "socket.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"
#undef PING_ECHO_ALIVE

#ifndef GDK_WINDOWING_X11
#error ONLY X11 is supported. other targets are not yet supported.
#endif

#define GNOMENU_SOCKET_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_SOCKET, GnomenuSocketPrivate))

#define SELF (GNOMENU_SOCKET(_self))
#define PRIV (GNOMENU_SOCKET_GET_PRIVATE(_self))

#define GET_OBJECT(_s, s, p) \
	GnomenuSocket * s = GNOMENU_SOCKET(_s); \
	GnomenuSocketPrivate * p = GNOMENU_SOCKET_GET_PRIVATE(_s);

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("%s<GnomenuSocket>::" fmt, SELF->name, ## args)
#else
#define LOG(fmt, args...)
#endif 
#define LOG_FUNC_NAME LOG("%s", __func__)
#define GNOMENU_SOCKET_ATOM_STRING "GNOMENU_SOCKET_MESSAGE"

enum {
	PROP_0,
	PROP_NAME,
	PROP_TARGET,
	PROP_STATUS,
	PROP_TIMEOUT
};
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
	int foo;
};
/**
 * GnomenuSocketHeaderType:
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
	GNOMENU_SOCKET_BROADCAST = 0,
	GNOMENU_SOCKET_CONNECT_REQ = 1,
	GNOMENU_SOCKET_CONNECT_ACK = 2,
	GNOMENU_SOCKET_ACK = 3 ,
	GNOMENU_SOCKET_DATA = 4,
	GNOMENU_SOCKET_SHUTDOWN = 5,
	GNOMENU_SOCKET_ISALIVE = 6,
	GNOMENU_SOCKET_ALIVE = 7 ,
	GNOMENU_SOCKET_PING = 8,
	GNOMENU_SOCKET_ECHO = 9
} GnomenuSocketHeaderType;

/**
 * GnomenuSocketHeader:
 *	@header_type: type of the header
 *	@bytes: length of the data(the size of the header is excluded!
 *	@seq: seq id of this package. unused.
 *	@source: source socket of this msg
 *
 *  Provides enough information to make possible the data diagram.
 */
typedef struct {
	guint8 type;
	guint8 bytes;
	gushort seq;
	GnomenuSocketNativeID source;
} GnomenuSocketHeader;


typedef struct _GnomenuSocketMessage {
	GnomenuSocketHeader header;
	union {
		guint32 l[3];
		guint16 s[6];
		guint8  b[12];
	} data;
} GnomenuSocketMessage;

#define FILL_HEADER(msg, t, src, b, s) \
{	GnomenuSocketHeader * header = (GnomenuSocketHeader*)msg; \
	header->type = (t); \
	header->bytes = (b);  \
	header->seq = (s);  \
	header->source = gnomenu_socket_get_native(src);}



/* GObject interface */
static GObject * _constructor 	( GType type, 
								  guint n_construct_properties, 
								  GObjectConstructParam *construct_params );
static void _dispose 			( GObject * _self );
static void _finalize			( GObject * _self );
static void _set_property 		( GObject * _self, 
								  guint property_id, const GValue * value, GParamSpec * pspec );
static void _get_property 		( GObject * _self, 
								  guint property_id, GValue * value, GParamSpec * pspec );

/* Default signal closures */
static void _c_data_arrival 		( GnomenuSocket * _self, gpointer data, guint size );
static void _c_connect_req 		( GnomenuSocket * _self, GnomenuSocketNativeID target );
static void _c_connected 			( GnomenuSocket * _self, GnomenuSocketNativeID target );
static void _c_shutdown 			( GnomenuSocket * _self );

/* Raw data sending */
static gboolean _raw_send 		( GnomenuSocket * _self, 
								  GdkNativeWindow target, gpointer data, guint bytes );
static gboolean 
_raw_send_nosync				( GnomenuSocket * _self, 
								  GdkNativeWindow target, gpointer data, guint bytes );
static gboolean 
_raw_broadcast_by_name
								( GnomenuSocket * _self, gchar * name, gpointer data, guint bytes );
/* utility functions */
static void 
_destroy_on_shutdown			( GnomenuSocket * _self, gpointer userdata);
static GList * 
_gnomenu_socket_find_targets		( GnomenuSocket * _self, gchar * name);
static gboolean 
/*send the ISALIVE message, invoked every _self->timeout*/
_gnomenu_socket_is_alive			(GnomenuSocket * _self); 

/* Wrap XClientMessage */
static GdkFilterReturn 
_window_filter_cb				(GdkXEvent* xevent, GdkEvent * event, gpointer data);

static gulong 
class_signals[SIGNAL_MAX] 		= {0};

G_DEFINE_TYPE 					(GnomenuSocket, gnomenu_socket, G_TYPE_OBJECT)

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

	klass->data_arrival = _c_data_arrival;
	klass->connect_req = _c_connect_req;
	klass->connected = _c_connected;
	klass->shutdown = _c_shutdown;

	class_signals[DATA_ARRIVAL] =
/**
 * GnomenuSocket::data-arrival:
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
			G_STRUCT_OFFSET (GnomenuSocketClass, data_arrival),
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
 * GnomenuSocket::connect-request:
 * @self: the #GnomenuSocket that receives this signal.
 * @data: the received data. It is owned by @self and the signal handler 
 * 		should not free it.
 * @bytes: the length of received data.
 *
 * The ::data-arrival signal is emitted each time a message arrives to
 * the socket.
 */
		g_signal_new ("request",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuSocketClass, connect_req),
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
			G_STRUCT_OFFSET (GnomenuSocketClass, connected),
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
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuSocketClass, shutdown),
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
 * GnomenuSocket:target:
 *
 * the target to connect. #GnomenuSocketNativeID
 */
	g_object_class_install_property (gobject_class, 
			PROP_TARGET,
			g_param_spec_uint ("target",
						"GnomenuSocket target prop",
						"Set GnomenuSocket's target",
						0, G_MAXUINT, 0, 
						G_PARAM_READWRITE));
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
						G_PARAM_READWRITE));
/**
 * GnomenuSocket::timeout
 *
 * number of seconds for the socket to shutdown without other peers response
 */
	g_object_class_install_property (gobject_class,
			PROP_TIMEOUT,
			g_param_spec_int ("timeout",
						"GnomenuSocket timeout prop",
						"Set GnomenuSocket's timeout",
						0, 2000, 30,
						G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
}

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	GObject * _self = ( *G_OBJECT_CLASS(gnomenu_socket_parent_class)->constructor)
						( type,
						  n_construct_properties,
						  construct_params);
	GET_OBJECT(_self, self, priv);
	self->queue = g_queue_new();
	self->acks = 0;
	self->alives = 0;
	priv->disposed = FALSE;

	gdk_window_add_filter(self->window, _window_filter_cb, self);

	return _self;
}

static void
gnomenu_socket_init (GnomenuSocket * _self){
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;
	GET_OBJECT(_self, self, priv);

	self->display = gdk_display_get_default();
	attr.title = "";
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;
	mask = 0;
	priv->time_source = 0;
	//mask = GDK_WA_TITLE;

	self->window = gdk_window_new(NULL, &attr, mask);
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
gnomenu_socket_accept (GnomenuSocket * _self, GnomenuSocket * service, GnomenuSocketNativeID target){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(self->status == GNOMENU_SOCKET_LISTEN){
		GnomenuSocketMessage ack;
		GET_OBJECT(service, s_self, s_priv);
		s_self->target = target;
		g_signal_connect(G_OBJECT(service), "shutdown", G_CALLBACK(_destroy_on_shutdown), NULL);
		g_signal_emit(G_OBJECT(service), class_signals[CONNECTED], 0, target);
		FILL_HEADER(&ack, GNOMENU_SOCKET_CONNECT_ACK, service, 0, 0);
		return _raw_send(service, service->target, &ack, sizeof(ack));
	} else{
		g_error("the socket is not listening, how can you ACCEPT?");
		return NULL;
	}

}
/**
 * _dispose:
 * 	@object: The #GnomenuSocket to be disposed.
 *
 * Disposer. Note that it don't shutdown any existed connections.
 */
static void
_dispose (GObject * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(! priv->disposed){
		gdk_window_remove_filter(self->window, _window_filter_cb, self);
		if(priv->time_source)
			g_source_remove(priv->time_source);
		priv->disposed = TRUE;	
	}
	G_OBJECT_CLASS(gnomenu_socket_parent_class)->dispose(_self);
}
/**
 * _finalize:
 * @object: the #GnomenuSocket to be finalized.
 *
 *  free all the resoursed occupied by @object
 **/
static void
_finalize(GObject * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	gdk_window_destroy(self->window);
	g_free(self->name);
	g_queue_free(self->queue);
	G_OBJECT_CLASS(gnomenu_socket_parent_class)->finalize(_self);
}

/**
 * gnomenu_socket_get_native:
 * 	@_self: the #GnomenuSocket this method acts on.
 *
 * find out the native id of the socket
 *
 * Returns: the native id of this socket. It happens to be the Window ID of the
 * 	@GnomenuWindow the GnomenuSocket wraps, in current implement.
 */
GnomenuSocketNativeID gnomenu_socket_get_native(GnomenuSocket * _self){
	return GDK_WINDOW_XWINDOW(_self->window);
}
/**
 * gnomenu_socket_connect:
 * @self: self
 * @target: the native id of the target socket.
 *
 * Connect to a remote socket. Don't check if the target socket is in listening
 * state.
 *
 * If the target socket is listening, a GnomenuSocket::connect-request signal
 * will be emitted. Then it is up to the user  whether to accept this request.
 * in the signal handler.
 */
gboolean gnomenu_socket_connect(GnomenuSocket * _self, GnomenuSocketNativeID target){
	GnomenuSocketMessage msg;
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(self->status != GNOMENU_SOCKET_DISCONNECTED){
		g_warning("Can not change to CONNECTED state from other than DISCONNECTED state: %s", 
			gnomenu_socket_status_get_value(self->status)->value_name);
		return FALSE;
	}
	FILL_HEADER(&msg, GNOMENU_SOCKET_CONNECT_REQ, self, 0, 0);
	return _raw_send(self, target, &msg, sizeof(msg));
}
/** 
 * gnomenu_socket_connect_by_name:
 * @_self: self
 * @name: the name of the remote socket to connect to. It has to be listening.
 *
 * connect to a remote socket by name. If multiple socket has the same name,
 * only one of them will receive the connecting request. It is possible that
 * the one that receives the request is not the listening one.
 * */
gboolean gnomenu_socket_connect_by_name(GnomenuSocket * _self, gchar * name){
	GnomenuSocketNativeID target;
	GET_OBJECT(_self, self, priv);
	GList * list;
	LOG_FUNC_NAME;

	list = _gnomenu_socket_find_targets(self, name);
	if(!list) return FALSE;
	target = (GnomenuSocketNativeID)list->data;
	g_list_free(list);
	return gnomenu_socket_connect(self, target);
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
gboolean gnomenu_socket_listen(GnomenuSocket * _self){
	LOG_FUNC_NAME;
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
gboolean gnomenu_socket_send(GnomenuSocket * _self, gpointer data, guint bytes){
	GnomenuSocketMessage * msg = g_new0(GnomenuSocketMessage, 1);
	LOG_FUNC_NAME;
	g_return_val_if_fail(GNOMENU_IS_SOCKET(_self), FALSE);
	GET_OBJECT(_self, self, priv);
	gchar buffer[1024];
	gint i;
	gint j;
	if(bytes >12){
		g_error("Can not send more than 12 byte");
		return FALSE;
	}
	if(self->status != GNOMENU_SOCKET_CONNECTED){
		LOG("Not connected, can not send");
		return FALSE;
	}
	FILL_HEADER(msg, GNOMENU_SOCKET_DATA, self, bytes, 0);
	g_memmove(msg->data.l, data, bytes);
	LOG("status = %s, ACKS=%d", gnomenu_socket_status_get_value(self->status)->value_name, self->acks);
	for(i = 0, j=0; i< bytes && j< sizeof(buffer); i++){
		j+=g_sprintf(&buffer[j], "%02hhX ", ((gchar * ) data) [i]);
	}
	LOG("data = %s", buffer);
	g_queue_push_tail(self->queue, 	msg);
	gnomenu_socket_flush(self);
	LOG("send queue length = %d\n", g_queue_get_length(self->queue));
	return TRUE;
}
/**
 * gnomenu_socket_broadcast_by_name:
 * 	@self:
 * 	@name:
 * 	@data:
 * 	@bytes:
 * 
 * broadcast a message to every socket with a given name.
 */
gboolean gnomenu_socket_broadcast(GnomenuSocket * _self, gpointer data, guint bytes){
	return gnomenu_socket_broadcast_by_name(_self, NULL, data, bytes);
}
gboolean gnomenu_socket_broadcast_by_name(GnomenuSocket * _self, gchar * name, gpointer data, guint bytes){
	GnomenuSocketMessage msg;
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(bytes >12) {
		g_error("%s: Can not send more than 12 bytes", __func__);
		return FALSE;
	}
	FILL_HEADER(&msg, GNOMENU_SOCKET_BROADCAST, self, bytes, 0);
	g_memmove(msg.data.l, data, bytes);
	return _raw_broadcast_by_name(self, name, &msg, sizeof(msg));	
}
/**
 * gnomenu_socket_shutdown:
 *	@_self: the socket to shutdown.
 *
 * Shutdown a socket connection. Note that this doesn't unref the object,
 * unless you have connected GnomenuSocket::shutdown to
 * gnomenu_socket_destroy_on_shutdown();
 */
void gnomenu_socket_shutdown(GnomenuSocket * _self) {
	GnomenuSocketMessage msg;
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(self->status == GNOMENU_SOCKET_CONNECTED){
		self->status = GNOMENU_SOCKET_DISCONNECTED;
		FILL_HEADER(&msg, GNOMENU_SOCKET_SHUTDOWN, self, 0, 0);
		_raw_send(self, self->target, &msg, sizeof(msg));
		g_signal_emit(G_OBJECT(self),
			class_signals[SHUTDOWN], 0);	
	} else{
		g_warning("Not connected, can not shutdown");
	}
}
/**
 * gnomenu_socket_flush:
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
 */
gboolean gnomenu_socket_flush(GnomenuSocket * _self){
	while(_self->acks && !g_queue_is_empty(_self->queue)){
		GnomenuSocketMessage * data_msg = g_queue_peek_head(_self->queue);
		if( _raw_send(_self, _self->target, data_msg, sizeof(GnomenuSocketMessage))){
			data_msg = g_queue_pop_head(_self->queue);
			g_free(data_msg);
			_self->acks--;
		} else {
		LOG("flushing queue failed");
		return FALSE;
		}
	}
	return TRUE;
}
/**
 * gnomenu_socket_window_filter_cb
 *
 * internal filter function related to this X11 implement of #GnomenuSocket
 */
static GdkFilterReturn 
	_window_filter_cb(GdkXEvent* gdkxevent, GdkEvent * event, gpointer _self){
	GET_OBJECT(_self, self, priv);
	XEvent * xevent = gdkxevent;
	if(xevent->type == ClientMessage){
		if(xevent->xclient.message_type ==
            gdk_x11_get_xatom_by_name_for_display(self->display, GNOMENU_SOCKET_ATOM_STRING)){

			GnomenuSocketMessage * msg =(GnomenuSocketMessage*) xevent->xclient.data.l;
			switch (msg->header.type){
				case GNOMENU_SOCKET_DATA:
					if(self->status == GNOMENU_SOCKET_CONNECTED){
						LOG("msg->source =%d , self->target = %d\n", msg->header.source, self->target);
						if(msg->header.source == self->target){ 
							guint bytes = msg->header.bytes; 
							GnomenuSocketMessage ack;
							FILL_HEADER(&ack, GNOMENU_SOCKET_ACK, self, 0, 0);
							_raw_send(self, self->target, &ack, sizeof(ack));
							{
							gpointer data = g_memdup(msg->data.l, bytes); 
							g_signal_emit(G_OBJECT(self), 
								class_signals[DATA_ARRIVAL],
								g_quark_from_string("peer"),
								data,
								bytes);
							}
						}
					} else
					g_warning("Wrong socket status, ignore DATA");
					return GDK_FILTER_REMOVE;
				break;
				case GNOMENU_SOCKET_CONNECT_ACK:
					if(self->status == GNOMENU_SOCKET_DISCONNECTED){
					/*These two simple lines is essential, we establish a connection here.*/
						self->status = GNOMENU_SOCKET_CONNECTED;
						self->target = msg->header.source;
						self->alives = 0;
						self->acks = 0;
						{
					/*Then we send an CONNECT_ACK to the other peer to allow it begin data transfer*/
							GnomenuSocketMessage ack;
							FILL_HEADER(&ack, GNOMENU_SOCKET_CONNECT_ACK, self, 0, 0);
							_raw_send(self, self->target, &ack, sizeof(ack));
						}
						g_signal_emit(self, class_signals[CONNECTED], 0, msg->header.source);
					}
				/*No break here*/
				case GNOMENU_SOCKET_ACK:
					if(self->status == GNOMENU_SOCKET_CONNECTED){
						if(msg->header.source == self->target){
							self->acks++;
							gnomenu_socket_flush(self);
						}
					} else
					g_warning("Wrong socket status(%d), ignore ACK", self->status);
					return GDK_FILTER_REMOVE;
				break;
				case GNOMENU_SOCKET_CONNECT_REQ:
					if(self->status == GNOMENU_SOCKET_LISTEN){
						g_signal_emit(self, 
								class_signals[CONNECT_REQ], 
								0,
								msg->header.source);
					}else
						g_warning("Wrong socket status, ignore CONNECT_REQ");
					return GDK_FILTER_REMOVE;
				break;
				case GNOMENU_SOCKET_ISALIVE:
#ifndef PING_ECHO_ALIVE
					g_warning("keep alive ping -echo is disabled");
#endif
					if(self->status == GNOMENU_SOCKET_CONNECTED
						&& self->target == msg->header.source)
					{
						GnomenuSocketMessage alive;
						FILL_HEADER(&alive, GNOMENU_SOCKET_ALIVE, self, 0, 0);
						_raw_send(self, self->target, &alive, sizeof(alive));
					}
					return GDK_FILTER_REMOVE;
				break;
				case GNOMENU_SOCKET_ALIVE:
#ifndef PING_ECHO_ALIVE
					g_warning("keep alive ping -echo is disabled");
#endif
					if(self->status == GNOMENU_SOCKET_CONNECTED
						&& self->target == msg->header.source){
						if(self->alives > 0) self->alives--;
						gnomenu_socket_flush(self);
					}
					return GDK_FILTER_REMOVE;
				break;
				case GNOMENU_SOCKET_BROADCAST:
					{

						guint bytes = msg->header.bytes; /*on x11 we always round off to 12 bytes*/
						/*FIXME: bytes should be squeezed in to GnomenuSocketMessage.*/
						gpointer data = g_memdup(msg->data.l, bytes); 
						g_signal_emit(G_OBJECT(self), 
							class_signals[DATA_ARRIVAL],
							g_quark_from_string("broadcast"),
							data,
							bytes);
					}
					return GDK_FILTER_REMOVE;
				break;
				case GNOMENU_SOCKET_SHUTDOWN:
					if(self->status != GNOMENU_SOCKET_CONNECTED){
						g_warning("SHUTDOWN a non connected socket");
					} else{
						g_signal_emit(self,
								class_signals[SHUTDOWN],
								0);
					}
					return GDK_FILTER_REMOVE;
				break;
				default:
					g_error("Should never reach here!");
			}
		}
	} 
	return GDK_FILTER_CONTINUE;
}

static void _c_data_arrival(GnomenuSocket * _self,
	gpointer data, guint size){
	LOG_FUNC_NAME;
	g_free(data);
}
static void _c_connect_req(GnomenuSocket * _self,
	GnomenuSocketNativeID target){
	LOG_FUNC_NAME;
}
static void _c_connected(GnomenuSocket * _self,
	GnomenuSocketNativeID target){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	
	priv->time_source = g_timeout_add_seconds(self->timeout, _gnomenu_socket_is_alive, self);
}
static void _c_shutdown (GnomenuSocket * _self){
	GnomenuSocketMessage * queue_message;
	GET_OBJECT(_self, self, priv);
	LOG_FUNC_NAME;
	while(queue_message = g_queue_pop_head(self->queue)){
/*FIXME: maybe sending all these message will be better than freeing them*/
		g_free(queue_message);
	}
	self->acks = 0;
	self->status = GNOMENU_SOCKET_DISCONNECTED;
}
static void 
_get_property( GObject * _self, guint property_id, GValue * value, GParamSpec * pspec){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	switch (property_id){
		case PROP_NAME:
			g_value_set_string(value, self->name);
		break;
		case PROP_TARGET:
			g_value_set_uint(value, self->target);
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
_set_property( GObject * _self, guint property_id, const GValue * value, GParamSpec * pspec){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	switch (property_id){
		case PROP_NAME:
			g_free(self->name);
			self->name = g_value_dup_string(value);
			gdk_window_set_title(self->window, self->name);
		break;
		case PROP_TARGET:
			self->target = g_value_get_uint(value);
		break;
		case PROP_STATUS:
			self->status = g_value_get_enum(value);
		break;
		case PROP_TIMEOUT:
			self->timeout = g_value_get_int(value);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}
}



/**
 * gnomenu_socket_raw_send:
 * @self: the #GnomenuSocket this method acts on.
 * @target: the native window id of target socket. In current implement, this
 * 		identifies the native window id of #GnomenuSocket::window.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * This method of #GnomenuSocket sends a messange to the target socket whose id 
 * is @target. Whether or not the target is a GnomenuSocket is not checked
 * (and impossible to check). The length of data has is limited by XClientMessage
 * to be 20 bytes. (or even fewer if in the future the socket adds a header to the
 * message. Note: this is a raw message that is not reliable and do not use it directly!
 *
 * Returns: if successful, return TRUE else return FALSE.
 * SeeAlso: #gnomenu_socket_send_nosync
 */
static gboolean _raw_send(GnomenuSocket * _self, 
		GnomenuSocketNativeID target, 
		gpointer data, 
		guint bytes){

	gboolean rt;
	rt = _raw_send_nosync(_self, target, data, bytes);
    gdk_display_sync (_self->display); /*resolve the message, sync the state*/
	return rt;
}
/**
 * gnomenu_socket_raw_send_nosync:
 * @self: the #GnomenuSocket this method acts on.
 * @target: the native window id of target socket. In current implement, this
 * 		identifies the native window id of #GnomenuSocket::window.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * This function don't call #gdk_display_sync at the end. See #gdk_socket_send. 
 *
 * Returns: if sucessful, TRUE; else FALSE.
 */
static gboolean _raw_send_nosync(GnomenuSocket * _self, GnomenuSocketNativeID target, gpointer data, guint bytes){
    XClientMessageEvent xclient;
	if(bytes > 20){
		g_error("GnomenuSocket: Can not send raw data for more than 20 bytes");
		return FALSE; /*X can not send more information*/
	}

    memset (&xclient, 0, sizeof (xclient));
    xclient.window = target; /*Though X11 places no interpretation of this field, GNOMENU interpretes this field at the target window.*/
    xclient.type = ClientMessage;
    xclient.message_type = gdk_x11_get_xatom_by_name_for_display (_self->display, GNOMENU_SOCKET_ATOM_STRING);
    xclient.format = 32;
	memcpy(&xclient.data.l, data, bytes);
    gdk_error_trap_push ();
    XSendEvent (GDK_DISPLAY_XDISPLAY(_self->display),
          target,
          False, NoEventMask, (XEvent *)&xclient);
	gdk_flush();
    return gdk_error_trap_pop () == 0;
}
static gboolean _socket_exist(GnomenuSocket * self, GnomenuSocketNativeID window){
	GdkNativeWindow root_return;
	GdkNativeWindow parent_return;
	GdkNativeWindow * children_return;
	guint 	nchildren_return;
	gdk_error_trap_push();
	XQueryTree(GDK_DISPLAY_XDISPLAY(self->display),
			window, 
			&root_return ,&parent_return, 
			&children_return, &nchildren_return);
	if(gdk_error_trap_pop()) return FALSE;
	else {
		XFree(children_return);
	}
	return TRUE;
}
/**
 * _gnomenu_socket_is_alive:
 * @self:
 *
 * Returns: usually TRUE, unless the connection is dead.
 */
static gboolean _gnomenu_socket_is_alive(GnomenuSocket * _self){
/* The socket has already been destroyed.*/
	g_return_val_if_fail(GNOMENU_IS_SOCKET(_self), FALSE);
	LOG_FUNC_NAME;
	if(_self->status != GNOMENU_SOCKET_CONNECTED) return FALSE;
	LOG("alives = %d", _self->alives);
	if(
#ifdef PING_ECHO_ALIVE
			_self->alives > 2 && 
#endif
			! _socket_exist(_self, _self->target)) /*FIXME: which number is better?*/{
		
		LOG("The peer is non responding for too long. shutdown the connection");
		g_signal_emit(_self,
				class_signals[SHUTDOWN],
				0);
#ifdef PING_ECHO_ALIVE
		/*Last obligation to the other peer, hope it will receive this SHUTDOWN message*/
		{
			GnomenuSocketMessage msg;
			FILL_HEADER(&msg, GNOMENU_SOCKET_SHUTDOWN, _self, 0, 0);
			_raw_send(_self, _self->target, &msg, sizeof(msg));
		}
#endif
		return FALSE;
	} 
#ifdef PING_ECHO_ALIVE
	else {
		LOG("Send keep alive query");
		GnomenuSocketMessage msg;
		FILL_HEADER(&msg, GNOMENU_SOCKET_ISALIVE, _self, 0, 0);
		_raw_send(_self, _self->target, &msg, sizeof(msg));
		_self->alives ++;
		LOG("done");
	}
#endif
	return TRUE;
}
GnomenuSocketNativeID gnomenu_socket_lookup(GnomenuSocket * _self, gchar * name){
	GList * l = _gnomenu_socket_find_targets(_self, name);
	GnomenuSocketNativeID n = l->data;
	g_list_free(l);
	return n;
}
/**
 * _gnomenu_socket_find_targets:
 * @self: self
 * @name: the name for the targets
 * 
 * Find every possible @GnomenuSocket on this display with the name @name.
 *
 * Returns: a GList contains the list of those sockets' native ID. 
 * It is the caller's obligation to free the list.
 */
static GList * _gnomenu_socket_find_targets(GnomenuSocket * _self, gchar * name){
	GList * window_list = NULL;
	GET_OBJECT(_self, self, priv);
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
	gdk_flush();
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
		gdk_flush();
		if(!gdk_error_trap_pop()){
			if(rt == Success && type_return == type_req){
				if(name == NULL || g_str_equal(name, wm_name)){
					window_list = g_list_append(window_list, (gpointer) children_return[i]);
				}
			XFree(wm_name);
			}
		}else{
			g_warning("%s:XGetWindowProperty Failed",__func__);
		}
	}
	XFree(children_return);
	return window_list;
}

/**
 * gnomenu_socket_send_by_name:
 * @self: you understand.
 * @name: the target socket's name.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * this method find out the all the #GnomenuSocket with name @name and calls 
 * #gnomenu_socket_send_nosync to the * first (successfully sended) target
 *
 * Returns:  TRUE if the message is successfully sent to a target.
 * 	FALSE if the message is not sent to anywhere.
 */
static gboolean 
	_raw_send_by_name(GnomenuSocket * _self, gchar * name, gpointer data, guint bytes){
	GList * window_list = _gnomenu_socket_find_targets(_self, name);
	GList * node;
	gboolean rt = FALSE;
	for(node = g_list_first(window_list); node; node = g_list_next(node)){
		if(gnomenu_socket_send_nosync(_self, (GdkNativeWindow)node->data, data, bytes)){
			rt = TRUE;
			break;
		}
	}
    gdk_display_sync (_self->display); /*resolve the message, sync the state*/
	g_list_free(window_list);
	return rt;
}

/**
 * gnomenu_socket_raw_broadcast_by_name:
 * @self: you understand.
 * @name: the target socket's name.
 * @data: the data buffer. After calling this function, the buffer can be freed.
 * @bytes: the length of data wanted to send.
 *
 * this method find out the all the #GnomenuSocket with name @name and calls #gnomenu_socket_send_nosync
 * on first target.
 *
 * Returns:  TRUE if the message is successfully sent to at least one target.
 * 	FALSE if the message is not sent to anywhere.
 *
 * FIXME: 
 *    seems won't work properly if I don't trace the g_message. 
 *    Messages are sent but except for the first socket, #gnomenu_socket_window_filter 
 *    don't receive them. Perhaps this means we need to get rid of the GnomenuWindow.
 *
 * TODO:
 *    Figure out why. 
 */
static gboolean
_raw_broadcast_by_name(GnomenuSocket * _self, gchar * name, gpointer data, guint bytes){
	GList * window_list = _gnomenu_socket_find_targets(_self, name);
	GList * node;
	gboolean rt = FALSE;
	gboolean rt1;
	int n;
	for(n = 0, node = g_list_first(window_list); node; n++, node = g_list_next(node)){
		rt1 =  _raw_send_nosync(_self, (GnomenuSocketNativeID)node->data, data, bytes);
		gdk_display_sync (_self->display); /* Hope fully it will fix some random X BadWindow errors*/
		rt = rt || rt1;
	}
    gdk_display_sync (_self->display); /*resolve the message, sync the state*/
	LOG("Broadcasted to %d target", n);
	g_list_free(window_list);
	return rt;
}
static void _destroy_on_shutdown( GnomenuSocket * _self, gpointer userdata){
	g_object_unref(_self);
}
/*
vim:ts=4:sw=4
*/
