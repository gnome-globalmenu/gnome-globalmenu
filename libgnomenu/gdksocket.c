#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <string.h>

#include "gdksocket.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"

#ifndef GDK_WINDOWING_X11
#error ONLY X11 is supported. other targets are not yet supported.
#endif

#define GDK_SOCKET_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GDK_TYPE_SOCKET, GdkSocketPrivate))

#define SELF (GDK_SOCKET(_self))
#define PRIV (GDK_SOCKET_GET_PRIVATE(_self))

#define GET_OBJECT(_s, s, p) \
	GdkSocket * s = GDK_SOCKET(_s); \
	GdkSocketPrivate * p = GDK_SOCKET_GET_PRIVATE(_s);

#define LOG(fmt, args...) g_message("%s<GdkSocket>::" fmt, SELF->name, ## args)
#define LOG_FUNC_NAME LOG("%s", __func__)
#define GDK_SOCKET_ATOM_STRING "GDK_SOCKET_MESSAGE"

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
typedef struct _GdkSocketPrivate GdkSocketPrivate;
struct _GdkSocketPrivate {
	gboolean disposed;
	int foo;
};
/**
 * GdkSocketMsgType:
 * #GDK_SOCKET_BROADCAST: a broadcast (connectionless) mesage. NOT DONE.
 * #GDK_SOCKET_CONNECT_REQ: a connect request.
 * #GDK_SOCKET_ACK: ready to accept a new data
 * #GDK_SOCKET_DATA: send data
 * #GDK_SOCKET_SHUTDOWN: peer closed, clean up your stuff
 * #GDK_SOCKET_ISALIVE: are you alive?
 * #GDK_SOCKET_ALIVE: yes i am.
 * #GDK_SOCKET_PING: are you a socket?  NOT DONE
 * #GDK_SOCKET_ECHO: yes I am. NOT DONE
 * */
typedef enum {
	GDK_SOCKET_BROADCAST = 0,
	GDK_SOCKET_CONNECT_REQ = 1,
	GDK_SOCKET_CONNECT_ACK = 2,
	GDK_SOCKET_ACK = 3 ,
	GDK_SOCKET_DATA = 4,
	GDK_SOCKET_SHUTDOWN = 5,
	GDK_SOCKET_ISALIVE = 6,
	GDK_SOCKET_ALIVE = 7 ,
	GDK_SOCKET_PING = 8,
	GDK_SOCKET_ECHO = 9
} GdkSocketHeaderType;
/**
 * GdkSocketHeader:
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
	GdkSocketNativeID source;
} GdkSocketHeader;


typedef struct _GdkSocketMessage {
	GdkSocketHeader header;
	union {
		guint32 l[3];
		guint16 s[6];
		guint8  b[12];
	} data;
} GdkSocketMessage;

#define FILL_HEADER(msg, t, src, b, s) \
{	GdkSocketHeader * header = (GdkSocketHeader*)msg; \
	header->type = (t); \
	header->bytes = (b);  \
	header->seq = (s);  \
	header->source = gdk_socket_get_native(src);}



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

/* Default signal handlers */
static void _data_arrival 		( GdkSocket * _self, gpointer data, guint size );
static void _connect_req 		( GdkSocket * _self, GdkSocketNativeID target );
static void _connected 			( GdkSocket * _self, GdkSocketNativeID target );
static void _shutdown 			( GdkSocket * _self );

/* Raw data sending */
static gboolean _raw_send 		( GdkSocket * _self, 
								  GdkNativeWindow target, gpointer data, guint bytes );
static gboolean 
_raw_send_nosync				( GdkSocket * _self, 
								  GdkNativeWindow target, gpointer data, guint bytes );
static gboolean 
_raw_broadcast_by_name
								( GdkSocket * _self, gchar * name, gpointer data, guint bytes );
/* utility functions */
static void 
_destroy_on_shutdown			( GdkSocket * _self, gpointer userdata);
static GList * 
_gdk_socket_find_targets		( GdkSocket * _self, gchar * name);
static gboolean 
/*send the ISALIVE message, invoked every _self->timeout*/
_gdk_socket_is_alive			(GdkSocket * _self); 

/* Wrap XClientMessage */
static GdkFilterReturn 
_window_filter_cb				(GdkXEvent* xevent, GdkEvent * event, gpointer data);

static gulong 
class_signals[SIGNAL_MAX] 		= {0};

G_DEFINE_TYPE 					(GdkSocket, gdk_socket, G_TYPE_OBJECT)

/**
 * gdk_socket_class_init:
 *
 * Initialize the class structure of #GdkSocket
 */
static void
gdk_socket_class_init(GdkSocketClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec * pspec;

	g_type_class_add_private(gobject_class, sizeof (GdkSocketPrivate));

	gobject_class->dispose = _dispose;
	gobject_class->constructor = _constructor;
	gobject_class->finalize = _finalize;
	gobject_class->get_property = _get_property;
	gobject_class->set_property = _set_property;

	klass->data_arrival = _data_arrival;
	klass->connect_req = _connect_req;
	klass->connected = _connected;
	klass->shutdown = _shutdown;

	class_signals[DATA_ARRIVAL] =
/**
 * GdkSocket::data-arrival:
 * @self: the #GdkSocket that receives this signal.
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
		g_signal_new ("data-arrival",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_DETAILED | G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GdkSocketClass, data_arrival),
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
 * GdkSocket::connect-request:
 * @self: the #GdkSocket that receives this signal.
 * @data: the received data. It is owned by @self and the signal handler 
 * 		should not free it.
 * @bytes: the length of received data.
 *
 * The ::data-arrival signal is emitted each time a message arrives to
 * the socket.
 */
		g_signal_new ("connect-request",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GdkSocketClass, connect_req),
			NULL /* accumulator */,
			NULL /* accu_data */,
			gnomenu_marshall_VOID__UINT,
			G_TYPE_NONE /* return_type */,
			1     /* n_params */,
			G_TYPE_UINT
			);
	class_signals[CONNECTED] =
/**
 * GdkSocket::connected:
 * @self: the #GdkSocket that receives this signal.
 * @target: the other peer which this socket is connect to.
 *
 * The ::connected signal is emitted on both sides
 * when the connection request is
 * resolved and the connection is created.
 */
		g_signal_new ("connected",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GdkSocketClass, connected),
			NULL /* accumulator */,
			NULL /* accu_data */,
			gnomenu_marshall_VOID__UINT,
			G_TYPE_NONE /* return_type */,
			1     /* n_params */,
			G_TYPE_UINT
			);

	class_signals[SHUTDOWN] =
/**
 * GdkSocket::shutdown
 * @self:
 * 
 * invoked when the socket is shut down. either from this peer or
 * the other peer. the default handler will cleanup the message queue.
 */
		g_signal_new ("shutdown",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GdkSocketClass, shutdown),
			NULL,
			NULL,
			gnomenu_marshall_VOID__VOID,
			G_TYPE_NONE,
			0);

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
			
/**
 * GdkSocket:target:
 *
 * the target to connect. #GdkSocketNativeID
 */
	g_object_class_install_property (gobject_class, 
			PROP_TARGET,
			g_param_spec_uint ("target",
						"GdkSocket target prop",
						"Set GdkSocket's target",
						0, G_MAXUINT, 0, 
						G_PARAM_READWRITE));
/**
 * GdkSocket::status
 *
 * the status of the socket. #GdkSocketNativeID
 */
	g_object_class_install_property (gobject_class, 
			PROP_STATUS,
			g_param_spec_enum ("status",
						"GdkSocket status prop",
						"Set GdkSocket's status",
						gdk_socket_status_get_type(),
						GDK_SOCKET_DISCONNECTED,
						G_PARAM_READWRITE));
/**
 * GdkSocket::timeout
 *
 * number of seconds for the socket to shutdown without other peers response
 */
	g_object_class_install_property (gobject_class,
			PROP_TIMEOUT,
			g_param_spec_int ("timeout",
						"GdkSocket timeout prop",
						"Set GdkSocket's timeout",
						0, 2000, 30,
						G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
}

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	GObject * _self;
	GdkSocket * self ;
	GdkSocketPrivate * priv;
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;
	gchar *name;

	_self = ( *G_OBJECT_CLASS(gdk_socket_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);
	self =  SELF;

	priv = PRIV;

	self->display = gdk_display_get_default();
	attr.title = self->name;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;
	mask = GDK_WA_TITLE;

	self->window = gdk_window_new(NULL, &attr, mask);

	self->queue = g_queue_new();
	self->acks = 0;
	self->alives = 0;

	gdk_window_add_filter(self->window, _window_filter_cb, self);
	priv->disposed = FALSE;

	return _self;
}

static void
gdk_socket_init (GdkSocket * _self){
/* Do nothing */
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
gdk_socket_new (char * name){
	return g_object_new(GDK_TYPE_SOCKET, "name", name,
				"timeout", 10, NULL);
}

GdkSocket *
gdk_socket_accept (GdkSocket * _self, GdkSocketNativeID target){
	LOG_FUNC_NAME;
	GdkSocket * rt;
	GET_OBJECT(_self, self, priv);
	if(self->status == GDK_SOCKET_LISTEN){
		GdkSocketMessage ack;
		gchar * newname = g_strconcat(self->name, "_SERVICE", NULL);
		rt = g_object_new(GDK_TYPE_SOCKET, "name", newname, "timeout", self->timeout,NULL);
		g_free(newname);
		rt->target = target;
		g_signal_connect(G_OBJECT(rt), "shutdown", G_CALLBACK(_destroy_on_shutdown), NULL);
/*Issue the first ACK message.*/
		FILL_HEADER(&ack, GDK_SOCKET_CONNECT_ACK, rt, 0, 0);
		_raw_send(rt, rt->target, &ack, sizeof(ack));
		return rt;
	} else{
		g_error("the socket is not listening, how can you ACCEPT?");
		return NULL;
	}

}
/**
 * _dispose:
 * 	@object: The #GdkSocket to be disposed.
 *
 * Disposer. Note that it don't shutdown any existed connections.
 */
static void
_dispose (GObject * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(! priv->disposed){
		gdk_window_remove_filter(self->window, _window_filter_cb, self);
		g_object_unref(self->window);
		priv->disposed = TRUE;	
	}
	G_OBJECT_CLASS(gdk_socket_parent_class)->dispose(_self);
}
/**
 * _finalize:
 * @object: the #GdkSocket to be finalized.
 *
 *  free all the resoursed occupied by @object
 **/
static void
_finalize(GObject * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	g_free(self->name);
	g_queue_free(self->queue);
	G_OBJECT_CLASS(gdk_socket_parent_class)->finalize(_self);
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
GdkSocketNativeID gdk_socket_get_native(GdkSocket * _self){
	return GDK_WINDOW_XWINDOW(_self->window);
}
/**
 * gdk_socket_connect:
 * @self: self
 * @target: the native id of the target socket.
 *
 * Connect to a remote socket. Don't check if the target socket is in listening
 * state.
 *
 * If the target socket is listening, a GdkSocket::connect-request signal
 * will be emitted. Then it is up to the user  whether to accept this request.
 * in the signal handler.
 */
gboolean gdk_socket_connect(GdkSocket * _self, GdkSocketNativeID target){
	GdkSocketMessage msg;
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(self->status != GDK_SOCKET_DISCONNECTED){
		g_warning("Can not change to CONNECTED state from other than DISCONNECTED state: %s", 
			gdk_socket_status_get_value(self->status)->value_name);
		return FALSE;
	}
	FILL_HEADER(&msg, GDK_SOCKET_CONNECT_REQ, self, 0, 0);
	return _raw_send(self, target, &msg, sizeof(msg));
}
/** gdk_socket_connect_by_name:
 * @self: self
 * @name: the name of the remote socket to connect to. It has to be listening.
 *
 * connect to a remote socket by name. If multiple socket has the same name,
 * only one of them will receive the connecting request. It is possible that
 * the one that receives the request is not the listening one.
 * */
gboolean gdk_socket_connect_by_name(GdkSocket * _self, gchar * name){
	GdkSocketNativeID target;
	GET_OBJECT(_self, self, priv);
	GList * list;
	LOG_FUNC_NAME;

	list = _gdk_socket_find_targets(self, name);
	if(!list) return FALSE;
	target = (GdkSocketNativeID)list->data;
	g_list_free(list);
	return gdk_socket_connect(self, target);
}
gboolean gdk_socket_listen(GdkSocket * _self){
	LOG_FUNC_NAME;
	_self->status = GDK_SOCKET_LISTEN;
/* FIXME: should I broadcast some message here or invoke a signal?*/
/* NOTE: don't broadcast any message here. maybe a signal will
 * be useful, but not now.*/
/* FIXME: should I set a flag on the window so that other sockets can
 * test if this socket is listening?*/

	return TRUE;
}
/**
 * gdk_socket_send:
 * @data: you can free the data after calling this function
 */
gboolean gdk_socket_send(GdkSocket * _self, gpointer data, guint bytes){
	GdkSocketMessage * msg = g_new0(GdkSocketMessage, 1);
	GET_OBJECT(_self, self, priv);
	gchar buffer[1024];
	gint i;
	gint j;
	LOG_FUNC_NAME;
	if(bytes >12){
		g_error("Can not send more than 12 byte");
		return FALSE;
	}
	if(self->status != GDK_SOCKET_CONNECTED){
		g_warning("Not connected, can not send");
		return FALSE;
	}
	FILL_HEADER(msg, GDK_SOCKET_DATA, self, bytes, 0);
	g_memmove(msg->data.l, data, bytes);
	LOG("status = %s, ACKS=%d", gdk_socket_status_get_value(self->status)->value_name, self->acks);
	for(i = 0, j=0; i< bytes && j< sizeof(buffer); i++){
		j+=g_sprintf(&buffer[j], "%02hhX ", ((gchar * ) data) [i]);
	}
	LOG("data = %s", buffer);
	if(self->acks <= 0){
		g_queue_push_tail(self->queue, 	msg);
	} else {
		_raw_send(self, self->target, msg, sizeof(GdkSocketMessage));
		self->acks--;
		g_free(msg);
	}
	return TRUE;
}
/**
 * gdk_socket_broadcast_by_name:
 * 	@self:
 * 	@name:
 * 	@data:
 * 	@bytes:
 * 
 * broadcast a message to every socket with a given name.
 */
gboolean gdk_socket_broadcast_by_name(GdkSocket * _self, gchar * name, gpointer data, guint bytes){
	GdkSocketMessage msg;
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(bytes >12) {
		g_error("%s: Can not send more than 12 bytes", __func__);
		return FALSE;
	}
	FILL_HEADER(&msg, GDK_SOCKET_BROADCAST, self, bytes, 0);
	g_memmove(msg.data.l, data, bytes);
	return _raw_broadcast_by_name(self, name, &msg, sizeof(msg));	
}
/**
 * gdk_socket_shutdown:
 *
 * Shutdown a socket connection. Note that this doesn't unref the object.
 */
void gdk_socket_shutdown(GdkSocket * _self) {
	GdkSocketMessage msg;
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	if(self->status == GDK_SOCKET_CONNECTED){
		self->status = GDK_SOCKET_DISCONNECTED;
		FILL_HEADER(&msg, GDK_SOCKET_SHUTDOWN, self, 0, 0);
		_raw_send(self, self->target, &msg, sizeof(msg));
		g_signal_emit(G_OBJECT(self),
			class_signals[SHUTDOWN], 0);	
	} else{
		g_warning("Not connected, can not shutdown");
	}
}
/**
 * gdk_socket_window_filter_cb
 *
 * internal filter function related to this X11 implement of #GdkSocket
 */
static GdkFilterReturn 
	_window_filter_cb(GdkXEvent* gdkxevent, GdkEvent * event, gpointer _self){
	GET_OBJECT(_self, self, priv);
	XEvent * xevent = gdkxevent;
	if(xevent->type == ClientMessage){
		if(xevent->xclient.message_type ==
            gdk_x11_get_xatom_by_name_for_display(self->display, GDK_SOCKET_ATOM_STRING)){

			GdkSocketMessage * msg =(GdkSocketMessage*) xevent->xclient.data.l;
			switch (msg->header.type){
				case GDK_SOCKET_DATA:
					if(self->status == GDK_SOCKET_CONNECTED){
						LOG("msg->source =%d , self->target = %d\n", msg->header.source, self->target);
						if(msg->header.source == self->target){ 
							guint bytes = msg->header.bytes; /*on x11 we always round off to 12 bytes*/
							/*FIXME: bytes should be squeezed in to GdkSocketMessage.*/
							GdkSocketMessage ack;
							FILL_HEADER(&ack, GDK_SOCKET_ACK, self, 0, 0);
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
				case GDK_SOCKET_CONNECT_ACK:
					if(self->status == GDK_SOCKET_DISCONNECTED){
					/*These two simple lines is essential, we establish a connection here.*/
						self->status = GDK_SOCKET_CONNECTED;
						self->target = msg->header.source;
						{
					/*Then we send an CONNECT_ACK to the other peer to allow it begin data transfer*/
							GdkSocketMessage ack;
							FILL_HEADER(&ack, GDK_SOCKET_CONNECT_ACK, self, 0, 0);
							_raw_send(self, self->target, &ack, sizeof(ack));
						}
						g_signal_emit(self, class_signals[CONNECTED], 0, msg->header.source);
					}
				/*No break here*/
				case GDK_SOCKET_ACK:
					if(self->status == GDK_SOCKET_CONNECTED){
						if(msg->header.source == self->target){
							self->acks++;
							if(!g_queue_is_empty(self->queue)){
								GdkSocketMessage * data_msg = g_queue_pop_head(self->queue);
								_raw_send(self, self->target, data_msg, sizeof(GdkSocketMessage));
								self->acks--;
							}
						}
					} else
					g_warning("Wrong socket status(%d), ignore ACK", self->status);
					return GDK_FILTER_REMOVE;
				break;
				case GDK_SOCKET_CONNECT_REQ:
					if(self->status == GDK_SOCKET_LISTEN){
						g_signal_emit(self, 
								class_signals[CONNECT_REQ], 
								0,
								msg->header.source);
					}else
						g_warning("Wrong socket status, ignore CONNECT_REQ");
					return GDK_FILTER_REMOVE;
				break;
				case GDK_SOCKET_ISALIVE:
					if(self->status == GDK_SOCKET_CONNECTED
						&& self->target == msg->header.source)
					{
						GdkSocketMessage alive;
						FILL_HEADER(&alive, GDK_SOCKET_ALIVE, self, 0, 0);
						_raw_send(self, self->target, &alive, sizeof(alive));
					}
					return GDK_FILTER_REMOVE;
				break;
				case GDK_SOCKET_ALIVE:
					if(self->status == GDK_SOCKET_CONNECTED
						&& self->alives > 0
						&& self->target == msg->header.source){
						self->alives--;
					}
					return GDK_FILTER_REMOVE;
				break;
				case GDK_SOCKET_BROADCAST:
					{

						guint bytes = msg->header.bytes; /*on x11 we always round off to 12 bytes*/
						/*FIXME: bytes should be squeezed in to GdkSocketMessage.*/
						gpointer data = g_memdup(msg->data.l, bytes); 
						g_signal_emit(G_OBJECT(self), 
							class_signals[DATA_ARRIVAL],
							g_quark_from_string("broadcast"),
							data,
							bytes);
					}
					return GDK_FILTER_REMOVE;
				break;
				case GDK_SOCKET_SHUTDOWN:
					if(self->status != GDK_SOCKET_CONNECTED){
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

static void _data_arrival(GdkSocket * _self,
	gpointer data, guint size){
	LOG_FUNC_NAME;
	g_free(data);
}
static void _connect_req(GdkSocket * _self,
	GdkSocketNativeID target){
	LOG_FUNC_NAME;
}
static void _connected(GdkSocket * _self,
	GdkSocketNativeID target){
	LOG_FUNC_NAME;
	g_timeout_add_seconds(_self->timeout, _gdk_socket_is_alive, _self);
}
static void _shutdown (GdkSocket * _self){
	GdkSocketMessage * queue_message;
	GET_OBJECT(_self, self, priv);
	LOG_FUNC_NAME;
	while(queue_message = g_queue_pop_head(self->queue)){
/*FIXME: maybe sending all these message will be better than freeing them*/
		g_free(queue_message);
	}
	self->acks = 0;
	self->status = GDK_SOCKET_DISCONNECTED;
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
 * gdk_socket_raw_send:
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
 * message. Note: this is a raw message that is not reliable and do not use it directly!
 *
 * Returns: if successful, return TRUE else return FALSE.
 * SeeAlso: #gdk_socket_send_nosync
 */
static gboolean _raw_send(GdkSocket * _self, 
		GdkNativeWindow target, 
		gpointer data, 
		guint bytes){

	gboolean rt;
	rt = _raw_send_nosync(_self, target, data, bytes);
    gdk_display_sync (_self->display); /*resolve the message, sync the state*/
	return rt;
}
/**
 * gdk_socket_raw_send_nosync:
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
static gboolean _raw_send_nosync(GdkSocket * _self, GdkNativeWindow target, gpointer data, guint bytes){
    XClientMessageEvent xclient;
	if(bytes > 20){
		g_error("GdkSocket: Can not send raw data for more than 20 bytes");
		return FALSE; /*X can not send more information*/
	}

    memset (&xclient, 0, sizeof (xclient));
    xclient.window = target; /*Though X11 places no interpretation of this field, GDK interpretes this field at the target window.*/
    xclient.type = ClientMessage;
    xclient.message_type = gdk_x11_get_xatom_by_name_for_display (_self->display, GDK_SOCKET_ATOM_STRING);
    xclient.format = 32;
	memcpy(&xclient.data.l, data, bytes);
    gdk_error_trap_push ();
    XSendEvent (GDK_DISPLAY_XDISPLAY(_self->display),
          target,
          False, NoEventMask, (XEvent *)&xclient);
    return gdk_error_trap_pop () == 0;
}
/**
 * _gdk_socket_is_alive:
 * @self:
 *
 * Returns: usually TRUE, unless the connection is dead.
 */
static gboolean _gdk_socket_is_alive(GdkSocket * _self){
	g_return_val_if_fail(GDK_IS_SOCKET(_self), FALSE); /*The socket already is destroyed*/
	LOG_FUNC_NAME;
	if(_self->status != GDK_SOCKET_CONNECTED) return FALSE;
	if(_self->alives >2) /*FIXME: which number is better?*/{
		g_signal_emit(_self,
				class_signals[SHUTDOWN],
				0);
		/*Last obligation to the other peer, hope it will receive this SHUTDOWN message*/
		{
			GdkSocketMessage msg;
			FILL_HEADER(&msg, GDK_SOCKET_SHUTDOWN, _self, 0, 0);
			_raw_send(_self, _self->target, &msg, sizeof(msg));
		}
		return FALSE;
	} else {
		GdkSocketMessage msg;
		FILL_HEADER(&msg, GDK_SOCKET_ISALIVE, _self, 0, 0);
		_raw_send(_self, _self->target, &msg, sizeof(msg));
		_self->alives ++;
	}
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
static GList * _gdk_socket_find_targets(GdkSocket * _self, gchar * name){
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
static gboolean 
	_raw_send_by_name(GdkSocket * _self, gchar * name, gpointer data, guint bytes){
	GList * window_list = _gdk_socket_find_targets(_self, name);
	GList * node;
	gboolean rt = FALSE;
	for(node = g_list_first(window_list); node; node = g_list_next(node)){
		if(gdk_socket_send_nosync(_self, (GdkNativeWindow)node->data, data, bytes)){
			rt = TRUE;
			break;
		}
	}
    gdk_display_sync (_self->display); /*resolve the message, sync the state*/
	g_list_free(window_list);
	return rt;
}

/**
 * gdk_socket_raw_broadcast_by_name:
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
static gboolean
_raw_broadcast_by_name(GdkSocket * _self, gchar * name, gpointer data, guint bytes){
	GList * window_list = _gdk_socket_find_targets(_self, name);
	GList * node;
	gboolean rt = FALSE;
	gboolean rt1;
	int n;
	for(n = 0, node = g_list_first(window_list); node; n++, node = g_list_next(node)){
		rt1 =  _raw_send_nosync(_self, (GdkNativeWindow)node->data, data, bytes);
		rt = rt || rt1;
	}
    gdk_display_sync (_self->display); /*resolve the message, sync the state*/
	LOG("Broadcasted to %d target", n);
	g_list_free(window_list);
	return rt;
}
static void _destroy_on_shutdown( GdkSocket * _self, gpointer userdata){
	g_object_unref(_self);
}
