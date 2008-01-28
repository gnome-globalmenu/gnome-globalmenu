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
static void gnomenu_server_data_arrival_cb(GdkSocket * socket, gpointer data, gint bytes, GnomenuServer * server);

G_DEFINE_TYPE (GnomenuServer, gnomenu_server, G_TYPE_OBJECT)

static void
gnomenu_server_class_init(GnomenuServerClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	/*FIXME: unref the type at class_finalize*/
	klass->type_gnomenu_message_type = g_type_class_ref(GNOMENU_TYPE_MESSAGE_TYPE); 

	g_type_class_add_private(gobject_class, sizeof (GnomenuServerPrivate));
	gobject_class->dispose = gnomenu_server_dispose;
	gobject_class->finalize = gnomenu_server_finalize;

	klass->client_new = gnomenu_server_client_new;
	klass->client_destroy = gnomenu_server_client_destroy;
	klass->client_size_request = gnomenu_server_client_size_request;

	klass->signals[GMS_SIGNAL_CLIENT_NEW] = 
/**
 * GnomenuServer::client-new:
 * @client_info: the client info.
 *
 * ::client-new signal is emitted when:
 * 	 server receives a  message indicates a new client is born;
 * 	 and server finished initializing internal data structures for the new client.
 */
		g_signal_new ("client-new",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerClass, client_new),
		NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);
	klass->signals[GMS_SIGNAL_CLIENT_DESTROY] = 
/**
 * GnomenuServer::client-destroy:
 * @client_info: the client info.
 *
 * ::client-destroy signal is emitted when:
 * 	 server receives a  message indicates a client is die;
 * 	 and before server disposing internal data structures for the dead client.
 */
		g_signal_new ("client-destroy",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerClass, client_destroy),
		NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);
	klass->signals[GMS_SIGNAL_SIZE_REQUEST] = 
/**
 * GnomenuServer::client-size-request:
 * @client_info: the client info.
 * @requisition: the size requisition.
 *
 * ::client-size-request signal is emitted when:
 * 	 server receives a  message indicates a client request a size allocation;
 * 	Typically this happens after a server sends a query-requisition message to
 * 	the client.
 */
		g_signal_new ("client-size-request",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
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

static void gnomenu_server_client_info_free(gpointer  p, gpointer data){
	g_free(p);
}
static void gnomenu_server_finalize(GObject * object){
	GnomenuServer *self;
	LOG_FUNC_NAME;
/**
 * FIXME: perhaps this is buggy
 */
	self = GNOMENU_SERVER(object);
	g_list_foreach(self->clients, gnomenu_server_client_info_free, NULL);
	g_list_free(self->clients);
	G_OBJECT_CLASS(gnomenu_server_parent_class)->finalize(object);
}

static gboolean gnomenu_server_client_info_compare_by_socket_id(
	GnomenuServerClientInfo * p1,
	GnomenuServerClientInfo * p2){
	g_message("compare");
	return !( p1 && p2 && p1->socket_id == p2->socket_id);
}
static GnomenuServerClientInfo * 
gnomenu_server_find_client_info_by_socket_id(
		GnomenuServer * self,
		GdkNativeWindow socket_id){

	GnomenuServerClientInfo ci_key;
	GList * found;
	ci_key.socket_id = socket_id;
	
	found = g_list_find_custom(self->clients, 
		&ci_key, gnomenu_server_client_info_compare_by_socket_id);
	if(found) return found->data;
	return NULL;
}
/** gnomenu_server_data_arrival_cb:
 *
 * 	callback, invoked when the embeded socket receives data
 */
static void gnomenu_server_data_arrival_cb(GdkSocket * socket, 
		gpointer data, gint bytes, GnomenuServer * server){
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	guint * signals = NULL;
	GnomenuServerClientInfo * ci = NULL;

	LOG_FUNC_NAME;
	g_assert(bytes >= sizeof(GnomenuMessage));
	/*initialize varialbes*/
	signals = GNOMENU_SERVER_GET_CLASS(server)->signals;
	ci = gnomenu_server_find_client_info_by_socket_id(server, message->client_destroy.socket_id);

	enumvalue = g_enum_get_value(
					GNOMENU_SERVER_GET_CLASS(server)->type_gnomenu_message_type, 
					message->any.type);
	g_message("message arrival: %s", enumvalue->value_name);

	switch(enumvalue->value){
		case GNOMENU_MSG_CLIENT_NEW:
			if(ci){
				g_warning("client already recorded, "
						"(silently) remove it and continue");
				server->clients = g_list_remove_all(server->clients, ci);
			}
			ci = g_new0(GnomenuServerClientInfo, 1);
			ci->socket_id = message->client_new.socket_id;
			ci->ui_window = message->client_new.ui_window;
			ci->parent_window = message->client_new.parent_window;
			g_signal_emit(G_OBJECT(server), 
					signals[GMS_SIGNAL_CLIENT_NEW],
					0,
					ci);
			break;
		case GNOMENU_MSG_CLIENT_DESTROY:
			if(!ci) {
				g_warning("client not found,"
					"(silently) ignore this message and continue");
				break;
			} 
			g_signal_emit(G_OBJECT(server), 
					signals[GMS_SIGNAL_CLIENT_DESTROY],
					0, ci);
			break;
		case GNOMENU_MSG_SIZE_REQUEST:
			if(!ci){
				g_warning("client not found,"
					"(silently) ignore this message and continue");
				break;
			} /*else*/
			{
				GtkRequisition * requisition = NULL;
				requisition = g_new0(GtkRequisition, 1);
				requisition->width = message->size_request.width;
				requisition->height = message->size_request.height;
				g_signal_emit(G_OBJECT(server),
						signals[GMS_SIGNAL_SIZE_REQUEST],
						0, ci, requisition);
			}
		break;
		default:
		g_warning("unknown message, ignore and continue");
	}
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

