#include <gtk/gtk.h>
#include <libgnomenu/ipcclient.h>
#include <glade/glade.h>
void server_destroy(gpointer data) {
	g_message("server destroy was caught");
}
int main(int argc, char* argv[]){
	GtkWindow * window;
	GtkBox * box;
	GTimer * timer;
	int i;
	gtk_init(&argc, &argv);

	timer = g_timer_new();
	if(!ipc_client_start(server_destroy, NULL)) {
		g_message("no server there");
		return 1;
	}

	for(i=100; i>0; i--) {
		g_timer_start(timer);
		gchar * msg = g_strdup_printf("hello %d", i);
		gchar * result = ipc_client_call_server("Ping", "message", msg, NULL);
		g_assert(g_str_equal(result, msg));
		g_free(msg);
		g_message("result: %s", result);
		g_message("time consumed: %lf", (double) g_timer_elapsed(timer, NULL));
		g_free(result);
	}
	gtk_main();
	return 0;
}
