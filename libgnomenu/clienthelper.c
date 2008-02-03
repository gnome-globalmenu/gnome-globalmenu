#include <gtk/gtk.h>

#include "clienthelper.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"

#define LOG_FUNC_NAME g_message(__func__)

typedef struct _GnomenuClientHelperPrivate GnomenuClientHelperPrivate;

struct _GnomenuClientHelperPrivate {
	gboolean disposed;
};

#define GNOMENU_CLIENT_HELPER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_CLIENT_HELPER, GnomenuClientHelperPrivate))

static void gnomenu_client_helper_dispose(GObject * self);
static void gnomenu_client_helper_finalize(GObject * self);
static void gnomenu_client_helper_init(GnomenuClientHelper * self);

static void gnomenu_client_helper_server_new
			(GnomenuClientHelper * self, GnomenuServerInfo * server_info);
static void gnomenu_client_helper_server_destroy
			(GnomenuClientHelper * self, GnomenuServerInfo * server_info);
static void gnomenu_client_helper_size_allocate
			(GnomenuClientHelper * self, GtkAllocation * allocation);
static void gnomenu_client_helper_size_query
			(GnomenuClientHelper * self, GtkRequisition * req);
static void gnomenu_client_helper_orientation_change
			(GnomenuClientHelper * self, GtkOrientation ori);
static GObject * gnomenu_client_helper_constructor
			(GType type, guint n_construct_properties, GObjectConstructParam *construct_params);

static void gnomenu_client_helper_data_arrival_cb
			(GdkSocket * socket, gpointer data, gint bytes, gpointer userdata);

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
	klass->type_gnomenu_message_type = g_type_class_ref(GNOMENU_TYPE_MESSAGE_TYPE); 

	g_type_class_add_private(gobject_class, sizeof (GnomenuClientHelperPrivate));
	gobject_class->constructor = gnomenu_client_helper_constructor;
	gobject_class->dispose = gnomenu_client_helper_dispose;
	gobject_class->finalize = gnomenu_client_helper_finalize;
	
	klass->server_new = gnomenu_client_helper_server_new;
	klass->server_destroy = gnomenu_client_helper_server_destroy;
	klass->size_allocate = gnomenu_client_helper_size_allocate;
	klass->size_query = gnomenu_client_helper_size_query;
	klass->orientation_change = gnomenu_client_helper_orientation_change;

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
			1,
			G_TYPE_POINTER);

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
			1,
			G_TYPE_POINTER);

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

static void gnomenu_client_helper_dispose(GObject * object){
	GnomenuClientHelper *self;
	GnomenuClientHelperPrivate * priv;
	LOG_FUNC_NAME;
	self = GNOMENU_CLIENT_HELPER(object);
	priv = GNOMENU_CLIENT_HELPER_GET_PRIVATE(self);
	
	if(self->server_info){
		GnomenuMessage msg;
		msg.any.type = GNOMENU_MSG_CLIENT_DESTROY;
		msg.client_destroy.socket_id = gdk_socket_get_native(self);
		gdk_socket_send(self, self->server_info->socket_id, &msg, sizeof(msg));
	}
	if(! priv->disposed){
		priv->disposed = TRUE;
	/*FIXME: should I send a client_destroy here?*/
	}
	G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->dispose(object);
}

static void gnomenu_client_helper_finalize(GObject * object){
	GnomenuClientHelper *self;
	LOG_FUNC_NAME;
	self = GNOMENU_CLIENT_HELPER(object);
	g_free(self->server_info);
	G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->finalize(object);
}

/** gnomenu_client_helper_data_arrival_cb:
 *
 * 	callback, invoked when the embeded socket receives data
 */
static void gnomenu_client_helper_data_arrival_cb(GdkSocket * socket, 
		gpointer data, gint bytes, gpointer userdata){
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	GnomenuServerInfo * server_info = NULL;
	GnomenuClientHelper * self = GNOMENU_CLIENT_HELPER(socket);
#define CHECK_SERVER(server, msg) \
			if (!(server) || (server)->socket_id !=(msg)->any.socket_id) {\
				g_warning("haven't establish a " \
					"relation with that server, ignore this message"); \
				break; \
			} 

	LOG_FUNC_NAME;

	g_assert(bytes >= sizeof(GnomenuMessage));
	server_info = self->server_info;
	
	enumvalue = g_enum_get_value( 
			GNOMENU_CLIENT_HELPER_GET_CLASS(self)->type_gnomenu_message_type,
					message->any.type);
	g_message("message arrival: %s", enumvalue->value_name);
	/*TODO: Dispatch the message and emit signals*/
	switch(enumvalue->value){
		case GNOMENU_MSG_SERVER_NEW:
			if(server_info){
				g_warning("already established a relation with a menu server."
					"so let's forget about the old one");
/* thus the client who listens on 
 * ::server-new signal has to make sure 
 * it forget everything about the former menu server*/
			/*FIXME: perhaps send a client-destroy to the old server is polite*/
				g_free(server_info);
			}
			server_info = g_new0(GnomenuServerInfo, 1);
			server_info->socket_id = message->server_new.socket_id;
			/*FIXME: the container_window field in the message is defined, 
  			but never used. MAKE SURE it is well defined!*/
			g_signal_emit(G_OBJECT(self),
				class_signals[SERVER_NEW],
				0,
				server_info);
		break;
		case GNOMENU_MSG_SERVER_DESTROY:
			CHECK_SERVER(server_info, message);
			g_signal_emit(G_OBJECT(self),
				class_signals[SERVER_DESTROY],
				0,
				server_info);
		break;
		case GNOMENU_MSG_SIZE_ALLOCATE:
			CHECK_SERVER(server_info, message);
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
			CHECK_SERVER(server_info, message);
			{
				GtkRequisition * req = g_new0(GtkRequisition, 1);
				g_signal_emit(G_OBJECT(self),
					class_signals[SIZE_QUERY],
					0,
					req);
			}
		break;
		case GNOMENU_MSG_ORIENTATION_CHANGE:
			CHECK_SERVER(server_info, message);
			{
				GtkOrientation ori = message->orientation_change.orientation;
				g_signal_emit(G_OBJECT(self),
					class_signals[ORIENTATION_CHANGE],
					0,
					ori);
			}
		default:
			g_warning("unknown message, ignore it and continue");
		break;
	}
}

static void 
gnomenu_client_helper_server_new(GnomenuClientHelper * self, GnomenuServerInfo * server_info){
	LOG_FUNC_NAME;
	self->server_info = server_info;
}
static void 
gnomenu_client_helper_server_destroy(GnomenuClientHelper * self, GnomenuServerInfo * server_info){
	g_warning("Menu server quited before client exits");
	g_free(server_info);
	self->server_info = NULL;
}
static void 
gnomenu_client_helper_size_allocate(GnomenuClientHelper * self, GtkAllocation * allocation){
	/*other signal handlers will deal with the allocation*/
	/*At the cleanup stage, we can safely free allocation*/
	g_free(allocation);
}
static void
gnomenu_client_helper_size_query(GnomenuClientHelper * self, GtkRequisition * req){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SIZE_REQUEST;
	msg.size_request.socket_id = gdk_socket_get_native(self);
	msg.size_request.width = req->width;
	msg.size_request.height = req->height;
/*FIXME: is it possible that we are in this handler, but self->server_info is NULL?*/
	g_free(req);
	g_return_if_fail(self->server_info);
	gdk_socket_send(GDK_SOCKET(self), 
		self->server_info->socket_id, &msg, sizeof(msg));
}

static void gnomenu_client_helper_orientation_change
			(GnomenuClientHelper * self, GtkOrientation ori){
	LOG_FUNC_NAME;
}

static GObject* gnomenu_client_helper_constructor(GType type, guint n_construct_properties,
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
	self->server_info = NULL;
	g_signal_connect(G_OBJECT(self), "data-arrival", G_CALLBACK(gnomenu_client_helper_data_arrival_cb), NULL);
	priv->disposed = FALSE;

	{
		GnomenuMessage msg;
		msg.any.type = GNOMENU_MSG_CLIENT_NEW;
		msg.client_new.socket_id = gdk_socket_get_native(self);
		gdk_socket_broadcast_by_name(self, GNOMENU_SERVER_NAME, &msg, sizeof(msg));
	}
	return obj;
}
