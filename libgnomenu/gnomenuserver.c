#include <gtk/gtk.h>

#include "gnomenuserver.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"

#define LOG_FUNC_NAME g_message(__func__)

typedef struct _GnomenuServerPrivate GnomenuServerPrivate;

struct _GnomenuServerPrivate {
	gboolean disposed;
};

#define GNOMENU_SERVER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_SERVER, GnomenuServerPrivate))

static void gnomenu_server_dispose(GObject * self);
static void gnomenu_server_finalize(GObject * self);
static void gnomenu_server_init(GnomenuServer * self);
static void gnomenu_server_client_new(GnomenuServer * self, GnomenuServerClientInfo * client_info);
static void gnomenu_server_client_destroy(GnomenuServer * self, GnomenuServerClientInfo * client_info);
static void gnomenu_server_client_size_request(GnomenuServer * self, GnomenuServerClientInfo * client_info, GtkRequisition * requisition);
static void gnomenu_server_data_arrival_cb(GdkSocket * socket, gpointer * data, gint bytes, GnomenuServer * server);

G_DEFINE_TYPE (GnomenuServer, gnomenu_server, GTK_TYPE_WIDGET)

static void
gnomenu_server_class_init(GnomenuServerClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	g_type_class_add_private(gobject_class, sizeof (GnomenuServerPrivate));
	gobject_class->dispose = gnomenu_server_dispose;
	gobject_class->finalize = gnomenu_server_finalize;

	klass->client_new = gnomenu_server_client_new;
	klass->client_destroy = gnomenu_server_client_destroy;
	klass->client_size_request = gnomenu_server_client_size_request;

/**
 * GnomenuServer::client-new:
 * @client_info: the client info.
 *
 * ::client-new signal is emitted when:
 * 	 server receives a  message indicates a new client is born;
 * 	 and server finished initializing internal data structures for the new client.
 */
	klass->signals[GMS_SIGNAL_CLIENT_NEW] = 
		g_signal_new ("client-new",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerClass, client_new),
		NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);
/**
 * GnomenuServer::client-destroy:
 * @client_info: the client info.
 *
 * ::client-destroy signal is emitted when:
 * 	 server receives a  message indicates a client is die;
 * 	 and before server disposing internal data structures for the dead client.
 */
	klass->signals[GMS_SIGNAL_CLIENT_DESTROY] = 
		g_signal_new ("client-destroy",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerClass, client_destroy),
		NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);
/**
 * GnomenuServer::client-size-request:
 * @client_info: the client info.
 * @requisition: the size requisition.
 *
 * ::client-size-request signal is emitted when:
 * 	 server receives a  message indicates a client request a size allocation;
 * 	Typically this happens after a server sends a get-requisition message to
 * 	the client.
 */
	klass->signals[GMS_SIGNAL_CLIENT_SIZE_REQUEST] = 
		g_signal_new ("client-size-request",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerClass, client_size_request),
		NULL, NULL,
			gnomenu_marshall_VOID__POINTER_POINTER,
			G_TYPE_NONE,
			2,
			G_TYPE_POINTER,
			G_TYPE_POINTER
			);
}

static void
gnomenu_server_init(GnomenuServer * self){
	LOG_FUNC_NAME;
}

/**
 * gnomenu_server_new:
 * 
 * create a new menu server object
 */
GnomenuServer * 
gnomenu_server_new(){
	GnomenuServer * self;
	GnomenuServerPrivate * priv;
	LOG_FUNC_NAME;
	self = g_object_new(GNOMENU_TYPE_SERVER, NULL);
	priv = GNOMENU_SERVER_GET_PRIVATE(self);
	self->socket = gdk_socket_new(GNOMENU_SERVER_NAME);
	self->clients = NULL;
	g_signal_connect(self->socket, "data-arrival", G_CALLBACK(gnomenu_server_data_arrival_cb), self);
	priv->disposed = FALSE;
}

static void gnomenu_server_dispose(GObject * object){
	GnomenuServer *self;
	GnomenuServerPrivate * priv;
	LOG_FUNC_NAME;
	self = GNOMENU_SERVER(object);
	priv = GNOMENU_SERVER_GET_PRIVATE(self);
	
	if(! priv->disposed){
		g_object_unref(self->socket);
		priv->disposed = TRUE;
	}
	G_OBJECT_CLASS(gnomenu_server_parent_class)->dispose(object);
}

static void gnomenu_server_finalize(GObject * object){
	GnomenuServer *self;
	LOG_FUNC_NAME;
	self = GNOMENU_SERVER(object);
	g_list_foreach(self->clients, g_free, NULL);
	g_list_free(self->clients);
	G_OBJECT_CLASS(gnomenu_server_parent_class)->finalize(object);
}

/** gnomenu_server_data_arrival_cb:
 *
 * 	callback when ::socket receives data
 */
static void gnomenu_server_data_arrival_cb(GdkSocket * socket, 
		gpointer * data, gint bytes, GnomenuServer * server){
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	LOG_FUNC_NAME;
	g_assert(bytes >= sizeof(GnomenuMessage));
	//enumvalue = g_enum_get_value(G_ENUM_CLASS(g_type_class_peek_parent(GNOMENU_TYPE_MESSAGE_TYPE)), message->any.type);
	//g_message("message arrival: %s", enumvalue->value_name);

}
/* virtual functions for signal handling*/
static void gnomenu_server_client_new(GnomenuServer * self, GnomenuServerClientInfo * client_info){
	LOG_FUNC_NAME;
	self->clients = g_list_prepend(self->clients, client_info);
}
static void gnomenu_server_client_destroy(GnomenuServer * self, GnomenuServerClientInfo * client_info){
	LOG_FUNC_NAME;
	self->clients = g_list_remove_all(self->clients, client_info);
}
static void gnomenu_server_client_size_request(GnomenuServer * self, GnomenuServerClientInfo * client_info, GtkRequisition * requisition){
	LOG_FUNC_NAME;
	g_free(requisition);
}

