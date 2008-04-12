#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS

#define GNOMENU_TYPE_CONNECTION 	(gnomenu_connection_get_type())
#define GNOMENU_CONNECTION(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_CONNECTION, GnomenuConnection))
#define GNOMENU_CONNECTION_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_CONNECTION, GnomenuConnectionClass))
#define GNOMENU_IS_CONNECTION(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_CONNECTION))
#define GNOMENU_IS_CONNECTION_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_CONNECTION))
#define GNOMENU_CONNECTION_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_CONNECTION, GnomenuConnectionClass))

typedef struct {
	GObject parent;
	gchar * path;
} GnomenuConnection;
typedef struct {
	gchar * name;
	gchar * fmt;
} GnomenuConnectionMethodInfo;
typedef struct {
	GObjectClass parent;
	void (*signal)(GnomenuConnection * connection, const char * name, gchar * arg);
} GnomenuConnectionClass;

gchar * gnomenu_connection_invoke(GnomenuConnection * connection, const gchar * object, const char * name, const char * fmt,...);

GnomenuConnection * gnomenu_connection_new(gchar * path);

gboolean gnomenu_connection_connect(GnomenuConnection * connection);
GList * gnomenu_connection_query_methods(GnomenuConnection * connection);

G_END_DECLS

#endif
