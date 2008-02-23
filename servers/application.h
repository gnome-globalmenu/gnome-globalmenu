#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE
#include "menuserver.h"

typedef struct _Application {
	WnckScreen * screen;
	GtkContainer * window;
	MenuServer * server;
} Application;

Application * application_new(GtkContainer * window);
void application_destroy(Application * app);
#endif
