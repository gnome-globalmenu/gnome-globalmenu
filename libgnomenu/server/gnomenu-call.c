#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuCall>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif

#include "ipcclient.h"
static gchar * command;
static gchar * event;
static gchar ** par_names;
static gchar ** values;

GOptionEntry options[] = {
	{"event", 'e', 0, G_OPTION_ARG_STRING, &event, "The event to wait", "name"},
	{"command", 'c', 0, G_OPTION_ARG_STRING, &command, "The command to call", "name"},
	{"parameter", 'p', 0, G_OPTION_ARG_STRING_ARRAY, &par_names, "The parameters", "p"},
	{"value", 'v', 0, G_OPTION_ARG_STRING_ARRAY, &values, "The values", "v"},
	{NULL}
};
static void event_cb (IPCEvent * event, gpointer data) {
	gchar * str = ipc_event_to_string(event);
	g_print("%s\n", str);
	g_free(str);
	gtk_main_quit();
}
int main(int argc, char* argv[]) {
	GError * error = NULL;
	if(!gtk_init_with_args(&argc, &argv, "Send a gnomenu call", options, NULL, &error)){
		return 1;	
	}
	if(error){
		g_printerr("%s", error->message);
		return 1;
	}
	if((!event && !command) || 
		(event && command)){
		/*can't do event and command both*/
		g_printerr("use --help to get help\n");
		return 1;
	}
	if(!ipc_client_start(NULL, NULL)){
		g_printerr("could not start the client\n");
		return 1;
	}
	if(command) {
		gchar * rt;
	   	ipc_client_call_server_array(command, &rt, par_names, values);
		g_print("%s\n", rt);
		return 0;
	}
	if(event) {
		ipc_client_set_event(event, event_cb, NULL);
		gtk_main();
	}
	return 0;
}
