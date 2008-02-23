#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include "menuserver.h"
typedef struct _Application {
	GtkContainer * window;
	MenuServer * server;
} Application;

Application * application_new(GtkContainer * window);
void application_destroy(Application * app);
#endif
