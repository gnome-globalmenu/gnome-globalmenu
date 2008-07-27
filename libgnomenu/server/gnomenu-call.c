#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuCall>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif

#include "ipcclient.h"
static gchar * command;
static gchar ** par_names;
static gchar ** values;

GOptionEntry options[] = {
	{"command", 'c', 0, G_OPTION_ARG_STRING, &command, "The command to call", "name"},
	{"parameter", 'p', 0, G_OPTION_ARG_STRING_ARRAY, &par_names, "The parameters", "p"},
	{"value", 'v', 0, G_OPTION_ARG_STRING_ARRAY, &values, "The values", "v"},
	{NULL}
};
int main(int argc, char* argv[]) {
	GError * error = NULL;
	if(!gtk_init_with_args(&argc, &argv, "Send a gnomenu call", options, NULL, &error)){
		return 1;	
	}
	if(error){
		g_printerr("%s", error->message);
		return 1;
	}
	if(!ipc_client_start(NULL, NULL)){
		g_printerr("could not start the client\n");
		return 1;
	}
	if(!command || ! par_names || !values) {
		g_printerr("use --help to get help\n");
		return 1;
	}
	gchar * rt = ipc_client_call_server_array(command, par_names, values);
	g_print("%s\n", rt);
	return 0;
}
