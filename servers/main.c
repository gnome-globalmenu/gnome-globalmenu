#include <config.h>

#include <gtk/gtk.h>

#include "libgnomenu/serverhelper.h"

#include "application.h"

#include "log.h"

static void window_destroy(GtkWidget * widget, gpointer useless){
	gtk_main_quit();
}
int main (int argc, char * argv []){
	GtkWindow * window;
	int i;
	gboolean title_vis = FALSE;
	gboolean icon_vis = FALSE;
	gboolean conf_dlg = FALSE;	
	Application * app;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(window, 400,20);
	g_signal_connect(window, "destroy", 
			window_destroy, NULL);
	for(i=0; i< argc; i++){
		if(g_str_equal(argv[i], "-t")) title_vis = TRUE;
		if(g_str_equal(argv[i], "-i")) icon_vis = TRUE;
		if(g_str_equal(argv[i], "-c")) conf_dlg = TRUE;
	}
	app = g_object_new(TYPE_APPLICATION, 
				"window", window, 
				"title-visible", title_vis,
				"icon-visible", icon_vis, 
				NULL);
	if(conf_dlg) application_show_conf_dialog(app);

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
/*
vim:ts=4:sw=4
*/
