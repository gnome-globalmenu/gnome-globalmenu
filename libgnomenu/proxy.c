#include <config.h>
#include "proxy.h"
#include "x11.h"
#include "gnomenu-marshall.h"

#define GNOMENU_PROXY_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_PROXY, GnomenuProxyPrivate))

#define GET_OBJECT(_s, s, p) \
	GnomenuProxy * s = GNOMENU_PROXY(_s); \
	GnomenuProxyPrivate * p = GNOMENU_PROXY_GET_PRIVATE(_s); \

#define GET_OBJECT_LOG(_s, s, p) \
	GET_OBJECT(_s, s, p) \
	LOG("<%s>", s->name);

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("<GnomenuProxy>::%s:" fmt, __func__, ## args)
#else
#define LOG(fmt, args...)
#endif 
typedef struct {
	guint id;
	GMainLoop * call_loop;
	GMainLoop * ret_loop;
	gchar * buffer;
	gboolean done;
	guint timeout_func;
} RequestInfo;

typedef struct {
	gboolean disposed;
	GdkWindow * pxy_win;
	GdkWindow * obj_win;
	GQueue call_stack; /*RequestInfo*/
} GnomenuProxyPrivate;

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
G_DEFINE_TYPE 					(GnomenuProxy, gnomenu_proxy, G_TYPE_OBJECT);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) ;
static void 
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);
void _signal(GnomenuProxy * proxy, const gchar * name, char * arg);

static GdkFilterReturn 
	_obj_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuProxy * proxy);
static GdkFilterReturn 
	_pxy_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuProxy * proxy);

static gulong class_signals[SIGNAL_MAX] 		= {0};

static void
gnomenu_proxy_class_init(GnomenuProxyClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec * pspec;

	g_type_class_add_private(gobject_class, sizeof (GnomenuProxyPrivate));

	gobject_class->constructor = _constructor;
	gobject_class->set_property = _set_property;

	klass->signal = _signal;	

	class_signals[SIGNAL] =
		g_signal_new ("signal",
			G_TYPE_FROM_CLASS (klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (GnomenuProxyClass, signal),
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
gnomenu_proxy_init(GnomenuProxy * proxy) {
	GET_OBJECT(proxy, self, priv);
	priv->disposed = FALSE;
	self->path = NULL;
	priv->obj_win = NULL;
	priv->pxy_win = NULL;
	g_queue_init(&priv->call_stack);
}
GnomenuProxy *
gnomenu_proxy_new(gchar * path){
	return g_object_new(GNOMENU_TYPE_PROXY, "path", path, NULL);
}
static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	GObject *object = (*G_OBJECT_CLASS(gnomenu_proxy_parent_class)->constructor)
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
static gboolean _request_timeout(RequestInfo * ri){
	g_main_loop_quit(ri->call_loop);
	g_main_loop_quit(ri->ret_loop);
	ri->timeout_func = 0;
	return FALSE;
}
void gnomenu_proxy_invoke(GnomenuProxy * proxy, const gchar * name, const gchar * fmt,...){
	GET_OBJECT(proxy, self, priv);
	static guint id = 0;
	gchar *  arg = NULL;
	gchar * req = NULL;
	RequestInfo * ri = g_new0(RequestInfo, 1);
	va_list va;
	va_start(va, fmt);
	arg = g_strdup_vprintf(fmt, va);
	va_end(va);
	req = g_strdup_printf("%s\t%d\t%s", name, id, arg);
	g_free(arg);
/*Then invoke the method via IPC*/
	LOG("req = '%s'", req);
	ri->buffer = req;
	ri->id = id++;
	ri->call_loop = g_main_loop_new(NULL, TRUE);
	ri->ret_loop = g_main_loop_new(NULL, TRUE);
	ri->done = FALSE;
	g_queue_push_tail(&priv->call_stack, ri);
	ri->timeout_func = g_timeout_add_seconds(10, _request_timeout, ri);
	_set_native_buffer(GDK_WINDOW_XWINDOW(priv->pxy_win), _GNOMENU_METHOD_CALL, req, strlen(req)+1);
	if(g_main_loop_is_running(ri->call_loop)){
		GDK_THREADS_LEAVE();
		g_main_loop_run(ri->call_loop);
		GDK_THREADS_ENTER();
	}
	LOG("method resolved");
	ri = g_queue_pop_tail(&priv->call_stack);
	if(ri->timeout_func) g_source_remove(ri->timeout_func);
	g_main_loop_unref(ri->call_loop);
	g_main_loop_unref(ri->ret_loop);
	g_free(ri);
	g_free(req);
}

void _signal(GnomenuProxy * proxy, const gchar * name, gchar * arg){
	g_free(arg);
}

gboolean gnomenu_proxy_bind(GnomenuProxy * proxy){
	GET_OBJECT(proxy, self, priv);
	GList * list_obj_win = _find_native_by_name (self->path);
	if(priv->obj_win) {
		return TRUE;
	}
	if(list_obj_win){
		GdkWindowAttr attr;
		GdkWindowAttributesType mask;

		priv->obj_win = gdk_window_foreign_new(list_obj_win->data);
		g_list_free(list_obj_win);

		attr.title = g_strdup_printf("%s.proxy", self->path);
		attr.wclass = GDK_INPUT_ONLY;
		attr.window_type = GDK_WINDOW_TEMP;
		mask = GDK_WA_TITLE;

		priv->pxy_win = gdk_window_new(NULL, &attr, mask);
		g_assert(priv->pxy_win);
		gdk_window_add_filter(priv->obj_win, _obj_win_filter, self);
		gdk_window_add_filter(priv->pxy_win, _pxy_win_filter, self);
	} else {
		LOG("target window not found");
	}
	LOG("binded");
	return priv->obj_win != NULL;
}

static GdkFilterReturn 
	_obj_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuProxy * proxy){
	XEvent * xevent = gdkxevent;
	switch(xevent->type){
		case PropertyNotify:
		break;
		case ClientMessage:
			if(xevent->xclient.message_type != gdk_x11_atom_to_xatom(_GNOMENU_MESSAGE_TYPE))
				return GDK_FILTER_CONTINUE;
		break;
		default:
		return GDK_FILTER_CONTINUE;
	}
}
static GdkFilterReturn 
	_pxy_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuProxy * proxy){
	XEvent * xevent = gdkxevent;
	switch(xevent->type){
		case PropertyNotify:
			if(xevent->xproperty.atom == gdk_x11_atom_to_xatom(_GNOMENU_METHOD_CALL)){
				if(xevent->xproperty.state == PropertyNewValue){
					GET_OBJECT(proxy, self, priv);
					guint bytes;
					gchar * buffer = _get_native_buffer(GDK_WINDOW_XWINDOW(priv->pxy_win),
						_GNOMENU_METHOD_CALL, &bytes, FALSE);
					g_queue_for(&priv->call_stack, RequestInfo * ri, 
						if(g_str_equal(ri->buffer, buffer)){
							ri->done = TRUE;
							g_main_loop_quit(ri->call_loop);
							LOG("method done: %s", ri->buffer);
						}
					);
				}
			}
			return GDK_FILTER_CONTINUE;
		break;
		case ClientMessage:
			if(xevent->xclient.message_type != gdk_x11_atom_to_xatom(_GNOMENU_MESSAGE_TYPE))
				return GDK_FILTER_CONTINUE;
		break;
		default:
		return GDK_FILTER_CONTINUE;
	}
}


