#include <config.h>

#include <glib.h>

typedef struct {
	GHashTable * results;
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
	if(g_str_equal(element_name, "results")) {
		gint i;
		pi->name = NULL;
		pi->current_prop = NULL;
		pi->current_prop_value = NULL;
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
	}

}
static void error  (GMarkupParseContext *context, 
		GError * error,
		ParseInfo *pi) {
	g_warning("parsing error: %s", error->message);
}

gboolean ipc_command_parse(const gchar * string, 
		gchar ** name, GHashTable ** parameters) {
	ParseInfo cpi;
	GMarkupParseContext * context;
	GError * error = NULL;
	context = g_markup_parse_context_new(&parser, 0, &cpi, NULL);
	if(g_markup_parse_context_parse(context, string, strlen(string), &error)){
		*name = cpi.name;
		*parameters = cpi.parameters;
		g_markup_parse_context_free(context);
		return TRUE;
	}
	if(error) {
		g_critical("Parse command failed: %s", error->message);
		g_error_free(error);
	}
	g_markup_parse_context_free(context);
	return FALSE;

}
gchar * ipc_command_to_string(gchar * name, GHashTable * parameters){
	GString * string = g_string_new("");
	name = g_markup_escape_text(name, -1);
	g_string_append_printf(string, "<command name=\"%s\">", name);
	g_free(name);
	if(parameters) {
		GList * keys = g_hash_table_get_keys(parameters);
		GList * key;
		for(key = keys; key; key = key->next) {
			gchar * para_name = g_markup_escape_text(key->data, -1);
			gchar * para_value = g_markup_escape_text(g_hash_table_lookup(parameters, key->data), -1);
			g_string_append_printf(string, "<p name=\"%s\">", para_name);
			g_string_append_printf(string, "%s",  para_value);
			g_string_append_printf(string, "</p>");
			g_free(para_name);
			g_free(para_value);
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
