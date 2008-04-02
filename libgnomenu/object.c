#include <config.h>

#include "object.h"
#include "x11.h"
#include "gnomenu-marshall.h"

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
	gboolean disposed;
	GdkWindow * obj_win;
	GHashTable * method_table;
} GnomenuObjectPrivate;

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
G_DEFINE_TYPE 					(GnomenuObject, gnomenu_object, G_TYPE_OBJECT);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) ;
static void 
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);

static void _invoke ( GnomenuObject * object, const gchar * name, gchar * args, gchar * rt);
static gulong class_signals[SIGNAL_MAX] 		= {0};

static void
gnomenu_object_class_init(GnomenuObjectClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec * pspec;

	g_type_class_add_private(gobject_class, sizeof (GnomenuObjectPrivate));

	gobject_class->constructor = _constructor;
	gobject_class->set_property = _set_property;

	klass->invoke = _invoke;	

	g_object_class_install_property (gobject_class,
			PROP_PATH,
			g_param_spec_string ("path",
						"",
						"",
						"",
						G_PARAM_CONSTRUCT_ONLY| G_PARAM_WRITABLE)
			);
}
static void
gnomenu_object_init(GnomenuObject * object) {
	GET_OBJECT(object, self, priv);
	priv->disposed = FALSE;
	self->path = NULL;
	priv->method_table = g_hash_table_new(g_str_hash, g_direct_equal);
}
GnomenuObject *
gnomenu_object_new(gchar * path){
	return g_object_new(GNOMENU_TYPE_OBJECT, "path", path, NULL);
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
void gnomenu_object_emit(GnomenuObject * object, const gchar * name, const gchar * fmt, ...){
	GET_OBJECT(object, self, priv);
	gpointer arg = NULL;
	va_list va;
	va_start(va, fmt);
	arg = g_strdup_vprintf(fmt, va);
	va_end(va);
/*for each proxy, emit via IPC*/
	g_free(arg);
}

void _invoke(GnomenuObject * object, const gchar * name, gchar * args, gchar * rt){
	GET_OBJECT(object, self, priv);
	GnomenuObjectMethod method;
	method = g_hash_table_lookup(priv->method_table, name);
	if(method) {
		(*method)(object, name, args, rt);
	} else {
		g_warning("method %s is not implemented", name);
		rt = NULL;
	}
}
guint gnomenu_object_install_method(GnomenuObject * object, const gchar * name, GnomenuObjectMethod method){
	GET_OBJECT(object, self, priv);
	g_hash_table_insert(priv->method_table, name, method);
}
void gnomenu_object_expose(GnomenuObject * object){
	GET_OBJECT(object, self, priv);
	GdkWindowAttr attr;
	GdkWindowAttributesType mask;
	attr.title = self->path;
	attr.wclass = GDK_INPUT_ONLY;
	attr.window_type = GDK_WINDOW_TEMP;
	mask = GDK_WA_TITLE;

	priv->obj_win = gdk_window_new(NULL, &attr, mask);

}
