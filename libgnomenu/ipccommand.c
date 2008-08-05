#include <config.h>

#include <glib.h>
#include "ipccommand.h"

static GHashTable * build_hash_table_va(va_list va) {
	GHashTable * rt = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	gchar * name;
   	while(name = va_arg(va, gchar *)){
		gchar * value = va_arg(va, gchar *);
		g_hash_table_insert(rt, g_strdup(name), g_strdup(value));
	};
	return rt;
}
static GHashTable * build_hash_table_array(gchar ** keys, gchar ** values) {
	GHashTable * rt = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	int i = 0;
	if(keys && values) {
		for(i = 0; keys[i] && values[i]; i++){
			g_hash_table_insert(rt, g_strdup(keys[i]), g_strdup(values[i]));
		}
	}
	return rt;
}

typedef struct {
	GList * command_list;
	IPCCommand * current_command;
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
		gchar * name = NULL;
		gchar * cid = NULL;
		for(i=0; attribute_names[i]; i++) {
			if(g_str_equal(attribute_names[i], "name")){
				name = g_strdup(attribute_values[i]);
			} else
			if(g_str_equal(attribute_names[i], "cid")){
				cid = g_strdup(attribute_values[i]);
			}
		}
		pi->current_command = ipc_command_new(cid, name);
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
		g_hash_table_insert(pi->current_command->parameters,
				pi->current_prop,
				g_string_free(pi->current_prop_value, FALSE));
		pi->current_prop = NULL;
		pi->current_prop_value = NULL;
	} else 
	if(g_str_equal(element_name, "r")) {
		g_hash_table_insert(pi->current_command->results,
				pi->current_result,
				g_string_free(pi->current_result_value, FALSE));
		pi->current_result = NULL;
		pi->current_result_value = NULL;
	} else
	if(g_str_equal(element_name, "command")){
		pi->command_list = g_list_append(pi->command_list, pi->current_command);
		pi->current_command = NULL;
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
	IPCCommand * rt = NULL;
	GList * list = ipc_command_list_parse(string);
	if(!list) {
		g_critical("paring failed");
		goto clean_up;
	}
	if(g_list_length(list)>1){
		g_critical("parsing a command list as a command!");
		GList * node;
		for(node = list; node; node=node->next){
			ipc_command_free(node->data);
		}
		goto clean_up;
	}
	rt = list->data;
clean_up:
	g_list_free(list);
	return rt;
}
GList * ipc_command_list_parse(const gchar * string) {
	ParseInfo cpi = {0};
	GMarkupParseContext * context;
	GError * error = NULL;
	context = g_markup_parse_context_new(&parser, 0, &cpi, NULL);
	if(g_markup_parse_context_parse(context, string, strlen(string), &error)){
	   	if(!cpi.command_list) {
			g_critical("parsing command failed");
			goto clean_up;
		}
		goto clean_up;
	}
	if(error) {
		/* FIXME: free any memory allocation in cpi*/
		g_critical("Parse command failed: %s", error->message);
		g_error_free(error);
		goto clean_up;
	}
clean_up:
	g_markup_parse_context_free(context);
	if(cpi.current_command) ipc_command_free(cpi.current_command);
	if(cpi.current_prop) g_free(cpi.current_prop);
	if(cpi.current_result) g_free(cpi.current_result);
	if(cpi.current_prop_value) g_string_free(cpi.current_prop_value, TRUE);
	if(cpi.current_result_value) g_string_free(cpi.current_result_value, TRUE);
	return cpi.command_list;
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
gchar * ipc_command_list_to_string(GList * command_list){
	GList * node;
	GString * result = g_string_new("");
	for(node = command_list; node; node = node->next) {
		g_string_append(result, ipc_command_to_string(node->data));
		g_string_append_c(result, '\n');
	}
	return g_string_free(result, FALSE);
}
IPCCommand * ipc_command_new(gchar * cid, gchar * name) {
	IPCCommand * rt = g_slice_new0(IPCCommand);
	rt->cid = g_strdup(cid);
	rt->name = g_strdup(name);
	rt->parameters = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	rt->results = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	return rt;
}
void ipc_command_free(IPCCommand * command) {
	if(command->name) g_free(command->name);
	if(command->cid) g_free(command->cid);
	if(command->parameters) g_hash_table_unref(command->parameters);
	if(command->results) g_hash_table_unref(command->results);
	g_slice_free(IPCCommand, command);
}
void ipc_command_list_free(GList * list) {
	GList * node;
	for(node = list; node; node = node->next){
		ipc_command_free(node->data);
	}
	g_list_free(list);
}
void ipc_command_set_parameters_array(IPCCommand * command, gchar ** paras, gchar ** values) {
	if(command->parameters) {
		g_hash_table_unref(command->parameters);
	}
	command->parameters = build_hash_table_array(paras, values);
}
void ipc_command_set_parameters_valist(IPCCommand * command, va_list va) {
	if(command->parameters) {
		g_hash_table_unref(command->parameters);
	}
	command->parameters = build_hash_table_va(va);
}
void ipc_command_set_results_valist(IPCCommand * command, va_list va) {
	if(command->results) {
		g_hash_table_unref(command->results);
	}
	command->results = build_hash_table_va(va);
}
void ipc_command_set_parameters(IPCCommand * command,  ...) {
	va_list va;
	va_start(va, command);
	ipc_command_set_parameters_valist(command, va);
	va_end(va);
}
void ipc_command_set_results(IPCCommand * command, ...) {
	va_list va;
	va_start(va, command);
	ipc_command_set_results_valist(command, va);
	va_end(va);
}
gchar * ipc_command_get_default_result(IPCCommand * command) {
	gchar * rt;
	rt = g_hash_table_lookup(command->results, "default");
	if(rt) return g_strdup(rt);
	return NULL;
}
