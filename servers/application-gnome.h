#ifndef _APPLICATION_GNOME_GNOME_H_
#define _APPLICATION_GNOME_GNOME_H_

#include <gtk/gtk.h>

#include "application.h"

typedef struct _ApplicationGnome ApplicationGnome;
typedef struct _ApplicationGnomeClass ApplicationGnomeClass;

#define TYPE_APPLICATION_GNOME	(application_gnome_get_type())
#define APPLICATION_GNOME(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_APPLICATION_GNOME, ApplicationGnome))
#define APPLICATION_GNOME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_APPLICATION_GNOME, ApplicationGnomeClass))
#define IS_APPLICATION_GNOME(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_APPLICATION_GNOME))
#define IS_APPLICATION_GNOME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_APPLICATION_GNOME))
#define APPLICATION_GNOME_GET_CLASS(klass) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_APPLICATION_GNOME, ApplicationGnomeClass))


struct _ApplicationGnome{
	Application parent;
};

struct _ApplicationGnomeClass{
	ApplicationClass parent_class;
};

GType application_gnome_get_type();

Application *application_gnome_new(GtkWidget *applet);


#endif
