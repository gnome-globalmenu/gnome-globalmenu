#include <gtk/gtk.h>
#include "application.h"
#include "log.h"

static void _s_window_destroy(GtkWidget * widget, Application * app);

Application * application_new(GtkContainer * window){
	LOG();	
	Application * app = g_new0(Application, 1);
	app->window = window;
	app->screen = wnck_screen_get_default();
	app->gtk_helper = gnomenu_server_helper_new();
	app->kde_helper = NULL; /*TODO: implement this*/
//	app->server = menu_server_new(app);
	g_signal_connect(G_OBJECT(app->window), 
		"destroy",
        G_CALLBACK(_s_window_destroy), app);

	return app;
}
void application_destroy(Application * app){
	LOG();
	g_object_unref(app->gtk_helper);
	//Destroy: app->kde_helper
	g_free(app);
}

static void _s_window_destroy(GtkWidget * widget, Application * app){
	LOG();
	application_destroy(app);
}

