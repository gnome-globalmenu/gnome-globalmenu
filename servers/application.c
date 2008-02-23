#include <gtk/gtk.h>
#include "application.h"
#include "menuserver.h"
#include "log.h"

static void _s_window_destroy(GtkWidget * widget, Application * app);

Application * application_new(GtkContainer * window){
	LOG();	
	Application * app = g_new0(Application, 1);
	app->window = window;
	app->server = menu_server_new(window);
	g_signal_connect(G_OBJECT(app->window), 
		"destroy",
        G_CALLBACK(_s_window_destroy), app);

	return app;
}
void application_destroy(Application * app){
	LOG();
	menu_server_destroy(app->server);
	g_free(app);
}

static void _s_window_destroy(GtkWidget * widget, Application * app){
	LOG();
	application_destroy(app);
}

