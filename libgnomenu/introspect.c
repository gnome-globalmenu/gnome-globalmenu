#include <config.h>
#include <gtk/gtk.h>
#include "introspect.h"

#if ENABLE_TRACING >= 2
#define LOG(fmt, args...) g_message("<Introspect>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)
static void _introspect_child(GtkWidget * child,
		GString * blob){
	gchar * intro = gtk_widget_introspect(child);
	g_string_append(blob, intro);
	g_free(intro);
}
static void _introspect_properties(GtkWidget * widget,
		GString * blob){
	gint n_props;
	GParamSpec ** prop_params = g_object_class_list_properties(G_OBJECT_GET_CLASS(widget), &n_props); 
	gint i;
	for(i = 0; i< n_props; i++){
		GValue prop_value = {0,};
		GType prop_type = G_PARAM_SPEC_VALUE_TYPE(prop_params[i]);
		const gchar * prop_name = g_param_spec_get_name(prop_params[i]);
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
			g_value_init(&prop_value, prop_type);
			g_object_get_property(widget, prop_name, &prop_value);
			gchar * prop_value_content = g_strdup_value_contents(&prop_value);
			g_value_unset(&prop_value);
			g_string_append_printf(blob, "<property name=\"%s\">\n", prop_name);
			g_string_append(blob, prop_value_content);
			g_string_append(blob, "</property>\n");
			g_free(prop_value_content);
			break;
			default:
			LOG("skipped property: %s", prop_name);
		}
	}
	g_free(prop_params);
}
gchar * gtk_widget_introspect(GtkWidget * widget){
	gchar * class_name = G_OBJECT_CLASS_NAME(widget);
	gchar * widget_name = gtk_widget_get_name(widget);
	GString * blob = g_string_new("");

	g_string_append_printf(blob, 
				"<object class = \"%s\" id = \"%s\">\n",
				class_name, widget_name);

	_introspect_properties(widget, blob);

	if(GTK_IS_CONTAINER(widget))
		gtk_container_foreach(widget, _introspect_child, blob);

	g_string_append(blob, "</object>\n");

	return g_string_free(blob, FALSE);
}
