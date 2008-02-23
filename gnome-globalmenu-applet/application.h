#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libgnomenu/serverhelper.h>

typedef void KDEmenuServerHelper; /* TODO: write this helper*/
typedef void MenuServer; /*TODO: write this*/
typedef struct _Application {
	WnckScreen * screen;
	GtkContainer * window;
	GnomenuServerHelper * gtk_helper;
	KDEmenuServerHelper * kde_helper;
	MenuServer * server;
} Application;

Application * application_new(GtkContainer * window);
void application_destroy(Application * app);
#endif
