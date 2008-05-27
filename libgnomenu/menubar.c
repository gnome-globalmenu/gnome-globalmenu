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
#define GET_OBJECT(_s, sgmb, p) \
	GnomenuMenuBar * sgmb = (GnomenuMenuBar*)_s; \
	GnomenuMenuBarPrivate * p = GNOMENU_MENU_BAR_GET_PRIVATE(_s);

#define GNOMENU_MENU_BAR_GET_PRIVATE(o)  \
	_get_private(o)

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuMenuBar>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <string.h>
#include "menubar.h"
#include "widget.h"
#include "introspector.h"
#include "builder.h"

#define BORDER_SPACING  0
#define DEFAULT_IPADDING 1

/* Properties */
enum {
  PROP_0,
  PROP_PACK_DIRECTION,
  PROP_CHILD_PACK_DIRECTION,
  PROP_IS_GLOBAL_MENU,
  PROP_SHOW_ARROW
};

typedef struct _GnomenuMenuBarPrivate GnomenuMenuBarPrivate;
struct _GnomenuMenuBarPrivate
{
  GtkPackDirection _pack_direction; /*comaptibility with GtkMenuBar, never directly used*/
  GtkPackDirection _child_pack_direction; /*same as above*/

  gboolean is_global_menu;
  gchar * introspection;
  gboolean introspection_is_dirty;
  gboolean show_arrow;

	gboolean disposed;
	gboolean detached;

	GHashTable * menu_items;
	GtkWidget * arrow_button;
	GtkWidget * arrow;
	gboolean need_arrow;
	GtkMenu	* popup_menu;
	GtkRequisition true_requisition;
	Builder * builder;
};



static void gnomenu_menu_bar_set_property      (GObject             *object,
					    guint                prop_id,
					    const GValue        *value,
					    GParamSpec          *pspec);
static void gnomenu_menu_bar_get_property      (GObject             *object,
					    guint                prop_id,
					    GValue              *value,
					    GParamSpec          *pspec);
static void gnomenu_menu_bar_size_request      (GtkWidget       *widget,
					    GtkRequisition  *requisition);
static void gnomenu_menu_bar_size_allocate     (GtkWidget       *widget,
					    GtkAllocation   *allocation);
static void gnomenu_menu_bar_paint             (GtkWidget       *widget,
					    GdkRectangle    *area);
static gint gnomenu_menu_bar_expose            (GtkWidget       *widget,
					    GdkEventExpose  *event);
static void _s_hierarchy_changed ( GtkWidget       *widget,
					    GtkWidget       *old_toplevel, gpointer data);
static void _s_toplevel_realize ( GtkMenuBar       *menubar,
					    GtkWidget * toplevel);
static void _s_notify_has_toplevel_focus ( GtkMenuBar * menubar, GParamSpec * pspec, GtkWindow * window);
static gint gnomenu_menu_bar_get_popup_delay   (GtkMenuShell    *menu_shell);
static void gnomenu_menu_bar_set_is_global_menu(GtkMenuBar * menubar, gboolean is_global_menu);
static void _sms_filter ( GtkMenuBar * menubar, gchar * sms, gint sizs);
static void _send_refresh_global_menu_sms (GtkMenuBar * menubar);

static gchar * _update_introspection ( GtkMenuBar * menubar);
static GdkWindow * _get_toplevel_gdk_window(GtkMenuBar * menubar);
static GtkMenuItem * _get_item_for_proxy(GtkMenuBar * menubar, GtkMenuItem * proxy);
GtkMenuItem * _get_proxy_for_item(GtkMenuBar * menubar, GtkMenuItem * item);
static void _invalidate_introspection ( GtkMenuBar * menubar);
/* GObject interface */
static GObject * _constructor 		( GType type, guint n_construct_properties, 
									  GObjectConstructParam *construct_params );
static void _dispose 				( GObject * object );
static void _finalize 				( GObject * object );
/* GtkMenuShell Interface */
static void _insert 				( GtkMenuShell * menu_shell, 
									  GtkWidget * widget, gint pos);
/* GtkContainer Inteface */
static void _remove 				( GtkContainer * container, GtkWidget * widget);
static void _forall					( GtkContainer    *container,
									  gboolean     include_internals,
									  GtkCallback      callback,
									  gpointer     callback_data);

static void _s_arrow_button_clicked		( GtkWidget * menubar,
									  GtkWidget * arrow_button);
static void _s_popup_menu_deactivated	( GtkWidget * menubar,
									  GtkWidget * popup_menu);
static void _build_popup_menu 		( GnomenuMenuBar * self);

typedef struct {
	gboolean overflowed;
	GtkMenuItem * menu_item;
} MenuItemInfo;

static GtkShadowType get_shadow_type   (GtkMenuBar      *menubar);

void gnomenu_menu_bar_init (GnomenuMenuBar * self);

static void gnomenu_menu_bar_class_init (GnomenuMenuBarClass * klass);

static gpointer _menu_shell_class = NULL;

void gnomenu_menu_bar_class_intern_init (gpointer klass){
	_menu_shell_class = g_type_class_peek(GTK_TYPE_MENU_SHELL);
	gnomenu_menu_bar_class_init ((GnomenuMenuBarClass *)klass);
}
GType gnomenu_menu_bar_get_type (void){
	static GType g_define_type_id = 0; 
	if (G_UNLIKELY (g_define_type_id == 0)) { 
		static const GTypeInfo g_define_type_info = { 
			sizeof (GnomenuMenuBarClass), 
			(GBaseInitFunc) NULL, 
			(GBaseFinalizeFunc) NULL, 
			(GClassInitFunc) gnomenu_menu_bar_class_intern_init, 
			(GClassFinalizeFunc) NULL, 
			NULL,   /* class_data */ 
			sizeof (GnomenuMenuBar), 
			0,      /* n_preallocs */ 
			(GInstanceInitFunc) gnomenu_menu_bar_init, 
		}; 
		g_define_type_id = g_type_register_static (GTK_TYPE_MENU_BAR, 
								"GnomenuMenuBar", &g_define_type_info, 0); 
	} 
	return g_define_type_id; 
}

static GnomenuMenuBarPrivate * _get_private(gpointer o){
	if(GNOMENU_IS_MENU_BAR(o)){
  		return (G_TYPE_INSTANCE_GET_PRIVATE ((o), GNOMENU_TYPE_MENU_BAR, GnomenuMenuBarPrivate));
	} else {
  		return (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_MENU_BAR, GnomenuMenuBarPrivate));
	}
}

static void
gnomenu_menu_bar_class_init (GnomenuMenuBarClass *class)
{
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkMenuShellClass *menu_shell_class;
  GtkContainerClass *container_class;

  GtkBindingSet *binding_set;

  gobject_class = (GObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  menu_shell_class = (GtkMenuShellClass*) class;
  container_class = (GtkContainerClass*) class;

  gobject_class->get_property = gnomenu_menu_bar_get_property;
  gobject_class->set_property = gnomenu_menu_bar_set_property;


  widget_class->size_request = gnomenu_menu_bar_size_request;
  widget_class->size_allocate = gnomenu_menu_bar_size_allocate;
  widget_class->expose_event = gnomenu_menu_bar_expose;
  
  menu_shell_class->submenu_placement = GTK_TOP_BOTTOM;
  menu_shell_class->get_popup_delay = gnomenu_menu_bar_get_popup_delay;

	gobject_class->finalize = _finalize;
	gobject_class->constructor = _constructor;
	menu_shell_class->insert = _insert;
	container_class->remove = _remove;
	container_class->forall = _forall;

  g_type_class_add_private (gobject_class, sizeof (GnomenuMenuBarPrivate));  
  g_object_class_install_property(gobject_class, PROP_IS_GLOBAL_MENU,
		  g_param_spec_boolean("is-global-menu",
			  "whether the menu bar is a global menu",
			  "whether the menu bar is a global menu",
			  TRUE, 
			  G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property(gobject_class, PROP_SHOW_ARROW,
		  g_param_spec_boolean("show-arrow",
			  "show the overflown items in a menu",
			  "show the overflown items in a menu",
			  TRUE, 
			  G_PARAM_READABLE | G_PARAM_WRITABLE));
}

static void _menu_item_info_free(MenuItemInfo * info){
	g_free(info);
}

void
gnomenu_menu_bar_init (GnomenuMenuBar *object)
{
	GET_OBJECT(object, menu_bar, priv);
	priv->menu_items = g_hash_table_new_full(NULL, NULL, NULL, (GDestroyNotify)_menu_item_info_free);
	priv->popup_menu = GTK_MENU(gtk_menu_new());
	priv->arrow_button = GTK_WIDGET(gtk_toggle_button_new());
	priv->arrow = gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_NONE);
	priv->need_arrow = FALSE;
	priv->disposed = FALSE;
	priv->detached = FALSE;
	priv->show_arrow = TRUE;
	priv->is_global_menu = TRUE;
	priv->introspection = NULL;
	priv->introspection_is_dirty = TRUE;
	priv->builder = NULL;
}

GtkWidget*
gnomenu_menu_bar_new (void)
{
  return g_object_new (GNOMENU_TYPE_MENU_BAR, NULL);
}

GtkWidget*
gnomenu_menu_bar_new_local (void)
{
  return g_object_new (GNOMENU_TYPE_MENU_BAR, "is-global-menu", FALSE, NULL);
}

static void
gnomenu_menu_bar_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  
  switch (prop_id)
    {
    case PROP_PACK_DIRECTION:
      gnomenu_menu_bar_set_pack_direction (menubar, g_value_get_enum (value));
      break;
    case PROP_CHILD_PACK_DIRECTION:
      gnomenu_menu_bar_set_child_pack_direction (menubar, g_value_get_enum (value));
      break;
    case PROP_IS_GLOBAL_MENU:
	  gnomenu_menu_bar_set_is_global_menu(menubar, g_value_get_boolean(value));
	  break;
    case PROP_SHOW_ARROW:
	  gnomenu_menu_bar_set_show_arrow(menubar, g_value_get_boolean(value));
	  break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gnomenu_menu_bar_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  
  switch (prop_id)
    {
    case PROP_PACK_DIRECTION:
      g_value_set_enum (value, gnomenu_menu_bar_get_pack_direction (menubar));
      break;
    case PROP_CHILD_PACK_DIRECTION:
      g_value_set_enum (value, gnomenu_menu_bar_get_child_pack_direction (menubar));
      break;
    case PROP_IS_GLOBAL_MENU:
	  g_value_set_boolean(value, gnomenu_menu_bar_get_is_global_menu(menubar));
	  break;
    case PROP_SHOW_ARROW:
	  g_value_set_boolean(value, gnomenu_menu_bar_get_show_arrow(menubar));
	  break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gnomenu_menu_bar_size_request (GtkWidget      *widget,
			   GtkRequisition *requisition)
{
  GtkMenuBar *menu_bar;
  GnomenuMenuBarPrivate *priv;
  GtkMenuShell *menu_shell;
  GtkWidget *child;
  GList *children;
  gint nchildren;
  GtkRequisition child_requisition;
  gint ipadding;
  GtkPackDirection child_pack_direction;
  GtkPackDirection pack_direction;

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (requisition != NULL);

  requisition->width = 0;
  requisition->height = 0;

  if (GTK_WIDGET_VISIBLE (widget))
    {
      menu_bar = GTK_MENU_BAR (widget);
      menu_shell = GTK_MENU_SHELL (widget);
	  child_pack_direction = gnomenu_menu_bar_get_child_pack_direction(menu_bar); 
	  pack_direction = gnomenu_menu_bar_get_pack_direction(menu_bar); 
      priv = GNOMENU_MENU_BAR_GET_PRIVATE (menu_bar);
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

      if (get_shadow_type (menu_bar) != GTK_SHADOW_NONE)
	{
	  requisition->width += widget->style->xthickness * 2;
	  requisition->height += widget->style->ythickness * 2;
	}
    }
	if(priv->show_arrow){
		priv->true_requisition = *requisition;
		GtkPackDirection pack_direction;
		pack_direction = gnomenu_menu_bar_get_pack_direction(GTK_MENU_BAR(widget));	
		switch(pack_direction){
			case GTK_PACK_DIRECTION_LTR:
			case GTK_PACK_DIRECTION_RTL:
				requisition->width = 0;
			break;
			case GTK_PACK_DIRECTION_BTT:
			case GTK_PACK_DIRECTION_TTB:
				requisition->height = 0;
			break;
		}
	}
}

static void
gnomenu_menu_bar_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation)
{
  GtkMenuBar *menu_bar;
  GtkMenuShell *menu_shell;
  GnomenuMenuBarPrivate *priv;
  GtkWidget *child;
  GList *children;
  GtkAllocation child_allocation;
  GtkRequisition child_requisition;
  guint offset;
  GtkTextDirection direction;
  gint ltr_x, ltr_y;
  gint ipadding;
	GtkPackDirection pack_direction;
	GtkPackDirection child_pack_direction;

	GtkAllocation adjusted;
	MenuItemInfo * menu_info;
	GtkRequisition arrow_requisition;
	GtkAllocation arrow_allocation;

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (allocation != NULL);

  menu_bar = GTK_MENU_BAR (widget);
  menu_shell = GTK_MENU_SHELL (widget);
  priv = GNOMENU_MENU_BAR_GET_PRIVATE (menu_bar);

	pack_direction = gnomenu_menu_bar_get_pack_direction(menu_bar);
	child_pack_direction = gnomenu_menu_bar_get_child_pack_direction(menu_bar);

	if(priv->show_arrow){
	adjusted = * allocation;
	adjusted.x = 0; /*the x, y offset is taken care by either widget->window or priv->floater*/
	adjusted.y = 0;
	gtk_widget_size_request(priv->arrow_button, &arrow_requisition);

	arrow_allocation.width = arrow_requisition.width;
	arrow_allocation.height = arrow_requisition.height;
	priv->need_arrow = FALSE;
	}

  direction = gtk_widget_get_direction (widget);

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window,
			    allocation->x, allocation->y,
			    allocation->width, allocation->height);

  gtk_widget_style_get (widget, "internal-padding", &ipadding, NULL);
  
  if (menu_shell->children)
    {
      child_allocation.x = (GTK_CONTAINER (menu_bar)->border_width +
			    ipadding + 
			    BORDER_SPACING);
      child_allocation.y = (GTK_CONTAINER (menu_bar)->border_width +
			    BORDER_SPACING);
      
      if (get_shadow_type (menu_bar) != GTK_SHADOW_NONE)
	{
	  child_allocation.x += widget->style->xthickness;
	  child_allocation.y += widget->style->ythickness;
	}
      
      if (pack_direction == GTK_PACK_DIRECTION_LTR ||
	  pack_direction == GTK_PACK_DIRECTION_RTL)
	{
	  child_allocation.height = MAX (1, (gint)allocation->height - child_allocation.y * 2);
	  
	  offset = child_allocation.x; 	/* Window edge to menubar start */
	  ltr_x = child_allocation.x;
	  
	  if(priv->show_arrow){
			if(allocation->width < priv->true_requisition.width){
				if((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_LTR)){
					priv->need_arrow = TRUE;
					adjusted.width -= arrow_requisition.width;
					arrow_allocation.height = adjusted.height;
					arrow_allocation.x = adjusted.width;
					arrow_allocation.y = 0;
				} else {
					priv->need_arrow = TRUE;
					adjusted.x = arrow_requisition.width;
					adjusted.width -= arrow_requisition.width;
					arrow_allocation.height = adjusted.height;
					arrow_allocation.x = 0;
					arrow_allocation.y = 0;
				}
			}
	  }
	  children = menu_shell->children;
	  while (children)
	    {
	      gint toggle_size;          
	      
	      child = children->data;
	      children = children->next;
	      
	      gtk_menu_item_toggle_size_request (GTK_MENU_ITEM (child),
						 &toggle_size);
	      gtk_widget_get_child_requisition (child, &child_requisition);

		  if(priv->show_arrow){
				menu_info = g_hash_table_lookup(priv->menu_items, child);
				menu_info->overflowed = (ltr_x + child_requisition.width/2
										> adjusted.width);
		  }
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
			if(priv->show_arrow){
					if(menu_info->overflowed){ /*move it away & clip it.*/
						child_allocation.width = 0;
						child_allocation.x = allocation->width;

					} else { /*use remaining width if there isn't enought*/
						child_allocation.width = MIN(
							child_requisition.width,
							(int)adjusted.width - (int)ltr_x);
						if ((direction == GTK_TEXT_DIR_LTR) 
							== (pack_direction == GTK_PACK_DIRECTION_LTR))
							child_allocation.x = ltr_x;
						else
							child_allocation.x = adjusted.width
											- child_allocation.width - ltr_x; 
						child_allocation.x += adjusted.x;
					}
			} else{
		  if ((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_LTR))
		    child_allocation.x = ltr_x;
		  else
		    child_allocation.x = allocation->width -
		      child_requisition.width - ltr_x; 
		  child_allocation.width = child_requisition.width;
			} 
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
	  if(priv->show_arrow){
			if(allocation->height < priv->true_requisition.height){
				if((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_TTB)){
					priv->need_arrow = TRUE;
					adjusted.height -= arrow_requisition.height;
					arrow_allocation.width = adjusted.width;
					arrow_allocation.y = adjusted.height;
					arrow_allocation.x = 0;
				} else {
					priv->need_arrow = TRUE;
					adjusted.y = arrow_requisition.height;
					adjusted.height -= arrow_requisition.height;
					arrow_allocation.width = adjusted.width;
					arrow_allocation.y = 0;
					arrow_allocation.x = 0;
				}
			}
	  }
	  children = menu_shell->children;
	  while (children)
	    {
	      gint toggle_size;          
	      
	      child = children->data;
	      children = children->next;
		  if(priv->show_arrow){
				menu_info = g_hash_table_lookup(priv->menu_items, child);
				menu_info->overflowed = (ltr_y + child_requisition.height/2
										> adjusted.height);
		  }
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
			if(priv->show_arrow){
					if(menu_info->overflowed){ /*move it aw.x & clip it.*/
						child_allocation.height = 0;
						child_allocation.y = allocation->height;

					} else { /*use remaining height if there isn't enough*/
						child_allocation.height = MIN(
							child_requisition.height,
							(int)adjusted.height - (int)ltr_y);
						if ((direction == GTK_TEXT_DIR_LTR) 
							== (pack_direction == GTK_PACK_DIRECTION_TTB))
							child_allocation.y = ltr_y;
						else {
							child_allocation.y = adjusted.height
											- child_allocation.height - ltr_y; 
						}
						child_allocation.y += adjusted.y;
						LOG("a %d %d %d %d", child_allocation);
						LOG("r %d %d ", child_requisition);
						LOG("l %d ", ltr_y);
					}
			}else{
		  if ((direction == GTK_TEXT_DIR_LTR) ==
		      (pack_direction == GTK_PACK_DIRECTION_TTB))
		    child_allocation.y = ltr_y;
		  else
		    child_allocation.y = allocation->height -
		      child_requisition.height - ltr_y; 
		  child_allocation.height = child_requisition.height;
			}
		  gtk_menu_item_toggle_size_allocate (GTK_MENU_ITEM (child),
						      toggle_size);
		  gtk_widget_size_allocate (child, &child_allocation);
		  
		  ltr_y += child_allocation.height;
		}
	    }
	}
    }
	if(priv->show_arrow && priv->need_arrow) {
		LOG("arrow_allocation: %d %d %d %d", arrow_allocation);
		gtk_widget_size_allocate(priv->arrow_button, &arrow_allocation);
		gtk_widget_show_all(priv->arrow_button);
	} else {
		gtk_widget_hide(priv->arrow_button);
	}
}

static void
gnomenu_menu_bar_paint (GtkWidget    *widget,
                    GdkRectangle *area)
{
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
gnomenu_menu_bar_expose (GtkWidget      *widget,
		     GdkEventExpose *event)
{
  g_return_val_if_fail (GTK_IS_MENU_BAR (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      gnomenu_menu_bar_paint (widget, &event->area);

      (* GTK_WIDGET_CLASS (_menu_shell_class)->expose_event) (widget, event);
    }

  return FALSE;
}
static GdkWindow * _get_toplevel_gdk_window(GtkMenuBar * menubar){
	GtkWidget * toplevel;
	GdkWindow * window;

	toplevel = gtk_widget_get_toplevel(GTK_WIDGET(menubar));
	g_return_val_if_fail(toplevel!=NULL, NULL);

	window= GTK_WIDGET(toplevel)->window;
	g_return_val_if_fail(window!=NULL, NULL);
	return window;
}
static void _send_refresh_global_menu_sms (GtkMenuBar * menubar){
	gchar * sms;
	GdkWindow * window = _get_toplevel_gdk_window(menubar);

	sms = g_strdup_printf("menu %p", GDK_WINDOW_XWINDOW(window));
	gdkx_tools_send_sms(sms, strlen(sms)+1);
	g_free(sms);
}
static void _invalidate_introspection ( GtkMenuBar * menubar){
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	GtkWidget * toplevel;
	priv->introspection_is_dirty = TRUE;

	toplevel = gtk_widget_get_toplevel(GTK_WIDGET(menubar));
	g_return_if_fail(toplevel!= NULL);
	if(gtk_window_has_toplevel_focus(GTK_WINDOW(toplevel))){
		_update_introspection(menubar);
		_send_refresh_global_menu_sms(menubar);
	}
}
static void _s_proxy_activate(GtkWidget * proxy, GtkWidget * item){
	g_signal_emit_by_name(item, "activate");
}
static void _setup_proxy(GtkWidget * item, GtkMenuBar * menubar){
	GtkMenuItem * proxy;
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);

	proxy = builder_get_object(priv->builder, gtk_widget_get_id(item));
	gtk_container_remove(gtk_widget_get_parent(proxy), proxy);
	g_object_set_data(item, "menu-item-proxy", proxy);
	g_object_set_data(proxy, "menu-item", item);
	g_signal_connect(proxy, "activate", _s_proxy_activate, _get_item_for_proxy(menubar, proxy));
}
static gchar * _update_introspection ( GtkMenuBar * menubar){
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	GdkWindow * window;
	if(priv->introspection_is_dirty){
		if(priv->introspection) g_free(priv->introspection);
		priv->introspection = gtk_widget_introspect_with_handle(GTK_WIDGET(menubar));
		window = _get_toplevel_gdk_window(menubar);
		if(window)
			gdkx_tools_set_window_prop_blocked(window , 
					gdk_atom_intern("GNOMENU_MENU_BAR", FALSE), 
					priv->introspection, 
					strlen(priv->introspection)+1);
		if(priv->builder) builder_destroy(priv->builder);
		priv->builder = builder_new();
		builder_parse(priv->builder, priv->introspection);
		gtk_container_foreach(menubar, _setup_proxy, menubar);
		priv->introspection_is_dirty = FALSE;
	}
	return priv->introspection;
}
static void _s_notify_has_toplevel_focus ( GtkMenuBar * menubar, GParamSpec * pspec, GtkWindow * window){
	if(gnomenu_menu_bar_get_is_global_menu(menubar)){
		if(gtk_window_has_toplevel_focus(window)){
			LOG("received top level focus %p", menubar);

			_update_introspection(menubar);
			_send_refresh_global_menu_sms(menubar);
			gdkx_tools_thaw_sms_filter(_sms_filter, menubar);
		}  else {
			gdkx_tools_freeze_sms_filter(_sms_filter, menubar);
		
		}
	}
}
static void _update_widget_id(GtkMenuBar * menubar){
  GtkWidget *toplevel;  
  toplevel = gtk_widget_get_toplevel (GTK_WIDGET(menubar));

	gchar * buffer = g_strdup_printf("%p", GDK_WINDOW_XWINDOW(toplevel->window));
	gtk_widget_set_id(GTK_WIDGET(menubar), buffer);
	g_free(buffer);
	_invalidate_introspection(menubar);
}
static void
_s_hierarchy_changed (GtkWidget *widget,
				GtkWidget *old_toplevel, gpointer data)
{
  GtkWidget *toplevel;  
  GtkMenuBar * menubar = GTK_MENU_BAR(widget);

  toplevel = gtk_widget_get_toplevel (widget);

  if (old_toplevel) {
		g_signal_handlers_disconnect_by_func(old_toplevel,
				_s_toplevel_realize, menubar);
		g_signal_handlers_disconnect_by_func(
			old_toplevel, _s_notify_has_toplevel_focus, menubar);
  }
  
  if (GTK_WIDGET_TOPLEVEL (toplevel)) {
	if(GTK_WIDGET_REALIZED(toplevel)){
		_update_widget_id(menubar);
	}
	g_signal_connect_swapped(toplevel, "realize",
				G_CALLBACK(_s_toplevel_realize), menubar);
	g_signal_connect_swapped(toplevel, "notify::has-toplevel-focus",
		G_CALLBACK(_s_notify_has_toplevel_focus), menubar);
  
  }
	
}
static void
_s_toplevel_realize (GtkMenuBar * menubar, GtkWidget * toplevel){
	_update_widget_id(menubar);
}
static void _sms_filter ( GtkMenuBar * menubar, gchar * sms, gint size) {
	LOG("received sms: %s", sms);
}

static GtkShadowType
get_shadow_type (GtkMenuBar *menubar)
{
  GtkShadowType shadow_type = GTK_SHADOW_OUT;
  
  gtk_widget_style_get (GTK_WIDGET (menubar),
			"shadow-type", &shadow_type,
			NULL);

  return shadow_type;
}

static gint
gnomenu_menu_bar_get_popup_delay (GtkMenuShell *menu_shell)
{
  gint popup_delay;
  
  g_object_get (gtk_widget_get_settings (GTK_WIDGET (menu_shell)),
		"gtk-menu-bar-popup-delay", &popup_delay,
		NULL);

  return popup_delay;
}


/**
 * gnomenu_menu_bar_get_pack_direction:
 * @menubar: a #GnomenuMenuBar
 * 
 * Retrieves the current pack direction of the menubar. See
 * gnomenu_menu_bar_set_pack_direction().
 *
 * Return value: the pack direction
 *
 * Since: 2.8
 **/
GtkPackDirection
gnomenu_menu_bar_get_pack_direction (GtkMenuBar *menubar)
{
  GnomenuMenuBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_MENU_BAR (menubar), 
			GTK_PACK_DIRECTION_LTR);
  
  return gtk_menu_bar_get_pack_direction(menubar);
}

/**
 * gnomenu_menu_bar_set_pack_direction:
 * @menubar: a #GnomenuMenuBar.
 * @pack_dir: a new #GtkPackDirection.
 * 
 * Sets how items should be packed inside a menubar.
 * 
 * Since: 2.8
 **/
void
gnomenu_menu_bar_set_pack_direction (GtkMenuBar       *menubar,
                                 GtkPackDirection  pack_dir)
{
  GnomenuMenuBarPrivate *priv;
  GList *l;

  g_return_if_fail (GTK_IS_MENU_BAR (menubar));

  gtk_menu_bar_set_pack_direction(menubar, pack_dir);
}

/**
 * gnomenu_menu_bar_get_child_pack_direction:
 * @menubar: a #GnomenuMenuBar
 * 
 * Retrieves the current child pack direction of the menubar. See
 * gnomenu_menu_bar_set_child_pack_direction().
 *
 * Return value: the child pack direction
 *
 * Since: 2.8
 **/
GtkPackDirection
gnomenu_menu_bar_get_child_pack_direction (GtkMenuBar *menubar)
{
  GnomenuMenuBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_MENU_BAR (menubar), 
			GTK_PACK_DIRECTION_LTR);
  
  priv = GNOMENU_MENU_BAR_GET_PRIVATE (menubar);

  return gtk_menu_bar_get_child_pack_direction(menubar);
}

/**
 * gnomenu_menu_bar_set_child_pack_direction:
 * @menubar: a #GnomenuMenuBar.
 * @child_pack_dir: a new #GtkPackDirection.
 * 
 * Sets how widgets should be packed inside the children of a menubar.
 * 
 * Since: 2.8
 **/
void
gnomenu_menu_bar_set_child_pack_direction (GtkMenuBar       *menubar,
                                       GtkPackDirection  child_pack_dir)
{
  GnomenuMenuBarPrivate *priv;
  GList *l;

  g_return_if_fail (GTK_IS_MENU_BAR (menubar));

  priv = GNOMENU_MENU_BAR_GET_PRIVATE (menubar);

  gtk_menu_bar_set_child_pack_direction(menubar, child_pack_dir);
}
static GObject* _constructor(GType type, 
		guint n_construct_properties,
		GObjectConstructParam *construct_params){

	GObject * object = (G_OBJECT_CLASS(_menu_shell_class)->constructor)(
		type, n_construct_properties, construct_params);

	GET_OBJECT(object, menu_bar, priv);

	gtk_container_set_border_width(GTK_CONTAINER(object), 0);

	gtk_widget_show(priv->arrow);
	gtk_container_add(GTK_CONTAINER(priv->arrow_button), priv->arrow);
	gtk_button_set_relief(GTK_BUTTON(priv->arrow_button), GTK_RELIEF_NONE);
	gtk_widget_set_parent(priv->arrow_button, GTK_WIDGET(menu_bar));

	g_signal_connect_swapped(G_OBJECT(priv->arrow_button), "clicked",
				G_CALLBACK(_s_arrow_button_clicked), menu_bar);
	g_signal_connect_swapped(G_OBJECT(priv->popup_menu), "deactivate",
				G_CALLBACK(_s_popup_menu_deactivated), menu_bar);

	g_signal_connect(object, "hierarchy-changed",
				G_CALLBACK(_s_hierarchy_changed), NULL);
	gdkx_tools_add_sms_filter_frozen(_sms_filter, menu_bar);
	return object;
}
gboolean
gnomenu_menu_bar_get_is_global_menu(GtkMenuBar * menubar){
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	return priv->is_global_menu;
}
static void
gnomenu_menu_bar_set_is_global_menu(GtkMenuBar * menubar, gboolean is_global_menu){
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	priv->is_global_menu = is_global_menu;
}
void 
gnomenu_menu_bar_set_show_arrow(GtkMenuBar * menubar, gboolean show_arrow) {
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	priv->show_arrow = show_arrow;
	gtk_widget_queue_resize(GTK_WIDGET(menubar));
}
gboolean 
gnomenu_menu_bar_get_show_arrow(GtkMenuBar * menubar) {
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	return priv->show_arrow;

}

static void
_dispose (GObject * _object){
	GET_OBJECT(_object, menu_bar, priv);
	if(!priv->disposed){
		priv->disposed = TRUE;	
		g_hash_table_remove_all(priv->menu_items);
		gdkx_tools_remove_sms_filter(_sms_filter, menu_bar);
	}
	G_OBJECT_CLASS(_menu_shell_class)->dispose(_object);
}
static void
_finalize(GObject * _object){
	GET_OBJECT(_object, menu_bar, priv);	
	gtk_widget_unparent(priv->arrow_button);
	g_hash_table_destroy(priv->menu_items);
	gtk_widget_destroy(GTK_WIDGET(priv->popup_menu));
	if(priv->introspection) g_free(priv->introspection);
	if(priv->builder) builder_destroy(priv->builder);
	G_OBJECT_CLASS(_menu_shell_class)->finalize(_object);
}
static void
_insert (GtkMenuShell * menu_shell, GtkWidget * widget, gint pos){
	GtkRequisition req;
	MenuItemInfo * item_info = g_new0(MenuItemInfo, 1);

	GET_OBJECT(menu_shell, menu_bar, priv);
	GTK_MENU_SHELL_CLASS(_menu_shell_class)->insert(menu_shell, widget, pos);
	if(GTK_IS_MENU_ITEM(widget)){
		item_info->menu_item = GTK_MENU_ITEM(widget);
	}
	g_hash_table_insert(priv->menu_items, widget, item_info);
}
static void
_remove (GtkContainer * container, GtkWidget * widget){
	GtkRequisition req;

	GET_OBJECT(container, menu_bar, priv);
	GTK_CONTAINER_CLASS(_menu_shell_class)->remove(container, widget);

	g_hash_table_remove(priv->menu_items, widget);
}
static void _forall					( GtkContainer    *container,
									  gboolean     include_internals,
									  GtkCallback      callback,
									  gpointer     callback_data){
	GET_OBJECT(container, menu_bar, priv);
	GTK_CONTAINER_CLASS(_menu_shell_class)->forall(container, 
					include_internals, callback, callback_data);
	
	if(include_internals){
		callback(priv->arrow_button, callback_data);
	}
}
static GtkMenuItem * _get_item_for_proxy(GtkMenuBar * menubar, GtkMenuItem * proxy){
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	GtkMenuItem * item = g_object_get_data(proxy, "menu-item");
	return item;
}
GtkMenuItem * _get_proxy_for_item(GtkMenuBar * menubar, GtkMenuItem * item){
	GnomenuMenuBarPrivate * priv = GNOMENU_MENU_BAR_GET_PRIVATE(menubar);
	GtkMenuItem * proxy = g_object_get_data(item, "menu-item-proxy");
	return proxy;
}
static void _remove_child ( GtkWidget * widget, GtkContainer * container){ 
    gtk_container_remove(container, widget); 
}

static void _build_popup_menu 	(GnomenuMenuBar * self){
	GET_OBJECT(self, menu_bar, priv);
	GList * list;
	GList * node;
	gtk_container_foreach(GTK_CONTAINER(priv->popup_menu), (GtkCallback)_remove_child, priv->popup_menu);
	
	list = GTK_MENU_SHELL(self)->children;
	for(node = g_list_first(list); node; node = g_list_next(node)){
		GtkWidget * child = node->data;
		MenuItemInfo * info = g_hash_table_lookup(priv->menu_items, child);
		if(info->overflowed) {
			GtkMenuItem * proxy = _get_proxy_for_item(self, info->menu_item);
			if(proxy) {
				gtk_menu_shell_append(GTK_MENU_SHELL(priv->popup_menu), GTK_WIDGET(proxy));
			}
		}
	}
}
static void _s_arrow_button_clicked		( GtkWidget * self,
									  GtkWidget * arrow_button){
	GET_OBJECT(self, menu_bar, priv);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->arrow_button)) &&
     !GTK_WIDGET_VISIBLE (priv->popup_menu)) {
		_build_popup_menu(menu_bar);
		gtk_menu_popup(priv->popup_menu, NULL, NULL, 
			NULL, NULL, 0, gtk_get_current_event_time());
		gtk_menu_shell_select_first (GTK_MENU_SHELL (priv->popup_menu), FALSE);
	
	}
}
static void _s_popup_menu_deactivated	( GtkWidget * menubar,
									  GtkWidget * popup_menu){
	GET_OBJECT(menubar, menu_bar, priv);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->arrow_button), FALSE);
}
