#include <config.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include "serverhelper.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"
#include "messages.h"

#define SELF (GNOMENU_SERVER_HELPER(_self))
#define PRIV (GNOMENU_SERVER_HELPER_GET_PRIVATE(_self))
#define GET_OBJECT(_s, s, p) \
	GnomenuServerHelper * s = GNOMENU_SERVER_HELPER(_s); \
	GnomenuServerHelperPrivate * p = GNOMENU_SERVER_HELPER_GET_PRIVATE(_s);

#if ENABLE_TRACING >= 2
#define LOG(fmt, args...) g_message("<GnomenuServerHelper>::" fmt, ## args )
#else
#define LOG(fmt, args...) 
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

typedef struct _GnomenuServerHelperPrivate GnomenuServerHelperPrivate;

struct _GnomenuServerHelperPrivate {
	gboolean disposed;
};

enum {
	CLIENT_NEW,
	CLIENT_REALIZE,
	CLIENT_REPARENT,
	CLIENT_UNREALIZE,
	CLIENT_DESTROY,
	SIZE_REQUEST,
	CLIENT_PARENT_FOCUS,
	SIGNAL_MAX
};

#define GNOMENU_SERVER_HELPER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_SERVER_HELPER, GnomenuServerHelperPrivate))

static GObject* _constructor 			( GType type, guint n_construct_properties, 
										  GObjectConstructParam *construct_params);
static void _dispose 					( GObject * _self);
static void _finalize 					( GObject * _self);

/* signal closures */
static void _c_client_new 				( GnomenuServerHelper * _self, GnomenuClientInfo * ci);
static void _c_client_realize 			( GnomenuServerHelper * _self, GnomenuClientInfo * ci);
static void _c_client_reparent 			( GnomenuServerHelper * _self, GnomenuClientInfo * ci);
static void _c_client_unrealize 		( GnomenuServerHelper * _self, GnomenuClientInfo * ci);
static void _c_client_destroy 			( GnomenuServerHelper * _self, GnomenuClientInfo * ci);
static void _c_client_size_request 		( GnomenuServerHelper * _self, GnomenuClientInfo * ci);
static void _c_client_parent_focus		( GnomenuServerHelper * _self, GnomenuClientInfo * ci);

/* signal handlers */
static void _s_connect_req				( GnomenuServerHelper * _self, GnomenuSocketNativeID target);

static void _s_service_shutdown			( GnomenuServerHelper * _self, GnomenuSocket * service);
static void _s_service_data_arrival		( GnomenuServerHelper * _self, gpointer data, gint bytes, 
										  GnomenuSocket * service);
static void _s_service_connected		( GnomenuServerHelper * self, GnomenuSocketNativeID target, GnomenuSocket * service);
/* utilities */

static GnomenuClientInfo * 
_find_ci_by_service						( GnomenuServerHelper * _self, GnomenuSocket* socket);
static void _client_do_allocate_size	( GnomenuServerHelper * _self, GnomenuClientInfo * ci);

static guint class_signals[SIGNAL_MAX] = {0};

G_DEFINE_TYPE (GnomenuServerHelper, gnomenu_server_helper, GNOMENU_TYPE_SOCKET)

static void
gnomenu_server_helper_class_init(GnomenuServerHelperClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);

	g_type_class_add_private(gobject_class, sizeof (GnomenuServerHelperPrivate));
	gobject_class->constructor = _constructor;
	gobject_class->dispose = _dispose;
	gobject_class->finalize = _finalize;

	klass->client_new = _c_client_new;
	klass->client_realize = _c_client_realize;
	klass->client_reparent = _c_client_reparent;
	klass->client_unrealize = _c_client_unrealize;
	klass->client_destroy = _c_client_destroy;
	klass->client_size_request = _c_client_size_request;
	klass->client_parent_focus = _c_client_parent_focus;

	class_signals[CLIENT_NEW] = 
/**
 * GnomenuServerHelper::client-new:
 * @self: who emits the signal.
 * @client_info: the client info. #GnomenuClientInfo. However only the socket_id field is defined.
 *
 * ::client-new signal is emitted when:
 * 	 server receives a  message indicates a new client is born;
 * 	 and server finished initializing internal data structures for the new client.
 * When you receive the signal, the connection between server and client
 * is not established yet, though the sockets are ready. every packet you
 * send before the establishment will be buffered.
 */
		g_signal_new ("client-new",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerHelperClass, client_new),
		NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);

	class_signals[CLIENT_REALIZE] =
/**
 * GnomenuServerHelper::client-realize:
 * 	@self: self.
 * 	@client_info: the client info. the realized window is contained
 * 	in ui_window field.
 *
 * ::client-reailze signal is emitted when the client's window is realized.
 */
		g_signal_new ("client-realize",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerHelperClass, client_realize),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);

	class_signals[CLIENT_REPARENT] =
/**
 * GnomenuServerHelper::client-reparent:
 * 	@self: self.
 * 	@client_info: the client info.
 *
 * ::client-reparent signal is emitted when the client is reparented.
 */
		g_signal_new ("client-reparent",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerHelperClass, client_reparent),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);

	class_signals[CLIENT_UNREALIZE] = 
/**
 * GnomenuServerHelper::client-unrealize:
 * 	@self: self.
 * 	@client_info: the client info.
 *
 * ::client-unreailze signal is emitted when the client's window is realized.
 */
		g_signal_new ("client-unrealize",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerHelperClass, client_unrealize),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);

	class_signals[CLIENT_DESTROY] = 
/**
 * GnomenuServerHelper::client-destroy:
 * @self: the GnomenuServe who emits the signal.
 * @client_info: the client info. #GnomenuClientInfo
 *
 * ::client-destroy signal is emitted when:
 * the connection between a service and the client is dead.
 */
		g_signal_new ("client-destroy",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerHelperClass, client_destroy),
		NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);

	class_signals[SIZE_REQUEST] = 
/**
 * GnomenuServerHelper::size-request:
 * @self: the GnomenuServe who emits the signal.
 * @client_info: the client info. #GnomenuClientInfo
 *
 * #GNOMENU_MSG_SIZE_QUERY, \\
 * #GnomenuClientHelper::size-query, \\
 * #GNOMENU_MSG_SIZE_REQUEST, \\
 * #GnomenuServerHelper::size-request,
 * #GNOMENU_MSG_SIZE_ALLOCATE, \\
 * #GnomenuClientHelper::size-allocate, \\
 *
 * are chained up to finish a size allocation. See #GnomenuMessageSizeQuery for some other 
 * desciptions.
 */
		g_signal_new ("size-request",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerHelperClass, client_size_request),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER
			);

	class_signals[CLIENT_PARENT_FOCUS] = 
/**
 * GnomenuServerHelper::client-parent-focus:
 * @self: the GnomenuServe who emits the signal.
 * @client_info: the client info. #GnomenuClientInfo
 *
 */
		g_signal_new ("client-parent-focus",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuServerHelperClass, client_parent_focus),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER
			);
}


/**
 * gnomenu_server_helper_new:
 * 
 * create a new menu server object
 *
 * Returns: the created server helper object.
 */
GnomenuServerHelper * 
gnomenu_server_helper_new(){
	LOG_FUNC_NAME;
	GnomenuServerHelper * _self;
	_self = g_object_new(GNOMENU_TYPE_SERVER_HELPER, "name", GNOMENU_SERVER_NAME, "timeout", 5, NULL);
	return _self;
}
gboolean gnomenu_server_helper_start(GnomenuServerHelper * self){
	GnomenuMessage msg;
	gnomenu_socket_listen(GNOMENU_SOCKET(self));
	msg.any.type = GNOMENU_MSG_SERVER_NEW;
	msg.server_new.socket_id = gnomenu_socket_get_native(GNOMENU_SOCKET(self));
	gnomenu_socket_broadcast_by_name(GNOMENU_SOCKET(self), GNOMENU_CLIENT_NAME, &msg, sizeof(msg));
}
static void
gnomenu_server_helper_init(GnomenuServerHelper * self){
	self->clients = NULL;
}
/**
 * Construtors and Destructors
 */
static GObject* _constructor(
		GType type, guint n_construct_properties, GObjectConstructParam *construct_params){
	GObject *obj = ( *G_OBJECT_CLASS(gnomenu_server_helper_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);
	GET_OBJECT(obj, self, priv);

	priv->disposed = FALSE;
	g_signal_connect(G_OBJECT(self), "connect-request", G_CALLBACK(_s_connect_req), NULL);

	return obj;
}

static void _client_info_free(gpointer  p, gpointer data){
	GnomenuClientInfo * ci = p;
	gnomenu_socket_shutdown(ci->service);
	g_free(p);
}

static void _dispose(GObject * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	
	if(! priv->disposed){
		priv->disposed = TRUE;
		g_list_foreach(self->clients, _client_info_free, NULL);
		g_list_free(self->clients);
		self->clients = NULL;
	}
	G_OBJECT_CLASS(gnomenu_server_helper_parent_class)->dispose(_self);
}

static void _finalize(GObject * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	G_OBJECT_CLASS(gnomenu_server_helper_parent_class)->finalize(_self);
}
/**
 * _s_connect_req:
 *
 */
static void _s_connect_req(GnomenuServerHelper * _self,
		GnomenuSocketNativeID target){
	LOG_FUNC_NAME;
	GnomenuClientInfo * ci = NULL;
	GET_OBJECT(_self, self, priv);

	ci = g_new0(GnomenuClientInfo, 1);
	ci->stage = GNOMENU_CI_STAGE_NEW;
	ci->size_stage = GNOMENU_CI_STAGE_RESOLVED;
/*FIXME: signal not setup when data diagram is built. Might lose signals.*/
	ci->service = gnomenu_socket_accept(GNOMENU_SOCKET(self), target);
	self->clients = g_list_prepend(self->clients, ci);
	LOG("clients length = %d", g_list_length(self->clients));
	g_signal_connect_swapped(G_OBJECT(ci->service), "shutdown", G_CALLBACK(_s_service_shutdown), self);
	g_signal_connect_swapped(G_OBJECT(ci->service), "data-arrival", G_CALLBACK(_s_service_data_arrival), self);
	g_signal_connect_swapped(G_OBJECT(ci->service), "connected", G_CALLBACK(_s_service_connected), self);

	gnomenu_socket_start(ci->service);		
}
static void _s_service_connected(GnomenuServerHelper * self, GnomenuSocketNativeID target, GnomenuSocket * service){
	GnomenuClientInfo * ci = _find_ci_by_service(self, service);
	g_return_if_fail(ci);
	g_signal_emit(G_OBJECT(self), 
			class_signals[CLIENT_NEW],
			0,
			ci);
}
static void _s_service_shutdown(GnomenuServerHelper * _self, GnomenuSocket * service){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, self, priv);
	GnomenuClientInfo * ci;

	ci = _find_ci_by_service(self, service);
	g_return_if_fail(ci);

	g_signal_emit(G_OBJECT(self), class_signals[CLIENT_DESTROY], 0, ci);
}

/** 
 * _service_data_arrival:
 *
 * 	callback, invoked when the embeded socket for serving clients receives data
 */
static void _s_service_data_arrival(GnomenuServerHelper * _self, 
		gpointer data, gint bytes, GnomenuSocket * service){
	LOG_FUNC_NAME;
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	GnomenuClientInfo * ci = NULL;

	GET_OBJECT(_self, self, priv);

	//g_assert(bytes >= sizeof(GnomenuMessage));
	/*initialize varialbes*/
	ci = _find_ci_by_service(self, service);
	g_return_if_fail(ci != NULL);

	enumvalue = gnomenu_message_type_get_value(message->any.type);
	LOG("message arrival: %s", enumvalue->value_name);

	switch(enumvalue->value){
		case GNOMENU_MSG_CLIENT_REALIZE:
			if(ci->stage == GNOMENU_CI_STAGE_REALIZED){
				g_warning("already realized. forget about the old one."
				"");
				/*FIXME: Shall we Simulate an unrealize signal?*/
			}
			ci->ui_window = message->client_realize.ui_window;
			LOG("ci->ui_window: %p", ci->ui_window);
			ci->stage = GNOMENU_CI_STAGE_REALIZED;
			g_signal_emit(G_OBJECT(self),
					class_signals[CLIENT_REALIZE],
					0, ci);
			break;
		case GNOMENU_MSG_CLIENT_REPARENT:{
			GdkNativeWindow old_parent = ci->parent_window;
			ci->parent_window = message->client_reparent.parent_window;
			if(old_parent != ci->parent_window){
				g_signal_emit(G_OBJECT(self),
						class_signals[CLIENT_REPARENT],
						0, ci);
			}
			break;
			}
		case GNOMENU_MSG_CLIENT_UNREALIZE:
			if(ci->stage != GNOMENU_CI_STAGE_REALIZED){
				g_warning("unrealize a not realized client? ignore it");
				break;
			}
			ci->ui_window = 0;
			ci->stage = GNOMENU_CI_STAGE_UNREALIZED;
			g_signal_emit(G_OBJECT(self),
					class_signals[CLIENT_UNREALIZE],
					0, ci);
			break;
		case GNOMENU_MSG_SIZE_REQUEST:
			if(ci->size_stage != GNOMENU_CI_STAGE_QUERYING){
				LOG("this resize queue is issued by the client");
			}
			/*NOTE: here we set some sanity value for the allocation*/
			ci->allocation.width =
			ci->requisition.width = message->size_request.width;
			ci->allocation.height =
			ci->requisition.height = message->size_request.height;
			LOG("requisition: %d, %d", ci->requisition);
			ci->size_stage = GNOMENU_CI_STAGE_RESPONSED;
			g_signal_emit(G_OBJECT(self),
					class_signals[SIZE_REQUEST],
					0, ci);
		break;
		case GNOMENU_MSG_PARENT_FOCUS:
			g_signal_emit(G_OBJECT(self),
					class_signals[CLIENT_PARENT_FOCUS],
					0, ci);
		break;
		default:
		g_warning("unknown message, ignore and continue");
	}
}

/**
 * _client_compare_by_service:
 *
 * Return: TRUE if two service socket matches. FALSE if else.
 */
static gboolean _client_compare_by_socket(
	const GnomenuClientInfo * p1,
	const GnomenuClientInfo * p2){
	LOG("p1.socket ==%p .vs. p2.socket=%p", p1->service, p2->service);
	return !( p1 && p2 && p1->service == p2->service);
}
/**
 * _client_compare_by_parent_window:
 *
 * Return: TRUE if socket_id matches. FALSE if else.
 */
static gboolean _client_compare_by_parent_window(
	const GnomenuClientInfo * p1,
	const GnomenuClientInfo * p2){
	return !( p1 && p2 && p1->parent_window == p2->parent_window);
}

/*public methods*/
static GnomenuClientInfo * 
_find_ci_by_service( GnomenuServerHelper * _self, GnomenuSocket* socket){

	GnomenuClientInfo ci_key;
	GList * found;
	ci_key.service = socket;
	
	found = g_list_find_custom(_self->clients, 
		&ci_key, (GCompareFunc)_client_compare_by_socket);
	if(found) return found->data;
	return NULL;
}
/**
 * gnomenu_server_helper_find_client_by_parent_window:
 *	@_self: self
 *	@parent_window: parent_window
 *
 * Find a client by parent_window
 */
GnomenuClientInfo * 
gnomenu_server_helper_find_client_by_parent_window(
		GnomenuServerHelper * _self,
		GdkNativeWindow parent_window){

	GnomenuClientInfo ci_key;
	GList * found;
	ci_key.parent_window = parent_window;
	
	found = g_list_find_custom(_self->clients, 
		&ci_key, (GCompareFunc)_client_compare_by_parent_window);
	if(found) return found->data;
	return NULL;
}

/**
 * gnomenu_server_helper_is_client:
 * 	@_self: self,
 * 	@ci: client info pointer.
 *
 * 	Test if @ci is a client of @_self.
 *
 * 	Returns: TRUE if it is. FALSE elsewhere.
 */
gboolean gnomenu_server_helper_is_client(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	return g_list_find(_self->clients, ci) != NULL;
}

/**
 * gnomenu_server_helper_queue_resize:
 * 	@self: self,
 * 	@ci: the client.
 *
 * 	queue a resize chain to the given client.
 */
void
gnomenu_server_helper_queue_resize(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	g_return_if_fail(gnomenu_server_helper_is_client(_self, ci));
	msg.any.type = GNOMENU_MSG_SIZE_QUERY;
	ci->size_stage = GNOMENU_CI_STAGE_QUERYING;
	gnomenu_socket_send(ci->service, &msg, sizeof(msg.size_query));
}

/**
 * gnomenu_server_helper_set_orientation:
 * 	@self: self
 * 	@ci: client info
 * 	@ori: orientation
 *
 * 	set the orientation of a client
 */
void gnomenu_server_helper_set_orientation(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GnomenuOrientation ori){
	GnomenuMessage msg;
	LOG_FUNC_NAME;
	g_return_if_fail(gnomenu_server_helper_is_client(self, ci));
	msg.any.type = GNOMENU_MSG_ORIENTATION_CHANGE;
	msg.orientation_change.orientation = ori;
	gnomenu_socket_send(ci->service, &msg, sizeof(msg.orientation_change));
}

/**
 * gnomenu_server_helper_allocate_size:
 * 	@self: self,
 * 	@ci: client info
 * 	@allocation: allocation
 *
 * set the allocation. only width and height is defined.
 */
void gnomenu_server_helper_allocate_size(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GtkAllocation * allocation){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	g_return_if_fail(gnomenu_server_helper_is_client(self, ci));
	ci->allocation.width = allocation->width;
	ci->allocation.height = allocation->height;
	_client_do_allocate_size(self, ci);
}

/**
 * gnomenu_server_helper_set_position:
 * 	@self: self
 * 	@ci: client info
 * 	@position: new position
 *
 * set the position of the menubar within its parent 
 * only width and height is defined.
 * A possible vulnerability note: if the global menu is not 
 * detached it should ignore this message. 
 *
 * I don't see any reason to use it on a client.
 */
void gnomenu_server_helper_set_position(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GdkPoint * position){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	g_return_if_fail(gnomenu_server_helper_is_client(self, ci));
	msg.any.type = GNOMENU_MSG_POSITION_SET;
	msg.position_set.x = position->x;
	msg.position_set.y = position->y;
	ci->allocation.x = position->x;
	ci->allocation.y = position->y;
	gnomenu_socket_send(ci->service, &msg, sizeof(msg.position_set));
}
/**
 * gnomenu_server_helper_set_visibility:
 * 	@self: self 
 * 	@ci: client info
 * 	@vis: visibility
 *
 * set the visibility of the menubar
 * A possible vulnerability note: if the global menu is not 
 * detached it should ignore this message. 
 */
void gnomenu_server_helper_set_visibility(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			gboolean vis){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	g_return_if_fail(gnomenu_server_helper_is_client(self, ci));
	msg.any.type = GNOMENU_MSG_VISIBILITY_SET;
	msg.visibility_set.visibility = vis;
	gnomenu_socket_send(ci->service, &msg, sizeof(msg.visibility_set));
}
/**
 * gnomenu_server_helper_set_background:
 * 	@self: self,
 * 	@ci: client info
 * 	@color: color can be NULL,
 * 	@pixmap: pixmap can be NULL.
 *
 * set the background of the menubar
 * A possible vulnerability note: if the global menu is not 
 * detached it should ignore this message. 
 */
void gnomenu_server_helper_set_background(GnomenuServerHelper * self, GnomenuClientInfo * ci,
			GdkColor * color, GdkPixmap * pixmap){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	g_return_if_fail(gnomenu_server_helper_is_client(self, ci));
	if(color){
		msg.any.type = GNOMENU_MSG_BGCOLOR_SET;
		msg.bgcolor_set.red = color->red;
		msg.bgcolor_set.blue = color->blue;
		msg.bgcolor_set.green = color->green;
		gnomenu_socket_send(ci->service, &msg, sizeof(msg.bgcolor_set));
	} 
	if(pixmap){
		static GdkPixmap * buffer = NULL;
		gint w, h;
		GdkGC * gc;
		if(buffer) g_object_unref(buffer);
		gdk_drawable_get_size(pixmap, &w, &h);
		buffer = gdk_pixmap_new(pixmap, w, h, -1);
		gc = gdk_gc_new(pixmap);
		gdk_draw_drawable(buffer, gc, pixmap, 0, 0, 0, 0, w, h);
		msg.any.type = GNOMENU_MSG_BGPIXMAP_SET;
		msg.bgpixmap_set.pixmap = GDK_PIXMAP_XID(buffer);
		gnomenu_socket_send(ci->service, &msg, sizeof(msg.bgpixmap_set));
	}	
}
/* virtual functions for signal handling*/
static void 
_c_client_new(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	GnomenuMessage msg;

}
static void 
_c_client_realize(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	/*Do nothing */
}
static void 
_c_client_reparent(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	/*Do nothing */
}
static void 
_c_client_unrealize(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	/*Do nothing */
}
static void 
_c_client_destroy(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	_self->clients = g_list_remove_all(_self->clients, ci);
}
static void 
_c_client_size_request(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	_client_do_allocate_size(_self, ci);
}
static void 
_c_client_parent_focus(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	/*Do nothing */
}
static void 
_client_do_allocate_size(GnomenuServerHelper * _self, GnomenuClientInfo * ci){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SIZE_ALLOCATE;

	msg.size_allocate.width = ci->allocation.width;
	msg.size_allocate.height = ci->allocation.height;
	
	gnomenu_socket_send(ci->service, 
		&msg, sizeof(msg.size_allocate));
	ci->size_stage = GNOMENU_CI_STAGE_RESOLVED;

}
/*
vim:ts=4:sw=4
*/
