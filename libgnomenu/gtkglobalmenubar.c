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


#include <config.h>
#include "gtkglobalmenubar.h"

#define BORDER_SPACING  0
#define DEFAULT_IPADDING 1

/* Properties */
enum {
  PROP_0,
};

typedef struct _GtkGlobalMenuBarPrivate GtkGlobalMenuBarPrivate;
struct _GtkGlobalMenuBarPrivate
{
	int foo;
};

#define GTK_GLOBAL_MENU_BAR_GET_PRIVATE(o)  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBarPrivate))


static void gtk_global_menu_bar_set_property      (GObject             *object,
					    guint                prop_id,
					    const GValue        *value,
					    GParamSpec          *pspec);
static void gtk_global_menu_bar_get_property      (GObject             *object,
					    guint                prop_id,
					    GValue              *value,
					    GParamSpec          *pspec);
static void gtk_global_menu_bar_size_request      (GtkWidget       *widget,
					    GtkRequisition  *requisition);
static void gtk_global_menu_bar_size_allocate     (GtkWidget       *widget,
					    GtkAllocation   *allocation);
static void gtk_global_menu_bar_paint             (GtkWidget       *widget,
					    GdkRectangle    *area);
static gint gtk_global_menu_bar_expose            (GtkWidget       *widget,
					    GdkEventExpose  *event);
static void gtk_global_menu_bar_hierarchy_changed (GtkWidget       *widget,
					    GtkWidget       *old_toplevel);


G_DEFINE_TYPE (GtkGlobalMenuBar, gtk_global_menu_bar, GTK_TYPE_MENU_BAR)

static void
gtk_menu_bar_class_init (GtkGlobalMenuBarClass *class)
{
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkMenuShellClass *menu_shell_class;

  gobject_class = (GObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  menu_shell_class = (GtkMenuShellClass*) class;

  gobject_class->get_property = gtk_global_menu_bar_get_property;
  gobject_class->set_property = gtk_global_menu_bar_set_property;

  widget_class->size_request = gtk_global_menu_bar_size_request;
  widget_class->size_allocate = gtk_global_menu_bar_size_allocate;
  widget_class->expose_event = gtk_global_menu_bar_expose;
  widget_class->hierarchy_changed = gtk_global_menu_bar_hierarchy_changed;
  
  menu_shell_class->submenu_placement = GTK_TOP_BOTTOM;

  g_type_class_add_private (gobject_class, sizeof (GtkGlobalMenuBarPrivate));  
}

static void
gtk_global_menu_bar_init (GtkGlobalMenuBar *object)
{
}

GtkWidget*
gtk_global_menu_bar_new (void)
{
  return g_object_new (GTK_TYPE_GLOBAL_MENU_BAR, NULL);
}

static void
gtk_global_menu_bar_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  GtkGlobalMenuBar *menubar = GTK_GLOBAL_MENU_BAR (object);
  
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_global_menu_bar_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
  GtkGlobalMenuBar *menubar = GTK_GLOBAL_MENU_BAR (object);
  
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_global_menu_bar_size_request (GtkWidget      *widget,
			   GtkRequisition *requisition)
{
/* Should Return 0 to the caller, a global menu bar is invisible to the
 * user in the parent widget.*/
  GtkMenuBar *menu_bar;
  GtkGlobalMenuBarPrivate *priv;
  GtkMenuShell *menu_shell;
  GtkWidget *child;
  GList *children;
  gint nchildren;
  GtkRequisition child_requisition;
  gint ipadding;

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (requisition != NULL);

  requisition->width = 0;
  requisition->height = 0;
  
}

static void
gtk_global_menu_bar_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation)
{
/*Should not pass this to the children.*/
  GtkMenuBar *menu_bar;
  GtkMenuShell *menu_shell;
  GtkGlobalMenuBarPrivate *priv;
  GtkWidget *child;
  GList *children;
  GtkAllocation child_allocation;
  GtkRequisition child_requisition;
  guint offset;
  GtkTextDirection direction;
  gint ltr_x, ltr_y;
  gint ipadding;

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (allocation != NULL);

  menu_bar = GTK_MENU_BAR (widget);
  menu_shell = GTK_MENU_SHELL (widget);
  priv = GTK_GLOBAL_MENU_BAR_GET_PRIVATE (menu_bar);

  direction = gtk_widget_get_direction (widget);

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window,
			    allocation->x, allocation->y,
			    allocation->width, allocation->height);

  gtk_widget_style_get (widget, "internal-padding", &ipadding, NULL);
  
}

static void
gtk_global_menu_bar_paint (GtkWidget    *widget,
                    GdkRectangle *area)
{
/*Remove this function and implement in expose event, where we
 * can distinguish where the expose event is from.*/
  g_return_if_fail (GTK_IS_MENU_BAR (widget));

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      gint border;

      border = GTK_CONTAINER (widget)->border_width;
      
      gtk_paint_box (widget->style,
		     widget->window,
                     GTK_WIDGET_STATE (widget),
                     get_shadow_type (GTK_MENU_BAR (widget)),
		     area, widget, "menubar",
		     border, border,
		     widget->allocation.width - border * 2,
                     widget->allocation.height - border * 2);
    }
}

static gint
gtk_menu_bar_expose (GtkWidget      *widget,
		     GdkEventExpose *event)
{
  g_return_val_if_fail (GTK_IS_MENU_BAR (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      gtk_menu_bar_paint (widget, &event->area);

      (* GTK_WIDGET_CLASS (gtk_global_menu_bar_parent_class)->expose_event) (widget, event);
    }

  return FALSE;
}


static void
gtk_global_menu_bar_hierarchy_changed (GtkWidget *widget,
				GtkWidget *old_toplevel)
{
  GtkWidget *toplevel;  
  GtkMenuBar *menubar;

  menubar = GTK_MENU_BAR (widget);

  GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->hierarchy_changed(widget, old_toplevel);
}




#define __GTK_GLOBAL_MENU_BAR_C__
