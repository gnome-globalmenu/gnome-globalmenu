#include <config.h>
#include <gtk/gtk.h>

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_printerr("<GnomenuCall>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif

#include "ipcclient.h"
static gchar * command;
static gchar * event;
static gchar ** par_names;
static gchar ** values;
static gboolean batch = FALSE;
static gchar * target = "SERVER";
GOptionEntry options[] = {
	{"event", 'e', 0, G_OPTION_ARG_STRING, &event, "The event to wait", "name"},
	{"batch", 'b', 0, G_OPTION_ARG_NONE, &batch, "batch mode, reading xml input from stdin", NULL},
	{"command", 'c', 0, G_OPTION_ARG_STRING, &command, "The command to call", "name"},
	{"parameter", 'p', 0, G_OPTION_ARG_STRING_ARRAY, &par_names, "The parameters", "p"},
	{"value", 'v', 0, G_OPTION_ARG_STRING_ARRAY, &values, "The values", "v"},
	{"target", 't', 0, G_OPTION_ARG_STRING, &target, "the target cid that receives the command", "cid|SERVER"},
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
	if((!event && !command && !batch) || 
		(event && command) ||
		(event && batch) ||
		(command && batch)
		){
		/*can't do (1) event and command both
		 * (2) event and batch both
		 * (3) command and bath both*/
		g_printerr("use --help to get help\n");
		return 1;
	}
	if(!ipc_client_start(NULL, NULL)){
		g_printerr("could not start the client\n");
		return 1;
	}
	if(command) {
		gchar * rt = NULL;
	   	ipc_client_call_array(target, command, &rt, par_names, values);
		g_print("%s\n", rt);
		return 0;
	}
	if(event) {
		ipc_client_set_event_array(event, event_cb, NULL, par_names, values);
		gtk_main();
	}
	if(batch) {
		gchar * contents = NULL;
		g_file_get_contents("/dev/stdin", &contents, NULL, NULL);
		if(!contents){
			g_printerr("need commands");
			return 1;
		}
		GList * list = ipc_command_list_parse(contents);
//		GList * rt = ipc_client_call_list(list);
//		gchar * result = ipc_command_list_to_string(rt);
//		ipc_command_list_free(rt);
		ipc_command_list_free(list);
//		g_print("%s", result);
//		g_free(result);
		return 0;
	}
	return 0;
}
