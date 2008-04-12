
#ifndef __SERVICE_H__
#define __SERVICE_H__
#include <gtk/gtk.h>
#include "object.h"
G_BEGIN_DECLS

#define GNOMENU_TYPE_SERVICE 	(gnomenu_service_get_type())
#define GNOMENU_SERVICE(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_SERVICE, GnomenuService))
#define GNOMENU_SERVICE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_SERVICE, GnomenuServiceClass))
#define GNOMENU_IS_SERVICE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_SERVICE))
#define GNOMENU_IS_SERVICE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_SERVICE))
#define GNOMENU_SERVICE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_SERVICE, GnomenuServiceClass))

typedef struct {
	GObject parent;
	gchar * path;
	GHashTable * object_table;
} GnomenuService;

typedef gchar * (*GnomenuServiceMethod) (GnomenuService * service, const gchar * name, 
					gchar * args);

typedef struct {
	GObjectClass parent;
	GHashTable * method_table;
} GnomenuServiceClass;

GnomenuService * gnomenu_service_new(gchar * name);

//void gnomenu_service_emit(GnomenuService * service, GObject * object, const char * name, 
//				const gchar * fmt, ...);
void gnomenu_service_expose(GnomenuService * service);
void gnomenu_service_register(GnomenuService * service, GnomenuObject * object);
void gnomenu_service_dispose(GnomenuService * service, const gchar * name);
GnomenuObject * gnomenu_service_steal(GnomenuService * service, const gchar * name);
G_END_DECLS
#endif
