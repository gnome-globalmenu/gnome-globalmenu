#include <gtk/gtk.h>

#include "gnomenuclient.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"

#define LOG_FUNC_NAME g_message(__func__)

typedef struct _GnomenuClientPrivate GnomenuClientPrivate;

struct _GnomenuClientPrivate {
	gboolean disposed;
};

#define GNOMENU_CLIENT_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_CLIENT, GnomenuClientPrivate))

static void gnomenu_client_dispose(GObject * self);
static void gnomenu_client_finalize(GObject * self);
static void gnomenu_client_init(GnomenuClient * self);

static void gnomenu_client_server_new(GnomenuClient * self, GnomenuClientServerInfo * server_info);
static void gnomenu_client_server_destroy(GnomenuClient * self, GnomenuClientServerInfo * server_info);
static void gnomenu_client_allocate_size(GnomenuClient * self, GnomenuClientServerInfo * server_info, GtkAllocation * allocation);
static void gnomenu_client_query_requisition(GnomenuClient * self, GnomenuClientServerInfo * server_info, GtkRequisition * req);

static void gnomenu_client_data_arrival_cb(GdkSocket * socket, gpointer * data, gint bytes, GnomenuClient * client);

G_DEFINE_TYPE (GnomenuClient, gnomenu_client, G_TYPE_OBJECT)

static void
gnomenu_client_class_init(GnomenuClientClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	/*FIXME: unref the type at class_finalize*/
	klass->type_gnomenu_message_type = g_type_class_ref(GNOMENU_TYPE_MESSAGE_TYPE); 

	g_type_class_add_private(gobject_class, sizeof (GnomenuClientPrivate));
	gobject_class->dispose = gnomenu_client_dispose;
	gobject_class->finalize = gnomenu_client_finalize;
	
	klass->server_new = gnomenu_client_server_new;
	klass->server_destroy = gnomenu_client_server_destroy;
	klass->allocate_size = gnomenu_client_allocate_size;
	klass->query_requisition = gnomenu_client_query_requisition;

	klass->signals[GMC_SIGNAL_SERVER_NEW] =
/**
 * GnomenuClient::server-new:
 * @server_info: server_info, owned by GnomenuClient. Do not free it.
 * 
 * emitted when the client receives a server's announcement for its creation.
 */
		g_signal_new("server-new",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientClass, server_new),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);
	klass->signals[GMC_SIGNAL_SERVER_DESTROY] =
/**
 * GnomenuClient::server-destroy:
 * @server_info: server_info, owned by GnomenuClient. Do not free it.
 * 
 * emitted when the client receives a server's announcement for its death.
 */
		g_signal_new("server-destroy",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientClass, server_destroy),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);
	klass->signals[GMC_SIGNAL_ALLOCATE_SIZE] =
/**
 * GnomenuClient::allocate-size:
 * @server_info: server_info, owned by GnomenuClient. Do not free it.
 * 
 * emitted when client receives server's sizes allocation.
 */
		g_signal_new("allocate-size",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientClass, allocate_size),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER_POINTER,
			G_TYPE_NONE,
			2,
			G_TYPE_POINTER,
			G_TYPE_POINTER);
	klass->signals[GMC_SIGNAL_QUERY_REQUISITION] =
/**
 * GnomenuClient::query-requisition:
 * @server_info: server_info, owned by GnomenuClient. Do not free it.
 * 
 * emitted when the client receives a server's request for querying 
 * its size requisition.
 */
		g_signal_new("query-requisition",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientClass, allocate_size),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER_POINTER,
			G_TYPE_NONE,
			2,
			G_TYPE_POINTER,
			G_TYPE_POINTER);

}

static void
gnomenu_client_init(GnomenuClient * self){
	LOG_FUNC_NAME;
}

/**
 * gnomenu_client_new:
 * 
 * create a new menu client object
 */
GnomenuClient * 
gnomenu_client_new(){
	GnomenuClient * self;
	GnomenuClientPrivate * priv;
	LOG_FUNC_NAME;
	self = g_object_new(GNOMENU_TYPE_CLIENT, NULL);
	priv = GNOMENU_CLIENT_GET_PRIVATE(self);
	self->socket = gdk_socket_new(GNOMENU_CLIENT_NAME);
	self->server_info = NULL;

	g_signal_connect(self->socket, "data-arrival", G_CALLBACK(gnomenu_client_data_arrival_cb), self);
	priv->disposed = FALSE;
}

static void gnomenu_client_dispose(GObject * object){
	GnomenuClient *self;
	GnomenuClientPrivate * priv;
	LOG_FUNC_NAME;
	self = GNOMENU_CLIENT(object);
	priv = GNOMENU_CLIENT_GET_PRIVATE(self);
	
	if(! priv->disposed){
		g_object_unref(self->socket);
		priv->disposed = TRUE;
	}
	G_OBJECT_CLASS(gnomenu_client_parent_class)->dispose(object);
}

static void gnomenu_client_finalize(GObject * object){
	GnomenuClient *self;
	LOG_FUNC_NAME;
	self = GNOMENU_CLIENT(object);
	g_free(self->server_info);
	G_OBJECT_CLASS(gnomenu_client_parent_class)->finalize(object);
}

/** gnomenu_client_data_arrival_cb:
 *
 * 	callback, invoked when the embeded socket receives data
 */
static void gnomenu_client_data_arrival_cb(GdkSocket * socket, 
		gpointer * data, gint bytes, GnomenuClient * client){
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	LOG_FUNC_NAME;

	g_assert(bytes >= sizeof(GnomenuMessage));

	enumvalue = g_enum_get_value( 
			GNOMENU_CLIENT_GET_CLASS(client)->type_gnomenu_message_type,
					message->any.type);
	g_message("message arrival: %s", enumvalue->value_name);
	/*TODO: Dispatch the message and emit signals*/
}

static void gnomenu_client_server_new(GnomenuClient * self, GnomenuClientServerInfo * server_info){
	if(self->server_info){
		g_warning("a new server is launched befoer the old one dies");
		/*TODO: perhaps need to send the old server a client_destroy message*/
		g_free(self->server_info);
	} 
	self->server_info = server_info;
}
static void gnomenu_client_server_destroy(GnomenuClient * self, GnomenuClientServerInfo * server_info){
	g_warning("Menu server quited before client exits");
	g_free(self->server_info);
	self->server_info = NULL;
}
static void gnomenu_client_allocate_size(GnomenuClient * self, GnomenuClientServerInfo * server_info, GtkAllocation * allocation){
	/*other signal handlers will deal with the allocation*/
	/*At the cleanup stage, we can safely free allocation*/
	g_free(allocation);
}
static void gnomenu_client_query_requisition(GnomenuClient * self, GnomenuClientServerInfo * server_info, GtkRequisition * req){
	/*TODO: send the requistion to the server*/
	g_free(req);
}
