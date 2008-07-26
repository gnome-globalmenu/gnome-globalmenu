#include <gtk/gtk.h>
#include <libgnomenu/ipccommand.h>

int main(int argc, char* argv[]) {
	gtk_init(&argc, &argv);
	gchar * str = "<command cid=\"testcid\" name=\"test\"><p name=\"p1\">value <!--note--> part2</p><r name=\"r1\">return 1</r></command>";
	gchar * name = NULL;
	IPCCommand * command;
	g_print("%s\n", str);
	g_assert(command = ipc_command_parse(str));

	gchar * reconstructed =
		ipc_command_to_string(command);
	g_print("%s\n", reconstructed);
	ipc_command_free(command);

}
