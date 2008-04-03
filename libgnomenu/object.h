
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

typedef gchar * (*GnomenuObjectMethod) (GnomenuObject * object, const gchar * name, 
					gchar * args);

typedef struct {
	GObjectClass parent;
	GnomenuObjectMethod invoke;
	GHashTable * method_table;
} GnomenuObjectClass;

guint gnomenu_object_class_install_method(GnomenuObjectClass * klass, const gchar * name, GnomenuObjectMethod method, const gchar * fmt);

GnomenuObject * gnomenu_object_new(gchar * name);
void gnomenu_object_emit(GnomenuObject * object, const char * name, const gchar * fmt, ...);

void gnomenu_object_expose(GnomenuObject * object);

#define GNOMENU_OBJECT_QUERY
G_END_DECLS
#endif
