#include <config.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include "clienthelper.h"
#include "gnomenu-marshall.h"
#include "gnomenu-enums.h"
#include "messages.h"
#include <string.h>

#define GNOMENU_CLIENT_HELPER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_CLIENT_HELPER, GnomenuClientHelperPrivate))

#define SELF (GNOMENU_CLIENT_HELPER(_self))
#define PRIV (GNOMENU_CLIENT_HELPER_GET_PRIVATE(_self))
#define GET_OBJECT(_s, s, p) \
	GnomenuClientHelper * s = GNOMENU_CLIENT_HELPER(_s); \
	GnomenuClientHelperPrivate * p = GNOMENU_CLIENT_HELPER_GET_PRIVATE(_s);
	
#if ENABLE_TRACING >= 2
#define LOG(fmt, args...) g_message("<GnomenuClientHelper>::" fmt, ## args)
#else
#define LOG(fmt, args...) 
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

typedef struct _GnomenuClientHelperPrivate GnomenuClientHelperPrivate;

struct _GnomenuClientHelperPrivate {
	gboolean disposed;
};

/* GObject interface */
static GObject * _constructor 		( GType type, guint n_construct_properties, 
									  GObjectConstructParam *construct_params );
static void _dispose				( GObject * _self );
static void _finalize				( GObject * _self );

/* Default Signal Closures */
static void _c_server_new 			( GnomenuClientHelper * _self );
static void _c_server_destroy 		( GnomenuClientHelper * _self );
static void _c_size_allocate			( GnomenuClientHelper * _self, GtkAllocation * allocation );
static void _c_size_query 			( GnomenuClientHelper * _self, GtkRequisition * req );
static void _c_orientation_change 	( GnomenuClientHelper * _self, GtkOrientation ori );
static void _c_position_set 			( GnomenuClientHelper * _self, GdkPoint * pos );
static void _c_visibility_set			( GnomenuClientHelper * _self, gboolean vis );
static void _c_background_set				( GnomenuClientHelper * _self, 
									GdkColor * color, GdkPixmap * pixmap);

/* Signal Handlers */
static void _s_data_arrival 			( GnomenuSocket * _self, gpointer data, gint bytes, gpointer userdata);
static void _s_connected 				( GnomenuSocket * _self, GnomenuSocketNativeID target);
static void _s_shutdown				( GnomenuSocket * _self);

G_DEFINE_TYPE (GnomenuClientHelper, gnomenu_client_helper, GNOMENU_TYPE_SOCKET)

enum { /*< private >*/
	SERVER_NEW,
	SERVER_DESTROY,
	SIZE_ALLOCATE,
	SIZE_QUERY,
	ORIENTATION_CHANGE,
	POSITION_SET,
	VISIBILITY_SET,
	BACKGROUND_SET,

	SIGNAL_MAX
};
static guint class_signals[SIGNAL_MAX] = {0};

static void 
gnomenu_client_helper_init			( GnomenuClientHelper * _self );


static void
gnomenu_client_helper_class_init(GnomenuClientHelperClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	g_type_class_add_private(gobject_class, sizeof (GnomenuClientHelperPrivate));
	gobject_class->constructor = _constructor;
	gobject_class->dispose = _dispose;
	gobject_class->finalize = _finalize;
	
	klass->server_new = _c_server_new;
	klass->server_destroy = _c_server_destroy;
	klass->size_allocate = _c_size_allocate;
	klass->size_query = _c_size_query;
	klass->orientation_change = _c_orientation_change;
	klass->position_set = _c_position_set;
	klass->visibility_set = _c_visibility_set;
	klass->background_set = _c_background_set;

	class_signals[SERVER_NEW] =
/**
 * GnomenuClientHelper::server-new:
 * @self: the #GnomenuClientHelper who emits this signal.
 * 
 * emitted when the client receives a server's creation announcement. The 
 * anouncement, as the name indicates, is a broadcast message to every
 * #GnomenuSocket with a name #GNOMENU_CLIENT_NAME. 
 *
 * It is the responsibility of the true client who listens to this signal
 * to reset its internal state, getting ready, and notify the server its
 * most recent state (eg, is it realized? who is its parent?)
 *
 * Note that the true client don't have a choice whether or not connect
 * to the server. This bahavior is not good and may change in the future.
 * If it changes, this signal shall be invoked with some parameter or
 * return value.
 */
		g_signal_new("server-new",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, server_new),
			NULL, NULL,
			gnomenu_marshall_VOID__VOID,
			G_TYPE_NONE,
			0);

	class_signals[SERVER_DESTROY] =
/**
 * GnomenuClientHelper::server-destroy:
 *	@self: the #GnomenuClientHelper who emits this signal.
 *  
 * emitted when the connection between the client and the server is dead.
 *
 * 	Deprecated: Use GnomenuSocket::shutdown instead. 
 * 	GnomenuClientHelper::server-destory is
 * 	emitted when it receives a GnomenuSocket::shutdown signal.
 */
		g_signal_new("server-destroy",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, server_destroy),
			NULL, NULL,
			gnomenu_marshall_VOID__VOID,
			G_TYPE_NONE,
			0);

	class_signals[SIZE_ALLOCATE] =
/**
 * GnomenuClientHelper::size-allocate:
 * 	@self: the #GnomenuClientHelper who emits this signal.
 * 	@allocation: Don't free it and don't rely on it. 
 * 			It is disposed when the signal ends.
 *
 * emitted when client receives size allocation from server. Only the
 * width and height field is meaningful.
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
 * 	@self: the #GnomenuClientHelper who emits this signal.
 * 	@req: the requisition the client need to fill in.
 *
 * emitted when the client receives a server's request for querying 
 * its size requisition. then the default handler will send the filled
 * in requisition to the server.
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
 *	@self: self
 *	@orientation: the new orientation. #GtkOrientation
 *
 * Implemented but the bahavior is not defined. 
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

	class_signals[POSITION_SET] =
/**
 * GnomenuClientHelper::position-set:
 * 	@self: self
 * 	@allocation: the allocation.
 *
 * only the x and y field of @allocation is defined.
*/
		g_signal_new("position-set",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, position_set),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER,
			G_TYPE_NONE,
			1,
			G_TYPE_POINTER);

	class_signals[VISIBILITY_SET] =
/**
 * GnomenuClientHelper::visibility-set:
 *
 * notifies that the server request for a visibility change.(show/hide);
*/
		g_signal_new("visibility-set",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, visibility_set),
			NULL, NULL,
			gnomenu_marshall_VOID__UINT,
			G_TYPE_NONE,
			1,
			G_TYPE_UINT);

	class_signals[BACKGROUND_SET] =
/**
 * GnomenuClientHelper::background-set:
 *  @self: self,
 *  @color: color, NOT allocated. You have to call #gdk_colormap_alloc_color yourself, or NULL,
 *  @pixmap: pixmap, or NULL, if you want to keep it, ref it.
 *  @userdata: userdata;
 */
		g_signal_new("background-set",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuClientHelperClass, background_set),
			NULL, NULL,
			gnomenu_marshall_VOID__POINTER_POINTER,
			G_TYPE_NONE,
			2,
			G_TYPE_POINTER,
			G_TYPE_POINTER);
}

static void
gnomenu_client_helper_init(GnomenuClientHelper * _self){
}

/**
 * gnomenu_client_helper_new:
 *
 * create a new menu client helper object
 * Returns: the created client.
 **/ 
GnomenuClientHelper * 
gnomenu_client_helper_new(){
	return g_object_new(GNOMENU_TYPE_CLIENT_HELPER, "name", GNOMENU_CLIENT_NAME, "timeout", 5, NULL);
}

static GObject* _constructor(GType type, guint n_construct_properties,
		GObjectConstructParam *construct_params){
	GObject * _self = (*G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);

	GET_OBJECT(_self, self, priv);

	priv->disposed = FALSE;
	g_signal_connect(G_OBJECT(self), "data-arrival", G_CALLBACK(_s_data_arrival), NULL);
	g_signal_connect(G_OBJECT(self), "connected", G_CALLBACK(_s_connected), NULL);
	g_signal_connect(G_OBJECT(self), "shutdown", G_CALLBACK(_s_shutdown), NULL);
/* try to connect to the server */
	gnomenu_socket_connect_by_name(self, GNOMENU_SERVER_NAME);

	return _self;
}
/**
 * _dispose:
 *  @object:
 *
 *  dispose the object. will shutdown the connection if it is connected
 */
static void _dispose(GObject * _self){
	GET_OBJECT(_self, self, priv);
	GnomenuSocket * socket = GNOMENU_SOCKET(self);
	LOG_FUNC_NAME;
	if(! priv->disposed){
		priv->disposed = TRUE;
		if(socket->status == GNOMENU_SOCKET_CONNECTED){
			gnomenu_socket_shutdown(socket);
		}
	}
	G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->dispose(_self);
}

static void _finalize(GObject * _self){
	LOG_FUNC_NAME;
	G_OBJECT_CLASS(gnomenu_client_helper_parent_class)->finalize(_self);
}

static void _s_shutdown(GnomenuSocket * _self){
	LOG_FUNC_NAME;
	g_signal_emit(G_OBJECT(_self),
		class_signals[SERVER_DESTROY],
		0);

}
static void _s_connected(GnomenuSocket * _self, GnomenuSocketNativeID target){
	LOG_FUNC_NAME;
	g_signal_emit(G_OBJECT(_self),
		class_signals[SERVER_NEW],
		0);
}
/** gnomenu_client_helper_data_arrival_cb:
 *
 * 	callback, invoked when the embeded socket receives data
 */
static void _s_data_arrival(GnomenuSocket * _self, 
		gpointer data, gint bytes, gpointer userdata){
	GnomenuMessage * message = data;
	GEnumValue * enumvalue = NULL;
	GET_OBJECT(_self, self, priv);

	LOG_FUNC_NAME;

//	g_assert(bytes >= sizeof(GnomenuMessage));
	
	enumvalue = gnomenu_message_type_get_value(message->any.type);
	LOG("message arrival: %s", enumvalue->value_name);
	/*TODO: Dispatch the message and emit signals*/
	switch(enumvalue->value){
		case GNOMENU_MSG_SERVER_NEW:
			gnomenu_socket_connect(self, message->server_new.socket_id);
		break;
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
		case GNOMENU_MSG_POSITION_SET:
			{
				GdkPoint * pt = g_new0(GdkPoint, 1);
				pt->x = message->position_set.x;
				pt->y = message->position_set.y;
				g_signal_emit(G_OBJECT(self),
					class_signals[POSITION_SET],
					0, pt);
			}
		break;
		case GNOMENU_MSG_VISIBILITY_SET:
			{
				g_signal_emit(G_OBJECT(self),
					class_signals[VISIBILITY_SET],
					0, message->visibility_set.visibility);
			}
		break;	
		case GNOMENU_MSG_BGCOLOR_SET:
			{
				GdkColor * color = g_new0(GdkColor, 1);
				color->red = message->bgcolor_set.red;	
				color->green = message->bgcolor_set.green;	
				color->blue = message->bgcolor_set.blue;	
				g_signal_emit(G_OBJECT(self),
					class_signals[BACKGROUND_SET],
					0, color, NULL);
			}
		break;
		case GNOMENU_MSG_BGPIXMAP_SET:
			{
				if(message->bgpixmap_set.pixmap){
				gint width, height;
			
				GdkPixmap * pixmap;

				GdkPixmap * dup;
				GdkGC * gc;
				gdk_x11_display_grab(gdk_display_get_default());
				pixmap = gdk_pixmap_lookup(message->bgpixmap_set.pixmap);
				if(!pixmap){
					gdk_error_trap_push();
					pixmap = gdk_pixmap_foreign_new(message->bgpixmap_set.pixmap);
					if(pixmap){
					gdk_drawable_set_colormap(pixmap, gdk_colormap_get_system()); /*maybe shall GtkGlobalMenuBar oset the colormap?*/
					gdk_drawable_get_size(pixmap, &width, &height);
					LOG("drawable size = %d, %d", width, height);
					LOG("drawable depth = %d", gdk_drawable_get_depth(pixmap));
					}else{
						LOG("can't find pixmap");
					}
					gdk_flush();
					if(gdk_error_trap_pop()){
						pixmap = NULL;
						LOG("meet an x error");
					}
				}
				if(pixmap){
					dup = gdk_pixmap_new(pixmap, width, height, -1);
					gc = gdk_gc_new(dup);
					gdk_draw_drawable(dup, gc, pixmap, 0, 0, 0, 0, width, height);
					gdk_display_sync(gdk_display_get_default());
					g_object_unref(pixmap);
					g_object_unref(gc);
					g_signal_emit(G_OBJECT(self),
						class_signals[BACKGROUND_SET],
						0, NULL, dup);
					}
				}
				gdk_x11_display_ungrab(gdk_display_get_default());
			}
		break;
		default:
			g_warning("unknown message, ignore it and continue");
		break;
	}
}

static void 
_c_server_new(GnomenuClientHelper * _self){
	LOG_FUNC_NAME;
}
static void 
_c_server_destroy(GnomenuClientHelper * _self){
	LOG_FUNC_NAME;
}
static void 
_c_size_allocate(GnomenuClientHelper * _self, GtkAllocation * allocation){
	LOG_FUNC_NAME;
	/*other signal handlers will deal with the allocation*/
	/*At the cleanup stage, we can safely free allocation*/
	g_free(allocation);
}
static void
_c_size_query(GnomenuClientHelper * _self, GtkRequisition * req){
	gnomenu_client_helper_request_size(_self, req);
	g_free(req);
}

static void 
_c_orientation_change
			(GnomenuClientHelper * _self, GtkOrientation ori){
	LOG_FUNC_NAME;
}
static void 
_c_position_set
			(GnomenuClientHelper * _self, GdkPoint * pt){
	LOG_FUNC_NAME;
	LOG("x = %d, y = %d", pt->x, pt->y);
	g_free(pt);
}
static void 
_c_visibility_set
			(GnomenuClientHelper * _self, gboolean vis){
	LOG_FUNC_NAME;
}
static void
_c_background_set
			(GnomenuClientHelper * _self, GdkColor * color, GdkPixmap * pixmap){
	LOG_FUNC_NAME;
	if(color)
		g_free(color);
	if(pixmap)
		g_object_unref(pixmap);
}

/**
 * gnomenu_client_helper_send_realize:
 * 	@_self: self;
 * 	@ui_window: the realized window which the client want the server to know
 * 		about. server will (possibily, depends on the implementation) 
 * 		grab this window so be careful.
 *
 * notify a #GnomenuServerHelper that the menu client's owner widget 
 * has been realized .
 */
void gnomenu_client_helper_send_realize(GnomenuClientHelper * _self, 
		GdkWindow * ui_window){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_REALIZE;
	msg.client_realize.ui_window = GDK_WINDOW_XWINDOW(ui_window);
	gnomenu_socket_send(GNOMENU_SOCKET(_self),
		&msg, sizeof(msg.client_realize));
}
/**
 * gnomenu_client_helper_send_reparent:
 * 	@_self: self;
 * 	@parent_window: the realized new parent toplevel gdk window 
 * 		of the menu bar which the client want the server to know
 * 		about. 
 *
 * notify a #GnomenuServerHelper that the menu client's parent window
 * exists.
 */
void gnomenu_client_helper_send_reparent(GnomenuClientHelper * _self, 
		GdkWindow * parent_window){
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_CLIENT_REPARENT;
	msg.client_reparent.parent_window = GDK_WINDOW_XWINDOW(parent_window);	
	gnomenu_socket_send(GNOMENU_SOCKET(_self),
		&msg, sizeof(msg.client_reparent));
}
/**
 * gnomenu_client_helper_send_unrealize:
 * 	@_self: self;
 *
 * notify a #GnomenuServerHelper that the menu client is unrealized.
 */
void gnomenu_client_helper_send_unrealize(GnomenuClientHelper * _self){
	GnomenuMessage msg; msg.any.type = GNOMENU_MSG_CLIENT_UNREALIZE;
	gnomenu_socket_send(GNOMENU_SOCKET(_self),
		&msg, sizeof(msg.client_unrealize));
}
/**
 * gnomenu_client_helper_request_size:
 * @_self: self;
 * @req: requisition;
 *
 * Request size from the server.
 */
void gnomenu_client_helper_request_size(GnomenuClientHelper * _self, GtkRequisition * req){
	LOG_FUNC_NAME;
	GnomenuMessage msg;
	msg.any.type = GNOMENU_MSG_SIZE_REQUEST;
	msg.size_request.width = req->width;
	msg.size_request.height = req->height;
	gnomenu_socket_send(GNOMENU_SOCKET(_self), &msg, sizeof(msg.size_request));
}
/*
vim:ts=4:sw=4
*/
