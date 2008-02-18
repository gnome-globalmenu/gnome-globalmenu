/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */
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
