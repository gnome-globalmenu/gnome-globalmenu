#include <gtk/gtk.h>
#include <libgnomenu/ipcclient.h>
#include <glade/glade.h>
void server_destroy(gpointer data) {
	g_message("server destroy was caught");
}
void test_single() {
	int i;
	for(i=10; i>0; i--) {
		gchar * msg = g_strdup_printf("hello %d", i);
		gchar * rt = ipc_client_call_server("Ping", "message", msg, NULL);
		if(rt) { 
			g_message("%s", rt);
			g_free(rt);
		}
		g_free(msg);
	}

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

	g_timer_start(timer);
	test_single();
	g_message("time consumed: %lf", (double) g_timer_elapsed(timer, NULL));

	g_message("transaction begins");
	g_timer_start(timer);
	ipc_client_begin_transaction();
	GList * returns, * node;
	test_single();
	ipc_client_end_transaction(&returns);
	g_message("time consumed: %lf", (double) g_timer_elapsed(timer, NULL));
	g_message("transaction ends");
	for(node = returns; node; node=node->next){
		gchar * rt = ipc_command_get_default_result(node->data);
		g_message("%s", rt);
	}
	gtk_main();
	return 0;
}
