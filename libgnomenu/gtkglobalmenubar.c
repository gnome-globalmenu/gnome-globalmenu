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
#define LOG_FUNC_NAME g_print ("%s\n", __func__);
/* Properties */
enum {
  PROP_0,
};

typedef struct 
{
	gboolean dispose_done;
} _Private;

#define _GET_PRIVATE(o)  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_GLOBAL_MENU_BAR, _Private))


static void _set_property      (GObject             *object,
					    guint                prop_id,
					    const GValue        *value,
					    GParamSpec          *pspec);
static void _get_property      (GObject             *object,
					    guint                prop_id,
					    GValue              *value,
					    GParamSpec          *pspec);
static void _size_request      (GtkWidget       *widget,
					    GtkRequisition  *requisition);
/** * s for server */
static void _s_size_request		(GtkWidget * menu_bar,
						GtkRequisition * requisition, 
						GnomenuClientHelper * helper);

static void _size_allocate     (GtkWidget       *widget,
					    GtkAllocation   *allocation);
/** * s for server */
static void _s_size_allocate     (GtkWidget       *menu_bar,
					    GtkAllocation   *allocation, 
						GnomenuClientHelper * helper);

static gint _expose            (GtkWidget       *widget,
					    GdkEventExpose  *event);
static void _hierarchy_changed (GtkWidget       *widget,
					    GtkWidget       *old_toplevel);
static void _realize (GtkWidget *widget);
static void _unrealize (GtkWidget *widget);

static void _dispose (GObject * object);
static void _finalize (GObject * object);
static void _map (GtkWidget * widget);
static void _insert (GtkMenuShell * menu_shell, GtkWidget * widget, gint pos);

static GObject * _constructor (GType type, 
						guint n_construct_properties, 
						GObjectConstructParam *construct_params);
G_DEFINE_TYPE (GtkGlobalMenuBar, gtk_global_menu_bar, GTK_TYPE_MENU_BAR)

static void
gtk_global_menu_bar_class_init (GtkGlobalMenuBarClass *class)
{
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;
	GtkMenuShellClass *menu_shell_class;

	gobject_class = (GObjectClass*) class;
	widget_class = (GtkWidgetClass*) class;
	menu_shell_class = (GtkMenuShellClass*) class;

	gobject_class->get_property = _get_property;
	gobject_class->set_property = _set_property;
	gobject_class->dispose = _dispose;

	gobject_class->finalize = _finalize;
	gobject_class->constructor = _constructor;

	widget_class->size_request = _size_request;
	widget_class->size_allocate = _size_allocate;
	widget_class->expose_event = _expose;
	widget_class->realize = _realize;
	widget_class->unrealize = _unrealize;
	widget_class->hierarchy_changed = _hierarchy_changed;
	widget_class->map = _map;

	menu_shell_class->submenu_placement = GTK_TOP_BOTTOM;
	menu_shell_class->insert = _insert;
	g_type_class_add_private (gobject_class, sizeof (_Private));  
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


static GObject* _constructor(GType type, 
		guint n_construct_properties,
		GObjectConstructParam *construct_params){

	GObject * object;
	GtkGlobalMenuBar * menu_bar;
	object = (G_OBJECT_CLASS(gtk_global_menu_bar_parent_class)->constructor)(
		type, n_construct_properties, construct_params);

	menu_bar = GTK_GLOBAL_MENU_BAR(object);
	(_GET_PRIVATE(object))->dispose_done = FALSE;
	menu_bar->helper = gnomenu_client_helper_new();
	menu_bar->allocation.width = 200;
	menu_bar->allocation.height = 20;
	menu_bar->allocation.x = 0;
	menu_bar->allocation.y = 0;
	menu_bar->requisition.width = 0;
	menu_bar->requisition.height = 0;

	g_signal_connect_swapped(G_OBJECT(menu_bar->helper), "size-allocate",
				G_CALLBACK(_s_size_allocate), menu_bar);
	g_signal_connect_swapped(G_OBJECT(menu_bar->helper), "size-query",
				G_CALLBACK(_s_size_request), menu_bar);
	return object;
}

static void
_dispose (GObject * object){
	_Private * priv = _GET_PRIVATE(object);
	GtkGlobalMenuBar * menu_bar = GTK_GLOBAL_MENU_BAR(object);

	if(!priv->dispose_done){
		priv->dispose_done = TRUE;	
		g_object_unref(menu_bar->helper);
	}
}
static void
_finalize(GObject * object){
	/*Do nothing */
}
static void
_set_property (GObject      *object,
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
_get_property (GObject    *object,
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
_size_request (GtkWidget      *widget,
			   GtkRequisition *requisition)
{
/* Should Return 0 to the caller, a global menu bar is invisible to the
 * user in the parent widget.*/

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (requisition != NULL);

  requisition->width = 0;
  requisition->height = 0;
  
}
static void
_s_size_request (
	GtkWidget * widget,
	GtkRequisition *  requisition,
	GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
  GtkMenuBar *menu_bar;
  GtkMenuShell *menu_shell;
  GtkWidget *child;
  GList *children;
  gint nchildren;
  GtkRequisition child_requisition;
  gint ipadding;
  GtkPackDirection pack_direction, child_pack_direction;

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (requisition != NULL);

  requisition->width = 0;
  requisition->height = 0;
  
  if (GTK_WIDGET_VISIBLE (widget))
    {
      menu_bar = GTK_MENU_BAR (widget);
      menu_shell = GTK_MENU_SHELL (widget);

	  pack_direction = gtk_menu_bar_get_pack_direction(menu_bar);
	  child_pack_direction = gtk_menu_bar_get_child_pack_direction(menu_bar);

      nchildren = 0;
      children = menu_shell->children;

      while (children)
	{
	  child = children->data;
	  children = children->next;

	  if (GTK_WIDGET_VISIBLE (child))
	    {
              gint toggle_size;

	      GTK_MENU_ITEM (child)->show_submenu_indicator = FALSE;
	      gtk_widget_size_request (child, &child_requisition);
              gtk_menu_item_toggle_size_request (GTK_MENU_ITEM (child),
                                                 &toggle_size);

	      if (child_pack_direction == GTK_PACK_DIRECTION_LTR ||
		  child_pack_direction == GTK_PACK_DIRECTION_RTL)
		child_requisition.width += toggle_size;
	      else
		child_requisition.height += toggle_size;

              if (pack_direction == GTK_PACK_DIRECTION_LTR ||
		  pack_direction == GTK_PACK_DIRECTION_RTL)
		{
		  requisition->width += child_requisition.width;
		  requisition->height = MAX (requisition->height, child_requisition.height);
		}
	      else
		{
		  requisition->width = MAX (requisition->width, child_requisition.width);
		  requisition->height += child_requisition.height;
		}
	      nchildren += 1;
	    }
	}

      gtk_widget_style_get (widget, "internal-padding", &ipadding, NULL);
      
      requisition->width += (GTK_CONTAINER (menu_bar)->border_width +
                             ipadding + 
			     BORDER_SPACING) * 2;
      requisition->height += (GTK_CONTAINER (menu_bar)->border_width +
                              ipadding +
			      BORDER_SPACING) * 2;

    }
}
static void
_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation)
{
  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (allocation != NULL);
 
}
static void
_s_size_allocate (GtkWidget * widget, 
	GtkAllocation * allocation,
	GnomenuClientHelper * helper){
  GtkMenuBar *menu_bar;
  GtkMenuShell *menu_shell;
  GtkWidget *child;
  GList *children;
  GtkAllocation child_allocation;
  GtkRequisition child_requisition;
  guint offset;
  GtkTextDirection direction;
  gint ltr_x, ltr_y;
  gint ipadding;
  GtkPackDirection pack_direction, child_pack_direction;

	LOG_FUNC_NAME;

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (allocation != NULL);


  menu_bar = GTK_MENU_BAR (widget);
  menu_shell = GTK_MENU_SHELL (widget);

  direction = gtk_widget_get_direction (widget);

  gtk_widget_style_get (widget, "internal-padding", &ipadding, NULL);

  pack_direction = gtk_menu_bar_get_pack_direction(menu_bar);
  child_pack_direction = gtk_menu_bar_get_child_pack_direction(menu_bar);
  
  if (menu_shell->children)
    {
      child_allocation.x = (GTK_CONTAINER (menu_bar)->border_width +
			    ipadding + 
			    BORDER_SPACING);
      child_allocation.y = (GTK_CONTAINER (menu_bar)->border_width +
			    BORDER_SPACING);
      
      if (pack_direction == GTK_PACK_DIRECTION_LTR ||
	  pack_direction == GTK_PACK_DIRECTION_RTL)
	{
	  child_allocation.height = MAX (1, (gint)allocation->height - child_allocation.y * 2);
	  
	  offset = child_allocation.x; 	/* Window edge to menubar start */
	  ltr_x = child_allocation.x;
	  
	  children = menu_shell->children;
	  while (children)
	    {
	      gint toggle_size;          
	      
	      child = children->data;
	      children = children->next;
	      
	      gtk_menu_item_toggle_size_request (GTK_MENU_ITEM (child),
						 &toggle_size);
	      gtk_widget_get_child_requisition (child, &child_requisition);
	    
	      if (child_pack_direction == GTK_PACK_DIRECTION_LTR ||
		  child_pack_direction == GTK_PACK_DIRECTION_RTL)
		child_requisition.width += toggle_size;
	      else
		child_requisition.height += toggle_size;
	      
	      /* Support for the right justified help menu */
	      if ((children == NULL) && (GTK_IS_MENU_ITEM(child))
		  && (GTK_MENU_ITEM(child)->right_justify)) 
		{
		  ltr_x = allocation->width -
		    child_requisition.width - offset;
		}
	      if (GTK_WIDGET_VISIBLE (child))
		{
		  if ((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_LTR))
		    child_allocation.x = ltr_x;
		  else
		    child_allocation.x = allocation->width -
		      child_requisition.width - ltr_x; 
		  
		  child_allocation.width = child_requisition.width;
		  
		  gtk_menu_item_toggle_size_allocate (GTK_MENU_ITEM (child),
						      toggle_size);
		  gtk_widget_size_allocate (child, &child_allocation);
		  
		  ltr_x += child_allocation.width;
		}
	    }
	}
      else
	{
	  child_allocation.width = MAX (1, (gint)allocation->width - child_allocation.x * 2);
	  
	  offset = child_allocation.y; 	/* Window edge to menubar start */
	  ltr_y = child_allocation.y;
	  
	  children = menu_shell->children;
	  while (children)
	    {
	      gint toggle_size;          
	      
	      child = children->data;
	      children = children->next;
	      
	      gtk_menu_item_toggle_size_request (GTK_MENU_ITEM (child),
						 &toggle_size);
	      gtk_widget_get_child_requisition (child, &child_requisition);
	      
	      if (child_pack_direction == GTK_PACK_DIRECTION_LTR ||
		  child_pack_direction == GTK_PACK_DIRECTION_RTL)
		child_requisition.width += toggle_size;
	      else
		child_requisition.height += toggle_size;
	      
	      /* Support for the right justified help menu */
	      if ((children == NULL) && (GTK_IS_MENU_ITEM(child))
		  && (GTK_MENU_ITEM(child)->right_justify)) 
		{
		  ltr_y = allocation->height -
		    child_requisition.height - offset;
		}
	      if (GTK_WIDGET_VISIBLE (child))
		{
		  if ((direction == GTK_TEXT_DIR_LTR) ==
		      (pack_direction == GTK_PACK_DIRECTION_TTB))
		    child_allocation.y = ltr_y;
		  else
		    child_allocation.y = allocation->height -
		      child_requisition.height - ltr_y; 
		  child_allocation.height = child_requisition.height;
		  
		  gtk_menu_item_toggle_size_allocate (GTK_MENU_ITEM (child),
						      toggle_size);
		  gtk_widget_size_allocate (child, &child_allocation);
		  
		  ltr_y += child_allocation.height;
		}
	    }
	}
    }

}
static gint
_expose (GtkWidget      *widget,
		     GdkEventExpose *event)
{
	GtkGlobalMenuBar * menu_bar;
	gint border;

	menu_bar = GTK_GLOBAL_MENU_BAR(widget);

	g_return_val_if_fail (event != NULL, FALSE);
	LOG_FUNC_NAME;

	if (GTK_WIDGET_DRAWABLE (widget))
    {
		border = GTK_CONTAINER(widget)->border_width;
		g_message("Expose from %p", event->window);
		gtk_paint_box (widget->style,
				menu_bar->container,
				GTK_WIDGET_STATE (widget),
				GTK_SHADOW_NONE,
				&event->area, widget, "menubar",
				border, border,
				menu_bar->allocation.width - border * 2,
				menu_bar->allocation.height - border * 2);

		(* GTK_WIDGET_CLASS (gtk_global_menu_bar_parent_class)->expose_event) (widget, event);
    }

  return FALSE;
}


static void
_hierarchy_changed (GtkWidget *widget,
				GtkWidget *old_toplevel)
{
	GtkWidget *toplevel;  
	GtkGlobalMenuBar *menu_bar;

	menu_bar = GTK_GLOBAL_MENU_BAR (widget);

	GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->hierarchy_changed(widget, old_toplevel);
	toplevel = gtk_widget_get_toplevel(widget);
	if(GTK_WIDGET_TOPLEVEL(toplevel)){
		if(GTK_WIDGET_REALIZED(toplevel)){
/* NOTE: This signal is rarely captured, because usually a menubar is added to a toplevel
 * BEFORE the toplevel is realized. So we need to handle this in _realize. */
			gnomenu_client_helper_send_reparent(menu_bar->helper, toplevel->window);
		}
	}
}
static void
_realize (GtkWidget * widget){
	GtkGlobalMenuBar * menu_bar;
	GtkWidget *toplevel;  

	GdkWindowAttr attributes;
	guint attributes_mask;

	LOG_FUNC_NAME;
	menu_bar = GTK_GLOBAL_MENU_BAR(widget);

	attributes.x = menu_bar->allocation.x;
	attributes.y = menu_bar->allocation.y;
	attributes.width = menu_bar->allocation.width;
	attributes.height = menu_bar->allocation.height; 
/*NOTE: if set this to GDK_WINDOW_CHILD, we can put it anywhere we want without
 * WM's decorations!*/
	attributes.window_type = GDK_WINDOW_TOPLEVEL;

	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget);
	attributes.event_mask |= (GDK_EXPOSURE_MASK |
                GDK_BUTTON_PRESS_MASK |
                GDK_BUTTON_RELEASE_MASK |
                GDK_KEY_PRESS_MASK |
                GDK_ENTER_NOTIFY_MASK |
                GDK_LEAVE_NOTIFY_MASK);

	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	/* FIXME: I don't think it will need visual and colormap, 
 	 * let me try to remove these later*/
	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	menu_bar->container = gdk_window_new (
		gtk_widget_get_root_window(widget), &attributes, attributes_mask);
	gdk_window_set_user_data (menu_bar->container, widget);
/*TODO: remove calling the parent realize function. use our own instead to
 * avoid side effects.*/
	GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->realize(widget);

	gtk_container_forall(GTK_CONTAINER(widget), 
           (GtkCallback)(gtk_widget_set_parent_window), 
           (gpointer)(menu_bar->container));

	gnomenu_client_helper_send_realize(menu_bar->helper, 
		menu_bar->container);

	toplevel = gtk_widget_get_toplevel(widget);
	if(GTK_WIDGET_TOPLEVEL(toplevel)){
		/*If we are in _realize, the toplevel widget must have been realized.*/
		gnomenu_client_helper_send_reparent(menu_bar->helper, toplevel->window);
	}
}
static void
_unrealize (GtkWidget * widget){
	GtkGlobalMenuBar * menu_bar;
	menu_bar = GTK_GLOBAL_MENU_BAR(widget);
	LOG_FUNC_NAME;
	GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->unrealize(widget);
	gdk_window_destroy(menu_bar->container);
	gnomenu_client_helper_send_unrealize(menu_bar->helper);
}
static void
_map (GtkWidget * widget){
	GtkGlobalMenuBar * menu_bar;
	menu_bar = GTK_GLOBAL_MENU_BAR(widget);

	g_return_if_fail(GTK_WIDGET_REALIZED(widget));

	gdk_window_show(menu_bar->container);
	GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->map(widget);
}

static void
_insert (GtkMenuShell * menu_shell, GtkWidget * widget, gint pos){
	GtkGlobalMenuBar * menu_bar = GTK_GLOBAL_MENU_BAR(menu_shell);
	if(GTK_WIDGET_REALIZED(menu_shell)) {
		gtk_widget_set_parent_window(widget, menu_bar->container);
	}
	GTK_MENU_SHELL_CLASS(gtk_global_menu_bar_parent_class)->insert(menu_shell, widget, pos);
}

