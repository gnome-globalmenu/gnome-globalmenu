/*
 * GnomenuMenuBar
 * */
#ifndef __GNOMENU_MENU_BAR_H__
#define __GNOMENU_MENU_BAR_H__
/**
 * SECTION: menubar
 * 	@short_description: Global Menu Bar widget for GTK, 
 * 		intended to replace GtkMenuBar
 * 	@see_also: #GtkMenuBar, #gtk_legacy_menu_bar_new
 * 	@stability: Unstable
 * 	@include: libgnomenu/menubar.h
 *
 * GnomenuMenuBar is the widget providing a global menu bar to 
 * the application. It is intended to (be able to) replace 
 * #GtkMenuBar transparently under most
 * situation. It has to be added as a child widget of a top level window
 * to make its full use. Usually you don't want to simplely use
 * #gnomenu_menu_bar_new. If you want to patch GTK, for the sake of
 * making every application that are built against 
 * #GtkMenuBar benifit from #GnomenuMenuBar, 
 * use #gtk_legacy_menu_bar_new instead. That function deals 
 * with quirks of various
 * hackish GTK applications.
 *
 * The solution provided by libgnomenu is via remote widget or widget embeding
 * . So maybe in the future we can seperate these code out and redesign the
 * GTK widget embedding solution (i.e #GtkPlug and #GtkSocket), which is 
 * difficult to build widget upon.
 *
 * #GnomenuMenuBar plays two roles. 
 *
 * 1. It allocates an 'widget->window' whose parent
 * 	(or parent of parrent, whatever level) is the toplevel widget.
 * 	The events received from this window will be translated and dispatched
 * 	from the ordinary GTK mechanism, which provides compatibility with
 * 	the original #GtkMenuBar, and which is the essential to receive keyboard
 * 	events (Alt+xxx) from the applications toplevel window.
 *
 * 2. It creates an #GnomenuClientHelper, which is a subclass of #GdkSocket
 * 	that understands widget embedding messages and Global Menu messages
 * 	(seperating them will be one of the future topic). These messages are
 * 	then dispatched via #GnomenuClientHelper's signals. This gives the
 * 	privilege for the menu server to control the clients behavior.
 *
 * 	But be careful here. This might be a security hole for a privilege raising
 * 	. Suppose a global menu
 * 	client is running as root other process
 * 	without root access can control a root process in some way. This behavior
 * 	is by definition and it is
 * 	dangerous.  However as GTk is supposed to not deal with these circumstances
 * 	(refer to the discussions around GTK_MODULES environment variable),
 * 	and the messages we are dealing is really nothing more than 12 bytes
 * 	(under X11), it might not be a big hole, or not even a hole.
 *
 */

#include <gdk/gdk.h>
#include <gtk/gtkmenubar.h>
#include "clienthelper.h"

G_BEGIN_DECLS


#define	GNOMENU_TYPE_MENU_BAR               (gnomenu_menu_bar_get_type ())
#define GNOMENU_MENU_BAR(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNOMENU_TYPE_MENU_BAR, GnomenuMenuBar))
#define GNOMENU_MENU_BAR_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GNOMENU_TYPE_MENU_BAR, GnomenuMenuBarClass))
#define GNOMENU_IS_MENU_BAR(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNOMENU_TYPE_MENU_BAR))
#define GNOMENU_IS_MENU_BAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GNOMENU_TYPE_MENU_BAR))
#define GNOMENU_MENU_BAR_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GNOMENU_TYPE_MENU_BAR, GnomenuMenuBarClass))

typedef struct _GnomenuMenuBar       GnomenuMenuBar;
typedef struct _GnomenuMenuBarClass  GnomenuMenuBarClass;

/**
 * GnomenuMenuBar:
 * 	@parent: the parent object. 
 *	@helper: the helper for it to play the role as a GNOME menu client.
 *		This is actually an interface. However since at the begining I
 *		was not sure if interface can have default behavior in GLib. 
 *		I decided to follow the favoring 'composition' than 'inheriting'
 *		rule here.
 *	@container: as the name indicates. #container is the parent window
 *		for the sub widgets(which turns out to be #GtkMenuItem s) 
 *		of this #GtkMenuShell. 
 *	@floater: this is a floating toplevel window which act as the 
 *		container of the #container window in detached mode.
 *  @allocation: this is the allocation the menu bar received from
 *  	the menu server. It should never be confused with the allocation
 *  	field in the #GtkWidget interface.
 *  @requisition: this is the requisition the menu bar request from 
 *  	the menu server. It should never be confused with the requisition
 *  	field in the #GtkWidget interface. It is the true requisition;
 *  	whereas the widget's is not always the true requisition.
 * 	@x: the x position of container in it's parent window. (ltr value)
 * 	@y: the y position of container in it's parent window. (ltr value)
 *
 *  I think some of these variables can be moved in to the Private
 *  data structure, especially the #allocation and #requisition, since
 *  The #GtkWidget interface also has those and we have different 
 *  semantic meanings with the #GtkWidget ones. The original purpose
 *  to keep them here is ease to refer to. But with my cute GET_OBJECT
 *  macro in the .c file, referring to a Private variable don't require
 *  much more typings than a public member.
 */
struct _GnomenuMenuBar
{
	GtkMenuBar parent;
};

/**
 * GnomenuMenuBarClass:
 *
 * This is shameless copied from gtkmenubar.c .
 * Even those _gtk_reserved. and the style of using
 * the asternoid is also the GTK way instead of my way.
 */
struct _GnomenuMenuBarClass
{
  GtkMenuBarClass parent_class;
};


GtkWidget * gnomenu_menu_bar_new             (void);

G_END_DECLS


#endif /* __GNOMENU_MENU_BAR_H__ */
