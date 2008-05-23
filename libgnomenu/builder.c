#include <config.h>
#include <gtk/gtk.h>
#include "builder.h"
#include "widget.h"


struct _Builder {
	GMarkupParser parser1;
	GMarkupParser parser2;
	GHashTable * widgets;
	GtkWidget * current_widget;
};
#define SWITCH_STR(x) { const gchar * _str_ = x; if(FALSE) {
#define CASE_STR(value)  } else if(g_str_equal(_str_, value)) {
#define DEFAULT_STR  } else {
#define END_SWITCH_STR }}
  /* Called for open tags <foo bar="baz"> */
static void _start_element1  (GMarkupParseContext *context,
                          const gchar         *element_name,
                          const gchar        **attribute_names,
                          const gchar        **attribute_values,
                          Builder *             builder,
                          GError             **error) {
	int i;
	SWITCH_STR(element_name)
		CASE_STR("object") {
			const char * type = NULL;
			const char * id = NULL;
			GtkWidget * new_widget;
			GType gtype = 0;
			for(i=0; attribute_names[i]; i++){
				SWITCH_STR(attribute_names[i])
					CASE_STR("type") type = attribute_values[i];
					CASE_STR("id") id = attribute_values[i];
				END_SWITCH_STR;
			}
			gtype = g_type_from_name(type);
			new_widget = g_object_new(gtype, NULL);
			gtk_widget_set_id(new_widget, id);

			if(builder->current_widget) {
				gtk_container_add(builder->current_widget, new_widget);
			} else {
				g_object_ref_sink(new_widget); /*this widget is the root*/
			}
			/*go downward*/
			builder->current_widget = new_widget;
		}
		CASE_STR("property") {
		}
		DEFAULT_STR {
			g_warning("unknown element: %s\n", element_name);
		}
	END_SWITCH_STR;
}

  /* Called for close tags </foo> */
static void _end_element1    (GMarkupParseContext *context,
                          const gchar         *element_name,
                          Builder *             builder,
                          GError             **error) {
	SWITCH_STR(element_name)
		CASE_STR("object") {
			GtkWidget * widget = builder->current_widget;
			g_hash_table_insert(builder->widgets, gtk_widget_get_id(widget), widget);
			/* go upward to the parent*/
			builder->current_widget = gtk_widget_get_parent(widget);
		}
		CASE_STR("property") {
		}
		DEFAULT_STR {
			g_warning("unknown element: %s\n", element_name);
		}
	END_SWITCH_STR;

}
  /* Called for open tags <foo bar="baz"> */
static void _start_element2  (GMarkupParseContext *context,
                          const gchar         *element_name,
                          const gchar        **attribute_names,
                          const gchar        **attribute_values,
                          Builder *             builder,
                          GError             **error) {
	int i;
	SWITCH_STR(element_name)
		CASE_STR("object") {
			const char * type = NULL;
			const char * id = NULL;
			for(i=0; attribute_names[i]; i++){
				SWITCH_STR(attribute_names[i])
					CASE_STR("type") type = attribute_values[i];
					CASE_STR("id") id = attribute_values[i];
				END_SWITCH_STR;
			}
			builder->current_widget = g_hash_table_lookup(builder->widgets, id);
			g_message("current_widget switched: %s", id);
		}
		CASE_STR("property") {
			GType gtype = 0;
			GValue gvalue = {0};
			GParamSpec * pspec;
			const char * type = NULL;
			const char * name = NULL;
			const char * value = NULL;
			GError * error = NULL;
			for(i=0; attribute_names[i]; i++){
				SWITCH_STR(attribute_names[i])
					CASE_STR("type") type = attribute_values[i];
					CASE_STR("name") name = attribute_values[i];
					CASE_STR("value") value = attribute_values[i];
				END_SWITCH_STR;
			}

			pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(builder->current_widget),
									name);
			/*FIXME: no type check yet*/
			if(pspec) {
				gtype = G_PARAM_SPEC_VALUE_TYPE(pspec);
				if(gtype == g_type_from_name(type)){
					if(g_type_is_a(gtype, GTK_TYPE_WIDGET)){
						g_object_set(builder->current_widget, name, 
								g_hash_table_lookup(builder->widgets, value), NULL);
					} else {
						gtk_builder_value_from_string(NULL, pspec, value, &gvalue, &error);
						g_object_set_property(builder->current_widget, name,
										&gvalue);
						g_value_unset(&gvalue);
					}
				} else {
					g_message("property types mismatch");
				
				}
			}
			else 
				g_message("no such property %s", name);

		}
		DEFAULT_STR {
			g_warning("unknown element: %s\n", element_name);
		}
	END_SWITCH_STR;
}

  /* Called for close tags </foo> */
static void _end_element2    (GMarkupParseContext *context,
                          const gchar         *element_name,
                          Builder *             builder,
                          GError             **error) {
	SWITCH_STR(element_name)
		CASE_STR("object") {
		}
		CASE_STR("property") {
		}
		DEFAULT_STR {
			g_warning("unknown element: %s\n", element_name);
		}
	END_SWITCH_STR;

}

  /* Called for character data */
  /* text is not nul-terminated */
static void _text           (GMarkupParseContext *context,
                          const gchar         *text,
                          gsize                text_len,  
                          Builder *             builder,
                          GError             **error) {
	return;
}

  /* Called for strings that should be re-saved verbatim in this same
 *    * position, but are not otherwise interpretable.  At the moment
 *       * this includes comments and processing instructions.
 *          */
  /* text is not nul-terminated. */
static void _passthrough    (GMarkupParseContext *context,
                          const gchar         *passthrough_text,
                          gsize                text_len,  
                          Builder *             builder,
                          GError             **error) {
	return;
}

  /* Called on error, including one set by other
 *    * methods in the vtable. The GError should not be freed.
 *       */
static void _error          (GMarkupParseContext *context,
                          GError              *error,
                          Builder *             builder){
	return;
}

Builder * builder_new (){
	Builder * builder = g_new0(Builder, 1);
	builder->parser1.start_element = _start_element1;
	builder->parser1.end_element = _end_element1;
	builder->parser1.text = _text;
	builder->parser1.passthrough = _passthrough;
	builder->parser1.error = _error;
	builder->parser2.start_element = _start_element2;
	builder->parser2.end_element = _end_element2;
	builder->parser2.text = _text;
	builder->parser2.passthrough = _passthrough;
	builder->parser2.error = _error;

	builder->widgets = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref);
	return builder;
}
void builder_parse(Builder * builder, const gchar * string){
	GError * error = NULL;
	GMarkupParseContext * context;
	context = g_markup_parse_context_new(&builder->parser1, G_MARKUP_TREAT_CDATA_AS_TEXT, builder, NULL);
	g_assert(context);
	g_markup_parse_context_parse(context, string, strlen(string), &error);
	if(error) goto error;
	g_markup_parse_context_end_parse(context, &error);
	if(error) goto error;
	g_markup_parse_context_free(context);
		
	context = g_markup_parse_context_new(&builder->parser2, G_MARKUP_TREAT_CDATA_AS_TEXT, builder, NULL);
	g_assert(context);
	g_markup_parse_context_parse(context, string, strlen(string), &error);
	if(error) goto error;
	g_markup_parse_context_end_parse(context, &error);
	if(error) goto error;
	g_markup_parse_context_free(context);
	return;
error:
	g_message("error_occured %s", error->message);
	return ;
}
GtkWidget * builder_get_object(Builder * builder, const gchar * id){
	return g_hash_table_lookup(builder->widgets, id);
}
Builder * builder_destroy(Builder * builder) {
	g_hash_table_destroy(builder->widgets);
	g_free(builder);
}
