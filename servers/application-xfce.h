#ifndef _APPLICATION_XFCE_XFCE_H_
#define _APPLICATION_XFCE_XFCE_H_

#include <gtk/gtk.h>

#include "application.h"

typedef struct _ApplicationXfce ApplicationXfce;
typedef struct _ApplicationXfceClass ApplicationXfceClass;

#define TYPE_APPLICATION_XFCE	(application_xfce_get_type())
#define APPLICATION_XFCE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_APPLICATION_XFCE, ApplicationXfce))
#define APPLICATION_XFCE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_APPLICATION_XFCE, ApplicationXfceClass))
#define IS_APPLICATION_XFCE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_APPLICATION_XFCE))
#define IS_APPLICATION_XFCE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_APPLICATION_XFCE))
#define APPLICATION_XFCE_GET_CLASS(klass) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_APPLICATION_XFCE, ApplicationXfceClass))


struct _ApplicationXfce{
	Application parent;
	GtkWidget *event_box;
};

struct _ApplicationXfceClass{
	ApplicationClass parent_class;
};

GType application_xfce_get_type();

Application *application_xfce_new(GtkWidget *applet);


#endif
