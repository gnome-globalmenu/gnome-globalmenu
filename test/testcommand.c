#include <gtk/gtk.h>
#include <libgnomenu/ipccommand.h>

int main(int argc, char* argv[]) {
	gtk_init(&argc, &argv);
	gchar * command = "<command name=\"test\"><p name=\"p1\">value <!--note--> part2</p><r name=\"r1\">return 1</r></command>";
	gchar * name = NULL;
	GHashTable * parameters = NULL;
	GHashTable * results = NULL;
	g_print("%s\n", command);
	g_assert(ipc_command_parse(command, 
			&name,
			&parameters,
			&results
			));

	gchar * reconstructed =
		ipc_command_to_string(name, parameters, results);
	g_print("%s\n", reconstructed);


}
