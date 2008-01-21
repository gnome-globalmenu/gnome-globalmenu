#error Cant continue before deciding to use virtual functions or signals.

#ifndef GNOMENU_SERVER_H
#define GNOMENU_SERVER_H

G_BEGIN_DECLS
/**
 * SECTION: gnomenuserver
 * @short_description: Gnome Global Menu Server.
 * @see_also: #GtkMenuBar, #GtkGlobalMenuBar, #GdkSocket.
 * @stablility: Unstable
 * @include: libgnome/gnomenuserver.h
 *
 * GnomeMenuServer is the virtual class for menu server. 
 * If you want to write a menu server, inherit from this class.
 */

#define GNOME_TYPE_MENU_SERVER	(gnome_menu_server_get_type())
#define GNOME_MENU_SERVER(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj), GNOME_TYPE_MENU_SERVER, GnomeMenuServer))
#define GNOME_MENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GNOME_TYPE_MENU_SERVER, GnomeMenuServerClass))
#define GNOME_IS_MENU_SERVER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOME_TYPE_MENU_SERVER))
#define GNOME_IS_MENU_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOME_TYPE_MENU_SERVER))
#define GNOME_MENU_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GNOME_TYPE_MENU_SERVER, GnomeMenuServerClass))

typedef struct _GnomeMenuServerClass GnomeMenuServerClass;
typedef struct _GnomeMenuServer GnomeMenuServer;

/**
 * GnomeMenuServer:
 *  
 *  GnomeMenuServer is the parent class for user defined menu server. It provides basic
 *  operations and interface a menu server should have.
 */
struct _GnomeMenuServer{
	GObject parent;
/*< public >*/
};
/**
 * GdkSocketClass:
 * @menu_create: the virtual function invoked.
 */
struct _GnomeMenuServerClass{
	GObjectClass parent;
/*< private >*/	
	guint signals[GMS_SIGNAL_MAX];
	void (*menu_create)(GnomeMenuServer * self, GnomeMenuClientInfo * clientinfo);
	void (*menu_destroy)(GnomeMenuServer * self, GnomeMenuClientInfo * clientinfo);

};
G_END_DECLS
#endif
