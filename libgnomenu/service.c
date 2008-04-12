#include <config.h>

#include "service.h"
#include "x11.h"
#include "gnomenu-marshall.h"
/*************
 * TODO:
 * 1 emit signals
 * 1 unbind meethod. shall be emulated by a keep alive detection.
 **************/
#define GNOMENU_SERVICE_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_SERVICE, GnomenuServicePrivate))

#define GET_OBJECT(_s, s, p) \
	GnomenuService * s = GNOMENU_SERVICE(_s); \
	GnomenuServicePrivate * p = GNOMENU_SERVICE_GET_PRIVATE(_s); \

#define GET_OBJECT_LOG(_s, s, p) \
	GET_OBJECT(_s, s, p) \
	LOG("<%s>", s->name);

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("<GnomenuService>::%s:" fmt, __func__, ## args)
#else
#define LOG(fmt, args...)
#endif 

typedef struct {
	guint id;
	GMainLoop * ret_loop;
	const gchar * buffer;
	gboolean done;
	guint timeout_func;
	GdkWindow * pxy_win;
} ReturnInfo;
typedef struct {
	const gchar * name;
	GnomenuServiceMethod method;
	const gchar * fmt;
} MethodInfo;
typedef struct {
	GdkWindow * pxy_win;
} ProxyInfo;
typedef struct {
	gboolean disposed;
	GdkWindow * obj_win;
	GList * proxies; /*ProxyInfo*/
	GQueue return_stack; /*ReturnInfo*/
} GnomenuServicePrivate;

/* Properties */
enum {
	PROP_0,
	PROP_PATH,
};
/* Signals */
enum {
	NOOP,
	SIGNAL_MAX,
};
G_DEFINE_TYPE 					(GnomenuService, gnomenu_service, G_TYPE_OBJECT);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) ;
static void 
_set_property( GObject * service, guint property_id, const GValue * value, GParamSpec * pspec);

static gchar * _invoke ( GnomenuService * service, const gchar * object, const gchar * name, gchar * arg);
static gchar * _query_methods ( GnomenuService * service, const gchar * name, gchar * args);
static gchar * _accept ( GnomenuService * service, const gchar * name, gchar * args);

static GdkFilterReturn
	_obj_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuService * service);
static GdkFilterReturn
	_pxy_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuService * service);
static gulong class_signals[SIGNAL_MAX] 		= {0};

static guint gnomenu_service_class_install_method(GnomenuServiceClass * klass, const gchar * name, GnomenuServiceMethod method, const gchar * fmt){
	MethodInfo * mi = g_new0(MethodInfo, 1);
	mi->name = name;
	mi->method = method;
	mi->fmt = fmt;
	g_hash_table_insert(klass->method_table, (gchar *) name, mi);
	LOG("install a method %s = %p", name, method);
	return (guint) method;
}

static void
gnomenu_service_class_init(GnomenuServiceClass * klass){
	GObjectClass * gservice_class = G_OBJECT_CLASS(klass);
	GParamSpec * pspec;

	g_type_class_add_private(gservice_class, sizeof (GnomenuServicePrivate));

	gservice_class->constructor = _constructor;
	gservice_class->set_property = _set_property;

	klass->method_table = g_hash_table_new(g_str_hash, g_str_equal);

	g_object_class_install_property (gservice_class,
			PROP_PATH,
			g_param_spec_string ("path",
						"",
						"",
						"",
						G_PARAM_CONSTRUCT_ONLY| G_PARAM_WRITABLE)
			);
	gnomenu_service_class_install_method(klass, 
			"QueryMethods", _query_methods, NULL);
	gnomenu_service_class_install_method(klass, 
			"Accept", _accept, "%d");
}
static void
gnomenu_service_init(GnomenuService * service) {
	GET_OBJECT(service, self, priv);
	priv->disposed = FALSE;
	self->path = NULL;
	priv->proxies = NULL;
	g_queue_init(&priv->return_stack);
}
GnomenuService *
gnomenu_service_new(gchar * path){
	return g_object_new(GNOMENU_TYPE_SERVICE, "path", path, NULL);
}
static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	GObject *service = (*G_OBJECT_CLASS(gnomenu_service_parent_class)->constructor)
						( type, n_construct_properties,
							construct_params);
	GET_OBJECT(service, self, priv);
	return service;	
}
static void 
_set_property( GObject * service, guint property_id, const GValue * value, GParamSpec * pspec){
	GET_OBJECT(service, self, priv);
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
/*
void gnomenu_service_emit(GnomenuService * service, const gchar * name, const gchar * fmt, ...){
	GET_OBJECT(service, self, priv);
	gpointer arg = NULL;
	va_list va;

	if(fmt){
		va_start(va, fmt);
		arg = g_strdup_vprintf(fmt, va);
		va_end(va);
	} else arg = g_strdup("");
//for each proxy, emit via IPC
	g_free(arg);
}
*/
gchar * _invoke_parse(GnomenuService * service, gchar * command){
	gchar ** splitted = g_strsplit_set(command, " ", 4);
	gchar * rt = _invoke(service, splitted[1], splitted[2], splitted[3]);
	g_strfreev(splitted);
	return rt;
}
gchar * _invoke(GnomenuService * service, const char * object, const gchar * name, gchar * args){
	if(g_str_equal(object, "SERVICE")) {	
		GnomenuServiceClass * klass = GNOMENU_SERVICE_GET_CLASS(service);
		MethodInfo *mi = g_hash_table_lookup(klass->method_table, name);
		if(mi) {
			return (*mi->method)(service, name, args);
		} else {
			g_warning("method %s is not implemented", name);
			return NULL;
		}
	} else {
		LOG("invoking object method not implemented yet");
		return NULL;
	}
}
void gnomenu_service_expose(GnomenuService * service){
	GET_OBJECT(service, self, priv);
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;
	attr.title = self->path;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;
	mask = GDK_WA_TITLE;

	priv->obj_win = gdk_window_new(NULL, &attr, mask);
	gdk_window_add_filter(priv->obj_win, _obj_win_filter, self);
}

static gchar * _query_methods ( GnomenuService * service, const gchar * name, gchar * args){
	gchar * rt;
	gchar **buffer;
	GnomenuServiceClass * klass = GNOMENU_SERVICE_GET_CLASS(service);
	
	GList * list = g_hash_table_get_values(klass->method_table);
	GList * node;
	gint i;
	buffer = g_new0(gpointer, g_list_length(list)+1);
	for(node = list, i=0; node; node = node->next, i++){
		MethodInfo * mi = node->data;
		LOG("method:%s %p (%s)", *mi);
		buffer[i] = g_strdup_printf("%s %s", mi->name, mi->fmt);
	}
	buffer[i] = NULL;
	rt = g_strjoinv("\n", buffer);
	g_list_free(list);
	g_strfreev(buffer);
		
	return rt;
}

static gchar * _accept ( GnomenuService * service, const gchar * name, gchar * args){
	GET_OBJECT(service, self, priv);
	ProxyInfo * pi = g_new0(ProxyInfo, 1);
	LOG("arg = %s", args);
	GdkNativeWindow w;
	sscanf(args, "%d", &w);
	pi->pxy_win = gdk_window_foreign_new(w);
	priv->proxies = g_list_append(priv->proxies, pi);
	g_assert(pi->pxy_win);
	LOG("binded by a new proxy at %d", w);
	gdk_window_set_events(pi->pxy_win, gdk_window_get_events(pi->pxy_win) |
				GDK_PROPERTY_CHANGE_MASK);
	gdk_window_add_filter(pi->pxy_win, _pxy_win_filter, service);
	return NULL;
}

static guint find_available_id(GnomenuService * service){
	GET_OBJECT(service, self, priv);
	guint id = 0;
	gboolean found = FALSE;
	while(!found) {
		found = TRUE;
/* shall be for the signals, because return_stack uses source id for id
		g_queue_for(&priv->return_stack, ReturnInfo * ri, 
			if(id == ri->id) { id++; found = FALSE; break; } );
*/
	}
	return id;
}
static ProxyInfo * _find_proxy_from_native(GnomenuService * service, GdkNativeWindow window){
	GET_OBJECT(service, self, priv);
	GList * node;
	for(node = priv->proxies; node; node=node->next){
		if(GDK_WINDOW_XWINDOW(((ProxyInfo*)(node->data))->pxy_win) == window)
			return node->data;
	}
	return NULL;
}
static gboolean _return_timeout(ReturnInfo * ri){
	g_main_loop_quit(ri->ret_loop);
	ri->timeout_func = 0;
	return FALSE;
}
static void _method_return(GnomenuService * service, GnomenuXMessage msg, const gchar * rt){
	GET_OBJECT(service, self, priv);
	LOG("_method return");
	GnomenuXMessage msg_r;
	
	if(rt == NULL) /*a void method*/ {

		msg_r.type = _XMESSAGE_TYPE_RETURN_VOID;
		msg_r.id = msg.id;
		msg_r.source = GDK_WINDOW_XWINDOW(priv->obj_win); /*set the source*/
		
		_send_xclient_message(/*GDK_WINDOW_XWINDOW(pi->pxy_win)*/ msg.source, &msg_r, sizeof(msg_r));
	} else {
		ProxyInfo * pi = _find_proxy_from_native(self, msg.source);
		ReturnInfo * ri = g_new0(ReturnInfo, 1);
		ri->id = msg.id;
		ri->ret_loop = g_main_loop_new(NULL, TRUE);
		ri->pxy_win = pi->pxy_win;
		ri->done = FALSE;
		ri->buffer = rt;
		ri->timeout_func = g_timeout_add_seconds(10, _return_timeout, ri);
		g_queue_push_tail(&priv->return_stack, ri);
		_set_native_buffer(msg.source, _GNOMENU_METHOD_RETURN, ri->id, rt, strlen(rt)+1);
		if(g_main_loop_is_running(ri->ret_loop)){
			GDK_THREADS_LEAVE();
			g_main_loop_run(ri->ret_loop);
			GDK_THREADS_ENTER();
		}
		LOG("after set buffer");
		ri = g_queue_pop_tail(&priv->return_stack);
		if(ri->timeout_func) g_source_remove(ri->timeout_func);
		g_main_loop_unref(ri->ret_loop);
		g_free(ri);
		msg_r.type = _XMESSAGE_TYPE_RETURN;
		msg_r.id = msg.id;
		msg_r.source = GDK_WINDOW_XWINDOW(priv->obj_win); /*set the source*/
		
		_send_xclient_message(/*GDK_WINDOW_XWINDOW(pi->pxy_win)*/ msg.source, &msg_r, sizeof(msg_r));
	}


}
static GdkFilterReturn 
	_obj_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuService * service){
	XEvent * xevent = gdkxevent;
	switch(xevent->type){
		case PropertyNotify:
		break;
		case ClientMessage:
			if(xevent->xclient.message_type != gdk_x11_atom_to_xatom(gdk_atom_intern(_GNOMENU_MESSAGE_TYPE, FALSE)))
				return GDK_FILTER_CONTINUE;
			GnomenuXMessage msg = * (GnomenuXMessage *)&xevent->xclient.data;
			GET_OBJECT(service, self, priv);
			switch(msg.type){
				case _XMESSAGE_TYPE_CALL:
					/*TODO: invoke the method call _invoke*/
					/*TODO: set result buffer and monitor, move this return msg to _invoke*/
					LOG("received a call");
					gchar * command = _get_native_buffer(msg.source, _GNOMENU_METHOD_CALL, msg.id, NULL, FALSE);
					gchar * rt = _invoke_parse(self, command);
					g_free(command);
					_method_return(self, msg, rt);
					if(rt) g_free(rt);
					
				break;
			}
		return GDK_FILTER_REMOVE;
		break;
		default:
		return GDK_FILTER_CONTINUE;
	}
}
static GdkFilterReturn 
	_pxy_win_filter (GdkXEvent* gdkxevent, GdkEvent * event, GnomenuService * service){
	LOG("pxy filter");
	XEvent * xevent = gdkxevent;
	switch(xevent->type){
		case PropertyNotify:
			if(xevent->xproperty.state == PropertyNewValue){
				const gchar * property_name = gdk_x11_get_xatom_name(xevent->xproperty.atom);
				guint id;
				gchar * buffer_name;
				LOG("%s", property_name);
				if( x_prop_name_decode(property_name, &buffer_name, &id)
				 && g_str_equal(buffer_name, _GNOMENU_METHOD_RETURN)){
					GET_OBJECT(service, self, priv);
					LOG("decoded name = %s:%d", buffer_name, id);
					gchar * buffer = _get_native_buffer(GDK_WINDOW_XWINDOW(event->any.window),
						_GNOMENU_METHOD_RETURN, id, NULL, FALSE);

					g_queue_for(&priv->return_stack, ReturnInfo * ri, 
						if(ri->pxy_win == event->any.window 
							&& ri->id == id && g_str_equal(ri->buffer, buffer)){
							ri->done = TRUE;
							g_main_loop_quit(ri->ret_loop);
							LOG("method-return buffer set: %s", ri->buffer);
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
		break;
		default:
		return GDK_FILTER_CONTINUE;
	}
}
