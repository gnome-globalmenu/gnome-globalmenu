#ifndef __PROXY_H__
#define __PROXY_H__
#include <gtk/gtk.h>
G_BEGIN_DECLS

#define GNOMENU_TYPE_PROXY 	(gnomenu_proxy_get_type())
#define GNOMENU_PROXY(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOMENU_TYPE_PROXY, GnomenuProxy))
#define GNOMENU_PROXY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_PROXY, GnomenuProxyClass))
#define GNOMENU_IS_PROXY(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_PROXY))
#define GNOMENU_IS_PROXY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_PROXY))
#define GNOMENU_PROXY_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GNOMENU_TYPE_PROXY, GnomenuProxyClass))

typedef struct {
	GObject parent;
	gchar * path;
} GnomenuProxy;

typedef struct {
	GObjectClass parent;
	void (*signal)(GnomenuProxy * proxy, const char * name, gchar * arg);
} GnomenuProxyClass;

gchar * gnomenu_proxy_invoke(GnomenuProxy * proxy, const char * name, const char * fmt,...);
GnomenuProxy * gnomenu_proxy_new(gchar * path);

G_END_DECLS

#endif
