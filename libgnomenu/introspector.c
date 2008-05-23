#include <config.h>
#include <gtk/gtk.h>
#include "introspector.h"

#if ENABLE_TRACING >= 2
#define LOG(fmt, args...) g_message("<Introspect>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)
struct _Introspector {
	GString * blob;
	GQueue queue;
};
static void _introspector_visit_widget_properties(Introspector * spector, GtkWidget * widget);
static void _introspector_visit_widget(Introspector * spector, GtkWidget * widget);
static gchar * gtk_widget_get_id(GtkWidget * widget);
static gboolean _str_in_list(gchar * str_list[], gchar * str);

gchar * gtk_widget_introspect(GtkWidget * widget){
	gchar * rt;
	Introspector * spector = introspector_new();
	introspector_queue_widget(spector, widget);
	introspector_visit_all(spector);
	return introspector_destroy(spector, FALSE);
}

static gchar * gtk_widget_get_id(GtkWidget * widget){
	gchar * id = g_object_get_data(widget, "introspect-id");
	static int unique = 0;
	if(id == NULL){
		gchar * name = gtk_widget_get_name(widget);
		if(!name) {
			name = G_OBJECT_TYPE_NAME(widget);
		}
		id = g_strdup_printf("%s%d", name , unique++);
		g_object_set_data_full(widget, "introspect-id", id, g_free);
	}
	return id;
}
void introspector_queue_widget(Introspector * spector, GtkWidget * widget){
	g_queue_push_tail(&spector->queue, widget);
}
Introspector * introspector_new(){
	Introspector * spector = g_new0(Introspector, 1);
	spector->blob = g_string_new("");
	g_queue_init(&spector->queue);
	return  spector;
}
gchar * introspector_destroy(Introspector * spector, gboolean free_blob_string){
	char * rt = NULL;
	rt = g_string_free(spector->blob, free_blob_string);
	g_queue_clear(&spector->queue);
	g_free(spector);
	return rt;
}
void introspector_visit_all(Introspector * spector){
	while(g_queue_peek_head(&spector->queue)){
		GtkWidget * widget = g_queue_pop_head(&spector->queue);
		_introspector_visit_widget(spector, widget);
	}
	
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
		"submenu",
		"label",
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
		switch(prop_type){
			case G_TYPE_CHAR:
			case G_TYPE_UCHAR:
			case G_TYPE_BOOLEAN:
			case G_TYPE_INT:
			case G_TYPE_UINT:
			case G_TYPE_LONG:
			case G_TYPE_ULONG:
			case G_TYPE_INT64:
			case G_TYPE_UINT64:
			case G_TYPE_ENUM:
			case G_TYPE_FLAGS:
			case G_TYPE_FLOAT:
			case G_TYPE_DOUBLE:
			case G_TYPE_STRING:
			prop_value_content = g_strdup_value_contents(&prop_value);
			break;
			default:
			if(G_VALUE_HOLDS(&prop_value, GTK_TYPE_WIDGET)){
				GtkWidget * widget = g_value_get_object(&prop_value);
				if(widget){
				introspector_queue_widget(spector, widget);
				prop_value_content = g_strdup(gtk_widget_get_id(widget));
				}
			} else
			LOG("skipped property: %s", prop_name);
		}

		g_value_unset(&prop_value);
		if(prop_value_content){
			g_string_append_printf(spector->blob, "  <property type=\"%s\" name=\"%s\">\n", g_type_name(prop_type), prop_name);
			g_string_append_printf(spector->blob, "    %s", prop_value_content);
			g_string_append(spector->blob, "  </property>\n");
			g_free(prop_value_content);
		}
	}
	g_free(prop_params);
}

static void _introspector_gtk_container_visit_child(GtkWidget * child, Introspector * spector){
	introspector_queue_widget(spector, child);
	g_string_append_printf(spector->blob, "  <child id=\"%s\"/>\n", gtk_widget_get_id(child));
}
static void _introspector_visit_widget(Introspector * spector, GtkWidget * widget){
	gchar * class_name = G_OBJECT_TYPE_NAME(widget);
	gchar * widget_id = gtk_widget_get_id(widget);
	if(!GTK_IS_MENU_SHELL(widget) && !GTK_IS_MENU_ITEM(widget)
		&& !GTK_IS_LABEL(widget)
			){
		g_warning("can't introspect other than menu shell and menu item");
		return NULL;
	}
	g_string_append_printf(spector->blob, 
				"<object class = \"%s\" id = \"%s\">\n",
				class_name, widget_id);
	_introspector_visit_widget_properties(spector, widget);
	if(GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(widget, _introspector_gtk_container_visit_child, spector);
	}
	
	g_string_append(spector->blob, "</object>\n");

}

