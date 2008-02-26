#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include "menuserver.h"
typedef struct _Application {
	GtkContainer * window;
	GtkFixed * menu_bar_area;
	GCallback * popup_menu;
	MenuServer * server;
} Application;

Application * application_new(GtkContainer * window);
void application_destroy(Application * app);
#endif
/*
vim:ts=4:sw=4
*/
