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

static void gnomenu_client_helper_data_arrival_cb
		(GdkSocket * socket, gpointer data, gint bytes, GnomenuClientHelper * client);

G_DEFINE_TYPE (GnomenuClientHelper, gnomenu_client_helper, G_TYPE_OBJECT)

static void
gnomenu_client_helper_class_init(GnomenuClientHelperClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	/*FIXME: unref the type at class_finalize*/
	klass->type_gnomenu_message_type = g_type_class_ref(GNOMENU_TYPE_MESSAGE_TYPE); 

	g_type_class_add_private(gobject_class, sizeof (GnomenuClientHelperPrivate));
	gobject_class->dispose = gnomenu_client_helper_dispose;
	gobject_class->finalize = gnomenu_client_helper_finalize;
	
	klass->server_new = gnomenu_client_helper_server_new;
	klass->server_destroy = gnomenu_client_helper_server_destroy;
	klass->size_allocate = gnomenu_client_helper_size_allocate;
	klass->size_query = gnomenu_client_helper_size_query;

	klass->signals[GMC_SIGNAL_SERVER_NEW] =
/**
 * GnomenuClient::server-new:
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
	klass->signals[GMC_SIGNAL_SERVER_DESTROY] =
/**
 * GnomenuClient::server-destroy:
 * @self: the #GnomenuClientHelper who emits this signal.
 * @server_info: owned by GnomenuClient. Do not free it.
 * 
 * emitted when the client receives a server's announcement for its death.
 * 
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
	klass->signals[GMC_SIGNAL_SIZE_ALLOCATE] =
/**
 * GnomenuClient::size-allocate:
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
	klass->signals[GMC_SIGNAL_SIZE_QUERY] =
/**
 * GnomenuClient::size-query:
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

}

static void
gnomenu_client_helper_init(GnomenuClientHelper * self){
	LOG_FUNC_NAME;
}

/**
 * gnomenu_client_helper_new:
 * 
 * create a new menu client object
 */
GnomenuClientHelper * 
gnomenu_client_helper_new(){
	GnomenuClientHelper * self;
	GnomenuClientHelperPrivate * priv;
	LOG_FUNC_NAME;
	self = g_object_new(GNOMENU_TYPE_CLIENT_HELPER, NULL);
	priv = GNOMENU_CLIENT_HELPER_GET_PRIVATE(self);

	self->socket = gdk_socket_new(GNOMENU_CLIENT_NAME);
	self->server_info = NULL;

	g_signal_connect(self->socket, "data-arrival", G_CALLBACK(gnomenu_client_helper_data_arrival_cb), self);
	priv->disposed = FALSE;
	return self;
}

static void gnomenu_client_helper_dispose(GObject * object){
	GnomenuClientHelper *self;
	GnomenuClientHelperPrivate * priv;
	LOG_FUNC_NAME;
	self = GNOMENU_CLIENT_HELPER(object);
	priv = GNOMENU_CLIENT_HELPER_GET_PRIVATE(self);
	
	if(! priv->disposed){
		g_object_unref(self->socket);
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
		gpointer data, gint bytes, GnomenuClientHelper * self){
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	GnomenuServerInfo * server_info = NULL;
	guint * signals = GNOMENU_CLIENT_HELPER_GET_CLASS(self)->signals;
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
/*thus the client who listens on 
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
				signals[GMC_SIGNAL_SERVER_NEW],
				0,
				server_info);
		break;
		case GNOMENU_MSG_SERVER_DESTROY:
			if(!server_info || server_info->socket_id !=message->server_new.socket_id){
				g_warning("haven't establish a "
					"relation with that server, ignore this message");
				break;
			}
			g_signal_emit(G_OBJECT(self),
				signals[GMC_SIGNAL_SERVER_DESTROY],
				0,
				server_info);
		break;
		case GNOMENU_MSG_SIZE_ALLOCATE:
			if(!server_info || server_info->socket_id !=message->server_new.socket_id){
				g_warning("haven't establish a "
					"relation with that server, ignore this message");
				break;
			}
			{
				GtkAllocation * allocation = g_new0(GtkAllocation, 1);
				allocation->width = message->size_allocate.width;
				allocation->height = message->size_allocate.height;
				g_signal_emit(G_OBJECT(self),
					signals[GMC_SIGNAL_SIZE_ALLOCATE],
					0,
					allocation);
			}
		break;
		case GNOMENU_MSG_SIZE_QUERY:
			if(!server_info || server_info->socket_id !=message->server_new.socket_id){
				g_warning("haven't establish a "
					"relation with that server, ignore this message");
				break;
			}
			{
				GtkRequisition * req = g_new0(GtkRequisition, 1);
				g_signal_emit(G_OBJECT(self),
					signals[GMC_SIGNAL_SIZE_QUERY],
					0,
					req);
			}
		break;
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
	msg.size_request.width = req->width;
	msg.size_request.height = req->height;
/*FIXME: is it possible that we are in this handler, but self->server_info is NULL?*/
	g_free(req);
	g_return_if_fail(self->server_info);
	gdk_socket_send(self->socket, 
		self->server_info->socket_id, &msg, sizeof(msg));
}
