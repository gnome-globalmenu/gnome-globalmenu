#include <config.h>

#include "object.h"
#include "x11.h"
#include "gnomenu-marshall.h"
/*************
 * TODO:
 * 1 emit signals
 * 1 unbind meethod. shall be emulated by a keep alive detection.
 **************/
#define GNOMENU_OBJECT_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOMENU_TYPE_OBJECT, GnomenuObjectPrivate))

#define GET_OBJECT(_s, s, p) \
	GnomenuObject * s = GNOMENU_OBJECT(_s); \
	GnomenuObjectPrivate * p = GNOMENU_OBJECT_GET_PRIVATE(_s); \

#define GET_OBJECT_LOG(_s, s, p) \
	GET_OBJECT(_s, s, p) \
	LOG("<%s>", s->name);

#if ENABLE_TRACING >= 3
#define LOG(fmt, args...) g_message("<GnomenuObject>::%s:" fmt, __func__, ## args)
#else
#define LOG(fmt, args...)
#endif 

typedef struct {
	guint id;
	GMainLoop * ret_loop;
	gchar * buffer;
	gboolean done;
	guint timeout_func;
	GdkWindow * pxy_win;
} ReturnInfo;
typedef struct {
	gchar * name;
	GnomenuObjectMethod method;
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
} GnomenuObjectPrivate;

/* Properties */
enum {
	PROP_0,
	PROP_NAME,
};
/* Signals */
enum {
	NOOP,
	SIGNAL_MAX,
};
G_DEFINE_TYPE 					(GnomenuObject, gnomenu_object, G_TYPE_INITIALLY_UNOWNED);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) ;
static void 
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);

static gchar * _invoke ( GnomenuObject * object, const gchar * name, gchar * arg);
static gchar * _query ( GnomenuObject * object, const gchar * name, gchar * args);

static gulong class_signals[SIGNAL_MAX] 		= {0};

guint gnomenu_object_class_install_method(GnomenuObjectClass * klass, const gchar * name, GnomenuObjectMethod method, const gchar * fmt){
	MethodInfo * mi = g_new0(MethodInfo, 1);
	mi->name = name;
	mi->method = method;
	mi->fmt = fmt;
	g_hash_table_insert(klass->method_table, name, mi);
	LOG("install a method %s = %p", name, method);
	return method;
}

static void
gnomenu_object_class_init(GnomenuObjectClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec * pspec;

	g_type_class_add_private(gobject_class, sizeof (GnomenuObjectPrivate));

	gobject_class->constructor = _constructor;
	gobject_class->set_property = _set_property;

	klass->method_table = g_hash_table_new(g_str_hash, g_str_equal);

	g_object_class_install_property (gobject_class,
			PROP_NAME,
			g_param_spec_string ("name",
						"",
						"",
						"",
						G_PARAM_CONSTRUCT_ONLY| G_PARAM_WRITABLE)
			);
	gnomenu_object_class_install_method(klass, 
			"query", _query, NULL);
}
static void
gnomenu_object_init(GnomenuObject * object) {
	GET_OBJECT(object, self, priv);
	priv->disposed = FALSE;
	self->name = NULL;
	priv->proxies = NULL;
	g_queue_init(&priv->return_stack);
}
GnomenuObject *
gnomenu_object_new(gchar * name){
	return g_object_new(GNOMENU_TYPE_OBJECT, "name", name, NULL);
}
static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	GObject *object = (*G_OBJECT_CLASS(gnomenu_object_parent_class)->constructor)
						( type, n_construct_properties,
							construct_params);
	GET_OBJECT(object, self, priv);
	return object;	
}
static void 
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec){
	GET_OBJECT(object, self, priv);
	switch(property_id){
		case PROP_NAME:
			if(self->name) g_free(self->name);
			self->name = g_value_dup_string(value);
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
void gnomenu_object_emit(GnomenuObject * object, const gchar * name, const gchar * fmt, ...){
	GET_OBJECT(object, self, priv);
	gpointer arg = NULL;
	va_list va;

	if(fmt){
		va_start(va, fmt);
		arg = g_strdup_vprintf(fmt, va);
		va_end(va);
	} else arg = g_strdup("");
/*for each proxy, emit via IPC*/
	g_free(arg);
}
static gchar * _invoke_parse(GnomenuObject * object, gchar * command){
	gchar ** splitted = g_strsplit_set(command, " ", 3);
	gchar *ret;

	ret = _invoke(object, splitted[0], splitted[2]);
	g_strfreev(splitted);
	return ret;
}
static gchar * _invoke(GnomenuObject * object, const gchar * name, gchar * args){
	GnomenuObjectClass * klass = G_OBJECT_GET_CLASS(object);
	MethodInfo *mi = g_hash_table_lookup(klass->method_table, name);
	
	if(mi) {
		return (*mi->method)(object, name, args);
	} else {
		g_warning("method %s is not implemented", name);
		return NULL;
	}
}

static gchar * _query ( GnomenuObject * object, const gchar * name, gchar * args){
	gchar * rt;
	gchar **buffer;
	GnomenuObjectClass * klass = G_OBJECT_GET_CLASS(object);
	
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

