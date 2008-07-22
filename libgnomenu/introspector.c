#include <config.h>
#include <gtk/gtk.h>
#include "introspector.h"
#include "widget.h"

#if ENABLE_TRACING >= 2
#define LOG(fmt, args...) g_message("<Introspect>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)
struct _Introspector {
	GString * blob;
	GQueue queue;
	GString * prefix;
	IntrospectFlags flags;
};
static void _introspector_visit_widget_properties(Introspector * spector, GtkWidget * widget);
static void _introspector_visit_widget(Introspector * spector, GtkWidget * widget);
static void _dec_level(Introspector * spector){
	g_string_truncate(spector->prefix, spector->prefix->len-1);
}
static void _inc_level(Introspector * spector){
	g_string_append_c(spector->prefix, ' ');
}
static gboolean _str_in_list(gchar * str_list[], gchar * str);
static gchar * _ensure_widget_id(Introspector * spector, GtkWidget * widget){
	static int unique = 0;
	static GHashTable * ids = NULL;
	if(G_UNLIKELY(ids == NULL)) ids = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	const char * id = gtk_widget_get_id(widget);
	int i;
	char * new_id;
	gpointer value;
	if(id == NULL){
		gchar * name = gtk_widget_get_name(widget);
		if(!name) {
			name = G_OBJECT_TYPE_NAME(widget);
		}
		id = g_strdup_printf("%s", name);
	} else {
		id = g_strdup(id);
	}
	new_id = g_strdup(id);	
	i = 0;
	while(value = g_hash_table_lookup(ids, new_id)){ /*tail it with an integer*/
		if(value == widget) break;
		g_free(new_id);
		new_id = g_strdup_printf("%s%d", id, i++);
	}
	g_free(id);
	/*reset even if this is the correct name for the widget*/
	gtk_widget_set_id(widget, new_id);
	g_hash_table_insert(ids, new_id, widget);
	return gtk_widget_get_id(widget);
}

void introspector_queue_widget(Introspector * spector, GtkWidget * widget){
	g_queue_push_tail(&spector->queue, widget);
}
Introspector * introspector_new(){
	Introspector * spector = g_new0(Introspector, 1);
	spector->blob = g_string_new("");
	spector->prefix = g_string_new("");
	g_queue_init(&spector->queue);
	return  spector;
}
void introspector_set_flags(Introspector * spector, IntrospectFlags flags){
	spector->flags = flags;
}
gchar * introspector_destroy(Introspector * spector, gboolean free_blob_string){
	char * rt = NULL;
	rt = g_string_free(spector->blob, free_blob_string);
	g_string_free(spector->prefix, TRUE);
	g_queue_clear(&spector->queue);
	g_free(spector);
	return rt;
}
void introspector_visit_all(Introspector * spector){
	g_string_append(spector->blob, "<root>\n");
	while(g_queue_peek_head(&spector->queue)){
		GtkWidget * widget = g_queue_pop_head(&spector->queue);
		_introspector_visit_widget(spector, widget);
	}
	g_string_append(spector->blob, "</root>\n");
	
}
static gboolean _str_in_list(gchar * str_list[], gchar * str){
	int i;
	for(i=0; str_list[i]; i++){
		if(g_str_equal(str, str_list[i])){
			return TRUE;
		}
	}
	return FALSE;
}
static void _introspector_visit_widget_properties(Introspector * spector, GtkWidget * widget){
	static gchar * useful_prop_list[] = {
		"visible",
		"use-markup",
		"use-underline",
		"submenu",
		"image",
		"label",
		"stock",
		"justify",
		"ellipsize",
		"icon-size",
		"pixel-size",
		"icon-name",
		"no-show-all",
		"show-arrow",
		"file",
		"xalign",
		"yalign",
		"xpad",
		"ypad",
		"draw-as-radio",
		"inconsistent",
		"active",
		NULL
	};
	gint n_props;
	GParamSpec ** prop_params = g_object_class_list_properties(G_OBJECT_GET_CLASS(widget), &n_props); 
	gint i;
	for(i = 0; i< n_props; i++){
		GValue prop_value = {0,};
		GType prop_type = G_PARAM_SPEC_VALUE_TYPE(prop_params[i]);
		const gchar * prop_name = g_param_spec_get_name(prop_params[i]);
		if(!_str_in_list(useful_prop_list, prop_name)) continue;
		gchar * prop_value_content = NULL;
		g_value_init(&prop_value, prop_type);
		g_object_get_property(widget, prop_name, &prop_value);
#define HANDLE_TYPE(type, format, type_lower) case G_TYPE_ ## type: \
		prop_value_content = g_strdup_printf("%" format, g_value_get_ ## type_lower(&prop_value));\
		break;

		switch(prop_type){
			HANDLE_TYPE(CHAR, "c", char);
			HANDLE_TYPE(UCHAR, "c", uchar);
			HANDLE_TYPE(BOOLEAN, "d", boolean);
			HANDLE_TYPE(INT, "d", int);
			HANDLE_TYPE(UINT, "d", uint);
			HANDLE_TYPE(LONG, "d", long);
			HANDLE_TYPE(ULONG, "d", ulong);
			HANDLE_TYPE(INT64, G_GINT64_FORMAT, int64);
			HANDLE_TYPE(UINT64, G_GUINT64_FORMAT, uint64);
			HANDLE_TYPE(ENUM, "d", enum);
			HANDLE_TYPE(FLAGS, "d", flags);
			HANDLE_TYPE(FLOAT, "f", float);
			HANDLE_TYPE(DOUBLE, "lf", double);
			case G_TYPE_STRING:
				prop_value_content = g_value_get_string(&prop_value);
				prop_value_content = prop_value_content?g_markup_escape_text(prop_value_content, -1):NULL;
			break;
			default:
			if(G_VALUE_HOLDS(&prop_value, GTK_TYPE_WIDGET)){
				GtkWidget * widget = g_value_get_object(&prop_value);
				if(widget){
				introspector_queue_widget(spector, widget);
				prop_value_content = g_strdup(_ensure_widget_id(spector, widget));
				}
			} else {
			//LOG("skipped property: %s", prop_name);
			}
		}

		g_value_unset(&prop_value);
		if(prop_value_content){
			
			g_string_append_printf(spector->blob, "%s <property name=\"%s\" type=\"%s\" value=\"%s\"/>\n", 
						spector->prefix->str,
						prop_name, g_type_name(prop_type), prop_value_content);
			g_free(prop_value_content);
		}
	}
	g_free(prop_params);

	if(GTK_IS_ACCEL_LABEL(widget)){
		"accel-string",
		g_string_append_printf(spector->blob, "%s <property name=\"%s\" type=\"%s\" value=\"%s\"/>\n", 
					spector->prefix->str,
					"accel-string", "gchararray",
				   	GTK_ACCEL_LABEL(widget)->accel_string);
	}
}

static void _introspector_gtk_container_visit_child(GtkWidget * child, Introspector * spector){
	/*work around GtkImageMenuItem's wrong children definition*/
	if(GTK_IS_IMAGE(child) && GTK_IS_IMAGE_MENU_ITEM(gtk_widget_get_parent(child))) return;

	_introspector_visit_widget(spector, child);
}

static void _introspector_visit_widget(Introspector * spector, GtkWidget * widget){
	gchar * class_name = G_OBJECT_TYPE_NAME(widget);
	gchar * widget_id = _ensure_widget_id(spector, widget);
	gchar * template;
	if(!GTK_IS_MENU_SHELL(widget) && !GTK_IS_MENU_ITEM(widget)
		&& !GTK_IS_LABEL(widget) && !GTK_IS_IMAGE(widget)
			){
		g_warning("can't introspect other than menu shell and menu item, :%s",
				G_OBJECT_TYPE_NAME(widget));
		return NULL;
	}
	_inc_level(spector);
	if(spector->flags & INTROSPECT_HANDLE)
		template = "%s<object type = \"%s\" id = \"%s\" handle = \"%p\">\n";
	else
		template = "%s<object type = \"%s\" id = \"%s\">\n";

	g_string_append_printf(spector->blob, 
				template,
				spector->prefix->str,
				class_name, widget_id, widget);

	_introspector_visit_widget_properties(spector, widget);
	if(GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(widget, _introspector_gtk_container_visit_child, spector);
	}
	g_string_append_printf(spector->blob, "%s</object>\n", spector->prefix->str);
	_dec_level(spector);
}

