
#include <config.h>
#include "connection.h"
#include "x11.h"
#include "gnomenu-marshall.h"

/*************
 * TODO:
 * 1 accept signals
 * 2 signal:: destroy. Shall be emulated by a keep-alive detection;
 **************/
#define GNOMENU_CONNECTION_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_CONNECTION, GnomenuConnectionPrivate))

#define GET_OBJECT(_s, s, p) \
	GnomenuConnection * s = GNOMENU_CONNECTION(_s); \
	GnomenuConnectionPrivate * p = GNOMENU_CONNECTION_GET_PRIVATE(_s); \

#define GET_OBJECT_LOG(_s, s, p) \
	GET_OBJECT(_s, s, p) \
	LOG("<%s>", s->name);

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("<GnomenuConnection>::%s:" fmt, __func__, ## args)
#else
#define LOG(fmt, args...)
#endif 
typedef struct {
	guint id;
	GMainLoop * call_loop;
	GMainLoop * ret_loop;
	gchar * buffer;
	gboolean done;
	gboolean returned;
	gboolean is_void;
	guint timeout_func;
} CallInfo;

typedef struct {
	gboolean disposed;
	GdkWindow * pxy_win;
	GdkWindow * obj_win;
	GQueue call_stack; /*CallInfo*/
} GnomenuConnectionPrivate;

/* Properties */
enum {
	PROP_0,
	PROP_PATH,
};
/* Signals */
enum {
	SIGNAL,
	SIGNAL_MAX,
};
G_DEFINE_TYPE 					(GnomenuConnection, gnomenu_connection, G_TYPE_OBJECT);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) ;
static void 
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);
void _signal(GnomenuConnection * connection, const gchar * name, char * arg);

static GdkFilterReturn 
	_obj_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuConnection * connection);
static GdkFilterReturn 
	_pxy_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuConnection * connection);

static gulong class_signals[SIGNAL_MAX] 		= {0};

static void
gnomenu_connection_class_init(GnomenuConnectionClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec * pspec;

	g_type_class_add_private(gobject_class, sizeof (GnomenuConnectionPrivate));

	gobject_class->constructor = _constructor;
	gobject_class->set_property = _set_property;

	klass->signal = _signal;	

	class_signals[SIGNAL] =
		g_signal_new ("signal",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuConnectionClass, signal),
			NULL /* accumulator */,
			NULL /* accu_data */,
			gnomenu_marshall_VOID__UINT_POINTER,
			G_TYPE_NONE /* return_type */,
			2     /* n_params */,
			G_TYPE_UINT,
			G_TYPE_POINTER
			);

	g_object_class_install_property (gobject_class,
			PROP_PATH,
			g_param_spec_string ("path",
						"",
						"",
						"",
						G_PARAM_CONSTRUCT_ONLY|G_PARAM_WRITABLE)
			);
}
static void
gnomenu_connection_init(GnomenuConnection * connection) {
	GET_OBJECT(connection, self, priv);
	priv->disposed = FALSE;
	self->path = NULL;
	priv->obj_win = NULL;
	priv->pxy_win = NULL;
	g_queue_init(&priv->call_stack);
}
GnomenuConnection *
gnomenu_connection_new(gchar * path){
	return g_object_new(GNOMENU_TYPE_CONNECTION, "path", path, NULL);
}
static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	GObject *object = (*G_OBJECT_CLASS(gnomenu_connection_parent_class)->constructor)
						( type, n_construct_properties,
							construct_params);
	GET_OBJECT(object, self, priv);
	return object;	
}
static void 
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec){
	GET_OBJECT(object, self, priv);
	switch(property_id){
		case PROP_PATH:
			if(self->path) g_free(self->path);
			self->path = g_value_dup_string(value);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}
}
/*
 * pointer: bytes,
 * pointer: bytes,
 * NULL
 * */
static gboolean _request_timeout(CallInfo * ri){
	g_main_loop_quit(ri->call_loop);
	g_main_loop_quit(ri->ret_loop);
	ri->timeout_func = 0;
	return FALSE;
}
static guint find_available_id(GnomenuConnection * connection){
	GET_OBJECT(connection, self, priv);
	guint id = 0;
	gboolean found = FALSE;
	while(!found) {
		found = TRUE;
		g_queue_for(&priv->call_stack, CallInfo * ri, 
			if(id == ri->id) { id++; found = FALSE; break; } );
	}
	return id;
}
gchar * gnomenu_connection_invoke(GnomenuConnection * connection, const gchar * object, const gchar * name, const gchar * fmt,...){
	GET_OBJECT(connection, self, priv);
	va_list va;
	gchar *  arg = NULL;
	gchar * req = NULL;
	gchar * rt = NULL;
	guint id = find_available_id(connection);
	CallInfo * ri = g_new0(CallInfo, 1);

	if(fmt){
		va_start(va, fmt);
		arg = g_strdup_vprintf(fmt, va);
		va_end(va);
	} else arg = g_strdup("");
	
	req = g_strdup_printf("%d %s %s %s", id, object?object:"SERVICE", name, arg);
	g_free(arg);
/*Then invoke the method via IPC*/
	LOG("req = '%s'", req);
	ri->buffer = req;
	ri->id = id;
	ri->call_loop = g_main_loop_new(NULL, TRUE);
	ri->ret_loop = g_main_loop_new(NULL, TRUE);
	ri->done = FALSE;
	ri->returned = FALSE;
	ri->is_void = TRUE;
	g_queue_push_tail(&priv->call_stack, ri);
	ri->timeout_func = g_timeout_add_seconds(10, _request_timeout, ri);
	
	_set_native_buffer(GDK_WINDOW_XWINDOW(priv->pxy_win), _GNOMENU_METHOD_CALL, id, req, strlen(req)+1);
	if(g_main_loop_is_running(ri->call_loop)){
		GDK_THREADS_LEAVE();
		g_main_loop_run(ri->call_loop);
		GDK_THREADS_ENTER();
	}
	LOG("method-call buffer set");
	GnomenuXMessage msg;
	msg.id = id;
	msg.type = _XMESSAGE_TYPE_CALL;
	msg.source = GDK_WINDOW_XWINDOW(priv->pxy_win);
	_send_xclient_message ( GDK_WINDOW_XWINDOW(priv->obj_win), &msg, sizeof(msg));
	if(g_main_loop_is_running(ri->ret_loop)){
		GDK_THREADS_LEAVE();
		g_main_loop_run(ri->ret_loop);
		GDK_THREADS_ENTER();
	}
	LOG("method returns");
	if(!ri->is_void){
		/* fetch the return value! */
		rt = _get_native_buffer(GDK_WINDOW_XWINDOW(priv->pxy_win), _GNOMENU_METHOD_RETURN, id, NULL, FALSE);
	} else {
		rt = NULL;
	}
	ri = g_queue_pop_tail(&priv->call_stack);
	if(ri->timeout_func) g_source_remove(ri->timeout_func);
	g_main_loop_unref(ri->call_loop);
	g_main_loop_unref(ri->ret_loop);
	g_free(ri);
	g_free(req);
	return rt;
}

void _signal(GnomenuConnection * connection, const gchar * name, gchar * arg){
	g_free(arg);
}

gboolean gnomenu_connection_connect(GnomenuConnection * connection){
	GET_OBJECT(connection, self, priv);
	GList * list_obj_win = _find_native_by_name (self->path);
	if(priv->obj_win) {
		return TRUE;
	}
	if(list_obj_win){
		GdkWindowAttr attr;
		GdkWindowAttributesType mask;

		priv->obj_win = gdk_window_foreign_new(list_obj_win->data);
		g_list_free(list_obj_win);

		attr.title = g_strdup_printf("%s.connection", self->path);
		attr.wclass = GDK_INPUT_ONLY;
		attr.window_type = GDK_WINDOW_TEMP;
		mask = GDK_WA_TITLE;

		priv->pxy_win = gdk_window_new(NULL, &attr, mask);
		g_assert(priv->pxy_win);
		gdk_window_add_filter(priv->obj_win, _obj_win_filter, self);
		gdk_window_add_filter(priv->pxy_win, _pxy_win_filter, self);
		gnomenu_connection_invoke(self, NULL, "Accept", "%d", GDK_WINDOW_XWINDOW(priv->pxy_win)); 
	} else {
		LOG("target window not found");
	}
	/*64BIT: use %lld instead!*/
	return priv->obj_win != NULL;
}
static GdkFilterReturn 
	_obj_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuConnection * connection){
	XEvent * xevent = gdkxevent;
	switch(xevent->type){
		case PropertyNotify:
		break;
		case ClientMessage:
			if(xevent->xclient.message_type != gdk_x11_atom_to_xatom(gdk_atom_intern(_GNOMENU_MESSAGE_TYPE, FALSE)))
				return GDK_FILTER_CONTINUE;
		break;
		default:
		return GDK_FILTER_CONTINUE;
	}
}
static GdkFilterReturn 
	_pxy_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuConnection * connection){
	XEvent * xevent = gdkxevent;
	switch(xevent->type){
		case PropertyNotify:
			if(xevent->xproperty.state == PropertyNewValue){
				const gchar * property_name = gdk_x11_get_xatom_name(xevent->xproperty.atom);
				guint id;
				gchar * buffer_name;
				if( x_prop_name_decode(property_name, &buffer_name, &id)
				 && g_str_equal(buffer_name, _GNOMENU_METHOD_CALL)){
					GET_OBJECT(connection, self, priv);
					LOG("decoded name = %s:%d", buffer_name, id);
					guint bytes;
					gchar * buffer = _get_native_buffer(GDK_WINDOW_XWINDOW(priv->pxy_win),
						_GNOMENU_METHOD_CALL, id, &bytes, FALSE);
					g_queue_for(&priv->call_stack, CallInfo * ri, 
						if(ri->id == id && g_str_equal(ri->buffer, buffer)){
							ri->done = TRUE;
							g_main_loop_quit(ri->call_loop);
							LOG("method-call buffer set: %s", ri->buffer);
						}
					);
				}
				g_free(buffer_name);
				/*no need to free property_name as indicated in gdk-doc*/
			}
			return GDK_FILTER_CONTINUE;
		break;
		case ClientMessage:
			if(xevent->xclient.message_type 
				!= gdk_x11_atom_to_xatom(gdk_atom_intern(_GNOMENU_MESSAGE_TYPE, FALSE)))
				return GDK_FILTER_CONTINUE;
			GnomenuXMessage msg = * (GnomenuXMessage *)&xevent->xclient.data;
			GnomenuXMessage msg_r;
			GET_OBJECT(connection, self, priv);
			switch(msg.type){
				case _XMESSAGE_TYPE_RETURN:
				case _XMESSAGE_TYPE_RETURN_VOID:
					/*TODO: invoke the method call _invoke*/
					/*TODO: set result buffer and monitor, move this return msg to _invoke*/
					LOG("received a return");
					g_queue_for(&priv->call_stack, CallInfo * ri, 
						if(ri->id == msg.id ){
							ri->returned = TRUE;
							if(msg.type == _XMESSAGE_TYPE_RETURN) ri->is_void = FALSE;
							else ri->is_void = TRUE;
							g_main_loop_quit(ri->ret_loop);
							LOG("method-call returned: %d", ri->id);
						}
					);
				break;
			}
		return GDK_FILTER_REMOVE;
		break;
		default:
		return GDK_FILTER_CONTINUE;
	}
}

GList * gnomenu_connection_query_methods(GnomenuConnection * connection){
	GList * rt = NULL;
	gchar * rt_buf = NULL;
	gchar ** lines = NULL;
	gchar ** words;
	GnomenuConnectionMethodInfo * gcmi;
	int i;
	rt_buf = gnomenu_connection_invoke(connection, NULL, "QueryMethods", NULL);
	lines = g_strsplit(rt_buf, "\n", 0);	
	g_free(rt_buf);
	for(i = 0; i < g_strv_length(lines); i++){
		gcmi = g_new0(GnomenuConnectionMethodInfo, 1);
		words = g_strsplit(lines[i], " ", 2);
		gcmi->name = words[0];
		gcmi->fmt = words[1];
		rt = g_list_append(rt, gcmi);
	}
	g_strfreev(lines);
	return rt;
}
