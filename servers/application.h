#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include "menuserver.h"
typedef struct _Application {
	GtkContainer * window;
	GtkFixed * menu_bar_area;
	GtkLabel * label;
	GtkImage * icon;
	MenuServer * server;
} Application;

Application * application_new(GtkContainer * window);
void applicatoin_set_background(Application * app, GdkColor * color, GdkPixmap * pixmap);
void application_destroy(Application * app);
#endif
/*
vim:ts=4:sw=4
*/
