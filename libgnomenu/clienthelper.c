#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include "clienthelper.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"

#define LOG(fmt, args...) g_message("GnomenuClientHelper::" fmt, ## args)
#define LOG_FUNC_NAME LOG("%s", __func__)

typedef struct _GnomenuClientHelperPrivate GnomenuClientHelperPrivate;

struct _GnomenuClientHelperPrivate {
	gboolean disposed;
};

#define GNOMENU_CLIENT_HELPER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_CLIENT_HELPER, GnomenuClientHelperPrivate))

static void _dispose(GObject * self);
static void _finalize(GObject * self);
static void gnomenu_client_helper_init(GnomenuClientHelper * self);

static void _server_new
			(GnomenuClientHelper * self);
static void _server_destroy
			(GnomenuClientHelper * self);
static void _size_allocate
			(GnomenuClientHelper * self, GtkAllocation * allocation);
static void _size_query
			(GnomenuClientHelper * self, GtkRequisition * req);
static void _orientation_change
			(GnomenuClientHelper * self, GtkOrientation ori);
static GObject * _constructor
			(GType type, guint n_construct_properties, GObjectConstructParam *construct_params);

static void _data_arrival
			(GdkSocket * socket, gpointer data, gint bytes, gpointer userdata);
static void _connected
			(GdkSocket * socket, GdkSocketNativeID target);
static void _shutdown(GdkSocket * socket);
G_DEFINE_TYPE (GnomenuClientHelper, gnomenu_client_helper, GDK_TYPE_SOCKET)

/*< private >*/
enum {
	SERVER_NEW,
	SERVER_DESTROY,
	SIZE_ALLOCATE,
	SIZE_QUERY,
	ORIENTATION_CHANGE,
	SIGNAL_MAX
};
static guint class_signals[SIGNAL_MAX] = {0};

static void
gnomenu_client_helper_class_init(GnomenuClientHelperClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	/*FIXME: unref the type at class_finalize*/

	g_type_class_add_private(gobject_class, sizeof (GnomenuClientHelperPrivate));
	gobject_class->constructor = _constructor;
	gobject_class->dispose = _dispose;
	gobject_class->finalize = _finalize;
	
	klass->server_new = _server_new;
	klass->server_destroy = _server_destroy;
	klass->size_allocate = _size_allocate;
	klass->size_query = _size_query;
	klass->orientation_change = _orientation_change;

	class_signals[SERVER_NEW] =
/**
 * GnomenuClientHelper::server-new:
 * @self: the #GnomenuClientHelper who emits this signal.
 * @server_info: owned by GnomenuClient. Do not free it.
 * 
 * emitted when the client receives a server's announcement for its creation.
 * it is the responsibility of the true client who listens to this signal
 * to reset its internal state to get ready for a new menu server; even if the
 * client has established a relation with another menu server;
 */
		g_signal_new("server-new",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, server_new),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			0);

	class_signals[SERVER_DESTROY] =
/**
 * GnomenuClientHelper::server-destroy:
 * @self: the #GnomenuClientHelper who emits this signal.
 * @server_info: owned by GnomenuClient. Do not free it.
 * 
 * emitted when the client receives a server's announcement for its death.
 */
		g_signal_new("server-destroy",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, server_destroy),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			0);

	class_signals[SIZE_ALLOCATE] =
/**
 * GnomenuClientHelper::size-allocate:
 * @self: the #GnomenuClientHelper who emits this signal.
 * @allocation: don't free it and don't pass it around. it is disposed when the signal ends.
 *
 * emitted when client receives server's sizes allocation.
 */
		g_signal_new("size-allocate",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, size_allocate),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER
			);
	class_signals[SIZE_QUERY] =
/**
 * GnomenuClientHelper::size-query:
 * @self: the #GnomenuClientHelper who emits this signal.
 * @req: the requisition the client need to fill in.
 *
 * emitted when the client receives a server's request for querying 
 * its size requisition.
 */
		g_signal_new("size-query",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, size_query),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);

	class_signals[ORIENTATION_CHANGE] =
/**
 * GnomenuClientHelper::orientation-change:
 *
*/
		g_signal_new("orientation-change",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, orientation_change),
			NULL, NULL,
			gnomenu_marshall_VOID__UINT,
			G_TYPE_NONE,
			1,
			G_TYPE_UINT);
}

static void
gnomenu_client_helper_init(GnomenuClientHelper * self){
	LOG_FUNC_NAME;
}

/**
 * gnomenu_client_helper_new:
 * * create a new menu client object */ 
GnomenuClientHelper * 
gnomenu_client_helper_new(){
	GnomenuClientHelper * self;
	LOG_FUNC_NAME;
	self = g_object_new(GNOMENU_TYPE_CLIENT_HELPER, "name", GNOMENU_CLIENT_NAME,NULL);
	return self;
}

static GObject* _constructor(GType type, guint n_construct_properties,
		GObjectConstructParam *construct_params){
	GObject *obj;
	GnomenuClientHelper * self;
	GnomenuClientHelperPrivate * priv;
	
	obj = (*G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);

	self = GNOMENU_CLIENT_HELPER(obj);
	priv = GNOMENU_CLIENT_HELPER_GET_PRIVATE(self);

/*	self->socket = gdk_socket_new(GNOMENU_CLIENT_NAME);*/
	g_signal_connect(G_OBJECT(self), "data-arrival", G_CALLBACK(_data_arrival), NULL);
	g_signal_connect(G_OBJECT(self), "connected", G_CALLBACK(_connected), NULL);
	g_signal_connect(G_OBJECT(self), "shutdown", G_CALLBACK(_shutdown), NULL);
	priv->disposed = FALSE;

	gdk_socket_connect_by_name(GDK_SOCKET(self), GNOMENU_SERVER_NAME);
	return obj;
}
/**
 * _dispose:
 *  @object:
 *
 *  dispose the object. will shutdown the connection if it is connected
 */
static void _dispose(GObject * object){
	GdkSocket * socket = GDK_SOCKET(object);
	GnomenuClientHelperPrivate * priv = GNOMENU_CLIENT_HELPER_GET_PRIVATE(object);
	if(! priv->disposed){
		priv->disposed = TRUE;
		if(socket->status == GDK_SOCKET_CONNECTED){
			gdk_socket_shutdown(GDK_SOCKET(object));
		}
	}
	G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->dispose(object);
}

static void _finalize(GObject * object){
	LOG_FUNC_NAME;
	G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->finalize(object);
}

static void _shutdown(GdkSocket * socket){
	g_signal_emit(G_OBJECT(socket),
		class_signals[SERVER_DESTROY],
		0);

}
static void _connected(GdkSocket * socket, GdkSocketNativeID target){
	g_signal_emit(G_OBJECT(socket),
		class_signals[SERVER_NEW],
		0);
}
/** gnomenu_client_helper_data_arrival_cb:
 *
 * 	callback, invoked when the embeded socket receives data
 */
static void _data_arrival(GdkSocket * socket, 
		gpointer data, gint bytes, gpointer userdata){
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	GnomenuClientHelper * self = GNOMENU_CLIENT_HELPER(socket);

	LOG_FUNC_NAME;

	g_assert(bytes >= sizeof(GnomenuMessage));
	
	enumvalue = gnomenu_message_type_get_value(message->any.type);
	g_message("message arrival: %s", enumvalue->value_name);
	/*TODO: Dispatch the message and emit signals*/
	switch(enumvalue->value){
		case GNOMENU_MSG_SIZE_ALLOCATE:
			{
				GtkAllocation * allocation = g_new0(GtkAllocation, 1);
				allocation->width = message->size_allocate.width;
				allocation->height = message->size_allocate.height;
				g_signal_emit(G_OBJECT(self),
					class_signals[SIZE_ALLOCATE],
					0,
					allocation);
			}
		break;
		case GNOMENU_MSG_SIZE_QUERY:
			{
				GtkRequisition * req = g_new0(GtkRequisition, 1);
				g_signal_emit(G_OBJECT(self),
					class_signals[SIZE_QUERY],
					0,
					req);
			}
		break;
		case GNOMENU_MSG_ORIENTATION_CHANGE:
			{
				GtkOrientation ori = message->orientation_change.orientation;
				g_signal_emit(G_OBJECT(self),
					class_signals[ORIENTATION_CHANGE],
					0,
					ori);
			}
		break;
		default:
			g_warning("unknown message, ignore it and continue");
		break;
	}
}

static void 
_server_new(GnomenuClientHelper * self){
	LOG_FUNC_NAME;
}
static void 
_server_destroy(GnomenuClientHelper * self){
	LOG_FUNC_NAME;
	g_warning("Menu server quited before client exits");
}
static void 
_size_allocate(GnomenuClientHelper * self, GtkAllocation * allocation){
	/*other signal handlers will deal with the allocation*/
	/*At the cleanup stage, we can safely free allocation*/
	g_free(allocation);
}
static void
_size_query(GnomenuClientHelper * self, GtkRequisition * req){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SIZE_REQUEST;
	msg.size_request.width = req->width;
	msg.size_request.height = req->height;
/* FIXME: is it possible that we are in this handler, but self->server_info is NULL?,
 * Here it is almost impossible, unless a server dies before we issue this message*/
	g_free(req);
	gdk_socket_send(GDK_SOCKET(self), &msg, sizeof(msg));
}

static void 
_orientation_change
			(GnomenuClientHelper * self, GtkOrientation ori){
	LOG_FUNC_NAME;
}


void gnomenu_client_helper_send_realize(GnomenuClientHelper * self, 
		GdkWindow * ui_window){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_REALIZE;
	msg.client_realize.ui_window = GDK_WINDOW_XWINDOW(ui_window);
	gdk_socket_send(GDK_SOCKET(self),
		&msg, sizeof(msg));
}
void gnomenu_client_helper_send_reparent(GnomenuClientHelper * self, 
		GdkWindow * parent_window){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_REPARENT;
	msg.client_reparent.parent_window = GDK_WINDOW_XWINDOW(parent_window);	
	gdk_socket_send(GDK_SOCKET(self),
		&msg, sizeof(msg));
}
void gnomenu_client_helper_send_unrealize(GnomenuClientHelper * self){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_UNREALIZE;
	gdk_socket_send(GDK_SOCKET(self),
		&msg, sizeof(msg));
}

