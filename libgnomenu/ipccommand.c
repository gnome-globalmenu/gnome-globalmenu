#include <config.h>

#include <glib.h>
#include "ipccommand.h"

static GHashTable * build_hash_table_va(gchar * name, va_list va) {
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

typedef struct {
	gchar * name;
	gchar * cid;
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
		pi->cid = NULL;
		pi->current_prop = NULL;
		pi->current_prop_value = NULL;
		pi->current_result = NULL;
		pi->current_result_value = NULL;
		for(i=0; attribute_names[i]; i++) {
			if(g_str_equal(attribute_names[i], "name")){
				pi->name = g_strdup(attribute_values[i]);
			} else
			if(g_str_equal(attribute_names[i], "cid")){
				pi->cid = g_strdup(attribute_values[i]);
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

IPCCommand * ipc_command_parse(const gchar * string){
	ParseInfo cpi = {0};
	GMarkupParseContext * context;
	GError * error = NULL;
	context = g_markup_parse_context_new(&parser, 0, &cpi, NULL);
	if(g_markup_parse_context_parse(context, string, strlen(string), &error)){
		IPCCommand * command = ipc_command_new();
			command->cid = cpi.cid;
			command->name = cpi.name;
			command->parameters = cpi.parameters;
			command->results = cpi.results;
		g_markup_parse_context_free(context);
		return command;
	}
	if(error) {
		if(cpi.name) g_free(cpi.name);
		if(cpi.cid) g_free(cpi.cid);
		if(cpi.parameters) g_hash_table_destroy(cpi.parameters);
		if(cpi.results) g_hash_table_destroy(cpi.results);
		g_critical("Parse command failed: %s", error->message);
		g_error_free(error);
	}
	g_markup_parse_context_free(context);
	return NULL;

}
gchar * ipc_command_to_string(IPCCommand * command){
	GString * string = g_string_new("");
	gchar * name = g_markup_escape_text(command->name, -1);
	gchar * cid = g_markup_escape_text(command->cid, -1);
	gpointer key, value;
	g_string_append_printf(string, "<command name=\"%s\" cid=\"%s\">", name, cid);
	g_free(name);
	g_free(cid);
	if(command->parameters) {
		GHashTableIter iter;
		g_hash_table_iter_init(&iter, command->parameters);
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
	if(command->results) {
		GHashTableIter iter;
		g_hash_table_iter_init(&iter, command->results);
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
IPCCommand * ipc_command_new() {
	return g_slice_new0(IPCCommand);
}
void ipc_command_free(IPCCommand * command) {
	if(command->name) g_free(command->name);
	if(command->cid) g_free(command->cid);
	if(command->parameters) g_hash_table_destroy(command->parameters);
	if(command->results) g_hash_table_destroy(command->results);
	g_slice_free(IPCCommand, command);
}
void ipc_command_set_parameters_valist(IPCCommand * command, gchar * para_name, va_list va) {
	if(command->parameters) {
		g_hash_table_destroy(command->parameters);
	}
	command->parameters = build_hash_table_va(para_name, va);
}
void ipc_command_set_results_valist(IPCCommand * command, gchar * result_name, va_list va) {
	if(command->results) {
		g_hash_table_destroy(command->results);
	}
	command->results = build_hash_table_va(result_name, va);
}
void ipc_command_set_parameters(IPCCommand * command, gchar * para_name, ...) {
	va_list va;
	va_start(va, para_name);
	ipc_command_set_parameters_valist(command, para_name, va);
	va_end(va);
}
void ipc_command_set_results(IPCCommand * command, gchar * result_name, ...) {
	va_list va;
	va_start(va, result_name);
	ipc_command_set_results_valist(command, result_name, va);
	va_end(va);
}

