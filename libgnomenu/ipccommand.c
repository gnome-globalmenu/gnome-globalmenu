#include <config.h>

#include <glib.h>

typedef struct {
	gchar * name;
	GHashTable * parameters;
	GHashTable * results;
	gchar * current_prop;
	GString * current_prop_value;
	gchar * current_result;
	GString * current_result_value;
} ParseInfo;
static void start_element  (GMarkupParseContext *context, 
		const gchar * element_name, 
		const gchar **attribute_names, 
		const gchar **attribute_values, 
		ParseInfo    *pi, 
		GError      **error);
static void end_element  (GMarkupParseContext *context, 
		const gchar * element_name, 
		ParseInfo    *pi, 
		GError      **error);
static void text  (GMarkupParseContext *context, 
		const gchar *text,
		gsize text_len,
		ParseInfo    *pi, 
		GError      **error);
static void error  (GMarkupParseContext *context, 
		GError * error,
		ParseInfo *pi);

static GMarkupParser parser = {
	start_element,
	end_element,
	text,
	NULL,
	error,
};
static void start_element  (GMarkupParseContext *context, 
		const gchar * element_name, 
		const gchar **attribute_names, 
		const gchar **attribute_values, 
		ParseInfo    *pi, 
		GError      **error) {
	if(g_str_equal(element_name, "command")) {
		gint i;
		pi->name = NULL;
		pi->current_prop = NULL;
		pi->current_prop_value = NULL;
		pi->current_result = NULL;
		pi->current_result_value = NULL;
		for(i=0; attribute_names[i]; i++) {
			if(g_str_equal(attribute_names[i], "name")){
				pi->name = g_strdup(attribute_values[i]);
				break;
			}
		}
		pi->parameters = g_hash_table_new_full(
				g_str_hash,
				g_str_equal,
				g_free,
				g_free);
		pi->results = g_hash_table_new_full(
				g_str_hash,
				g_str_equal,
				g_free,
				g_free);
	} else 
	if(g_str_equal(element_name, "p")){
		gint i;
		for(i=0; attribute_names[i]; i++) {
			if(g_str_equal(attribute_names[i], "name")){
				pi->current_prop = g_strdup(attribute_values[i]);
				pi->current_prop_value = g_string_new("");
				break;
			}
		}
	} else 
	if(g_str_equal(element_name, "r")){
		gint i;
		for(i=0; attribute_names[i]; i++) {
			if(g_str_equal(attribute_names[i], "name")){
				pi->current_result = g_strdup(attribute_values[i]);
				pi->current_result_value = g_string_new("");
				break;
			}
		}
	}

}
static void end_element  (GMarkupParseContext *context, 
		const gchar * element_name, 
		ParseInfo    *pi, 
		GError      **error) {
	if(g_str_equal(element_name, "p")) {
		g_hash_table_insert(pi->parameters,
				pi->current_prop,
				g_string_free(pi->current_prop_value, FALSE));
		pi->current_prop = NULL;
		pi->current_prop_value = NULL;
	} else 
	if(g_str_equal(element_name, "r")) {
		g_hash_table_insert(pi->results,
				pi->current_result,
				g_string_free(pi->current_result_value, FALSE));
		pi->current_result = NULL;
		pi->current_result_value = NULL;
	}
}
static void text  (GMarkupParseContext *context, 
		const gchar *text,
		gsize text_len,
		ParseInfo    *pi, 
		GError      **error) {
	if(pi->current_prop) {
		g_string_append_len(pi->current_prop_value,
			   text, text_len);
	} else 
	if(pi->current_result) {
		g_string_append_len(pi->current_result_value,
			   text, text_len);
	}

}
static void error  (GMarkupParseContext *context, 
		GError * error,
		ParseInfo *pi) {
	g_warning("parsing error: %s", error->message);
}

gboolean ipc_command_parse(const gchar * string, 
		gchar ** name, GHashTable ** parameters, GHashTable ** results) {
	ParseInfo cpi = {0};
	GMarkupParseContext * context;
	GError * error = NULL;
	context = g_markup_parse_context_new(&parser, 0, &cpi, NULL);
	if(g_markup_parse_context_parse(context, string, strlen(string), &error)){
		/** always return a table even if they are empty 
		if(g_hash_table_size(cpi.parameters) == 0) {
			g_hash_table_destroy(cpi.parameters);
			cpi.parameters = NULL;
		}
		if(g_hash_table_size(cpi.results) == 0) {
			g_hash_table_destroy(cpi.results);
			cpi.results = NULL;
		}
		*/
		if(name) {
			*name = cpi.name;
		} else {
			if(cpi.name)
				g_free(cpi.name);
		}
		if(parameters) {
			*parameters = cpi.parameters;
		} else {
			if(cpi.parameters)
				g_hash_table_destroy(cpi.parameters);
		}
		if(results) {
			*results = cpi.results;
		} else {
			if(cpi.results)
				g_hash_table_destroy(cpi.results);
		}
		g_markup_parse_context_free(context);
		return TRUE;
	}
	if(error) {
		if(cpi.name) g_free(cpi.name);
		if(cpi.parameters) g_hash_table_destroy(cpi.parameters);
		if(cpi.results) g_hash_table_destroy(cpi.results);
		g_critical("Parse command failed: %s", error->message);
		g_error_free(error);
	}
	g_markup_parse_context_free(context);
	return FALSE;

}
gchar * ipc_command_to_string(gchar * name, GHashTable * parameters, GHashTable * results){
	GString * string = g_string_new("");
	name = g_markup_escape_text(name, -1);
	gpointer key, value;
	g_string_append_printf(string, "<command name=\"%s\">", name);
	g_free(name);
	if(parameters) {
		GHashTableIter iter;
		g_hash_table_iter_init(&iter, parameters);
		while(g_hash_table_iter_next(&iter, &key, &value)){
			gchar * para_name = g_markup_escape_text(key, -1);
			gchar * para_value = g_markup_escape_text(value, -1);
			g_string_append_printf(string, "<p name=\"%s\">", para_name);
			g_string_append_printf(string, "%s",  para_value);
			g_string_append_printf(string, "</p>");
			g_free(para_name);
			g_free(para_value);
		}
	}
	if(results) {
		GHashTableIter iter;
		g_hash_table_iter_init(&iter, results);
		while(g_hash_table_iter_next(&iter, &key, &value)){
			gchar * result_name = g_markup_escape_text(key, -1);
			gchar * result_value = g_markup_escape_text(value, -1);
			g_string_append_printf(string, "<r name=\"%s\">", result_name);
			g_string_append_printf(string, "%s",  result_value);
			g_string_append_printf(string, "</r>");
			g_free(result_name);
			g_free(result_value);
		}
	
	}
	g_string_append_printf(string, "</command>");
	return g_string_free(string, FALSE);
}
GHashTable * ipc_parameters_va(gchar * name, va_list va) {
	GHashTable * rt = g_hash_table_new(g_str_hash, g_str_equal);
	gchar * value = va_arg(va, gchar *);
	do {
		g_hash_table_insert(rt, name, value);
	gchar * name = va_arg(va, gchar *);
	if(!name) break;
	gchar * value = va_arg(va, gchar *);
	} while(1);

	return rt;
}
GHashTable * ipc_results_va(gchar * name, va_list va) {
	return  ipc_parameters_va(name, va);
}
