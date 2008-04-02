
#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS

#define GNOMENU_TYPE_OBJECT 	(gnomenu_object_get_type())
#define GNOMENU_OBJECT(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_OBJECT, GnomenuObject))
#define GNOMENU_OBJECT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_OBJECT, GnomenuObjectClass))
#define GNOMENU_IS_OBJECT(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_OBJECT))
#define GNOMENU_IS_OBJECT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_OBJECT))
#define GNOMENU_OBJECT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_OBJECT, GnomenuObjectClass))

typedef struct {
	GObject parent;
	gchar * path;
} GnomenuObject;

typedef void (*GnomenuObjectMethod) (GnomenuObject * object, const gchar * name, 
					gchar * args, gchar * rt);

typedef struct {
	GObjectClass parent;
	GnomenuObjectMethod invoke;
} GnomenuObjectClass;

GnomenuObject * gnomenu_object_new(gchar * name);
void gnomenu_object_emit(GnomenuObject * object, const char * name, const gchar * fmt, ...);

guint gnomenu_object_install_method(GnomenuObject * object, const gchar * name, GnomenuObjectMethod method);

#define GNOMENU_OBJECT_QUERY
G_END_DECLS
#endif
