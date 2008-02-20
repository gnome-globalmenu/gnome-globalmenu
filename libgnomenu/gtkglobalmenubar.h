/*
 * GtkGlobalMenuBar
 * */
#ifndef __GTK_GLOBAL_MENU_BAR_H__
#define __GTK_GLOBAL_MENU_BAR_H__


#include <gdk/gdk.h>
#include <gtk/gtkmenubar.h>
#include "clienthelper.h"

G_BEGIN_DECLS


#define	GTK_TYPE_GLOBAL_MENU_BAR               (gtk_global_menu_bar_get_type ())
#define GTK_GLOBAL_MENU_BAR(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBar))
#define GTK_GLOBAL_MENU_BAR_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBarClass))
#define GTK_IS_GLOBAL_MENU_BAR(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_GLOBAL_MENU_BAR))
#define GTK_IS_GLOBAL_MENU_BAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_GLOBAL_MENU_BAR))
#define GTK_GLOBAL_MENU_BAR_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBarClass))

typedef struct _GtkGlobalMenuBar       GtkGlobalMenuBar;
typedef struct _GtkGlobalMenuBarClass  GtkGlobalMenuBarClass;

struct _GtkGlobalMenuBar
{
	GtkMenuBar parent;
	GnomenuClientHelper * helper;
	GdkWindow * container;
	GdkWindow * floater;
	GtkAllocation allocation;
	GtkRequisition requisition;
};

struct _GtkGlobalMenuBarClass
{
  GtkMenuBarClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GtkWidget* gtk_global_menu_bar_new             (void);

G_END_DECLS


#endif /* __GTK_GLOBAL_MENU_BAR_H__ */
