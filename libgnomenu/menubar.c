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
#define OVERFLOWED_ITEMS

#define GET_OBJECT(_s, sgmb, p) \
	GnomenuMenuBar * sgmb = GNOMENU_MENU_BAR(_s); \
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
#include "menubar.h"

#define BORDER_SPACING  0
#define DEFAULT_IPADDING 1

/* Properties */
enum {
  PROP_0,
  PROP_PACK_DIRECTION,
  PROP_CHILD_PACK_DIRECTION
};

typedef struct _GnomenuMenuBarPrivate GnomenuMenuBarPrivate;
struct _GnomenuMenuBarPrivate
{
  GtkPackDirection _pack_direction; /*comaptibility with GtkMenuBar, never directly used*/
  GtkPackDirection _child_pack_direction; /*same as above*/

#ifdef OVERFLOWED_ITEMS
	gboolean disposed;
	gboolean detached;

	GHashTable * menu_items;
	GtkWidget * arrow_button;
	GtkWidget * arrow;
	gboolean show_arrow;
	GtkMenu	* popup_menu;
	GtkRequisition true_requisition;
#endif
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
static void gnomenu_menu_bar_hierarchy_changed (GtkWidget       *widget,
					    GtkWidget       *old_toplevel);
static gint gnomenu_menu_bar_get_popup_delay   (GtkMenuShell    *menu_shell);

#ifdef OVERFLOWED_ITEMS
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
	GtkMenuItem * proxy;
} MenuItemInfo;
#endif

static GtkShadowType get_shadow_type   (GnomenuMenuBar      *menubar);

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
#ifdef OVERFLOWED_ITEMS
	GtkContainerClass *container_class;
#endif

  GtkBindingSet *binding_set;

  gobject_class = (GObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  menu_shell_class = (GtkMenuShellClass*) class;
#ifdef OVERFLOWED_ITEMS
	container_class = (GtkContainerClass*) class;
#endif

  gobject_class->get_property = gnomenu_menu_bar_get_property;
  gobject_class->set_property = gnomenu_menu_bar_set_property;


  widget_class->size_request = gnomenu_menu_bar_size_request;
  widget_class->size_allocate = gnomenu_menu_bar_size_allocate;
  widget_class->expose_event = gnomenu_menu_bar_expose;
 // widget_class->hierarchy_changed = gnomenu_menu_bar_hierarchy_changed;
  
  menu_shell_class->submenu_placement = GTK_TOP_BOTTOM;
  menu_shell_class->get_popup_delay = gnomenu_menu_bar_get_popup_delay;

#ifdef OVERFLOWED_ITEMS
	gobject_class->finalize = _finalize;
	gobject_class->constructor = _constructor;
	menu_shell_class->insert = _insert;
	container_class->remove = _remove;
	container_class->forall = _forall;
#endif
  g_type_class_add_private (gobject_class, sizeof (GnomenuMenuBarPrivate));  
}

#ifdef OVERFLOWED_ITEMS
static void _menu_item_info_free(MenuItemInfo * info){
	if(info->proxy) g_object_unref(info->proxy);
	g_free(info);
}
#endif

void
gnomenu_menu_bar_init (GnomenuMenuBar *object)
{
#ifdef OVERFLOWED_ITEMS
	GET_OBJECT(object, menu_bar, priv);
	priv->menu_items = g_hash_table_new_full(NULL, NULL, NULL, _menu_item_info_free);
	priv->popup_menu = GTK_MENU(gtk_menu_new());
	priv->arrow_button = GTK_WIDGET(gtk_toggle_button_new());
	priv->arrow = gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_NONE);
	priv->show_arrow = FALSE;
	priv->disposed = FALSE;
	priv->detached = FALSE;
#endif
}

GtkWidget*
gnomenu_menu_bar_new (void)
{
  return g_object_new (GNOMENU_TYPE_MENU_BAR, NULL);
}

static void
gnomenu_menu_bar_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  GnomenuMenuBar *menubar = GNOMENU_MENU_BAR (object);
  
  switch (prop_id)
    {
    case PROP_PACK_DIRECTION:
      gnomenu_menu_bar_set_pack_direction (menubar, g_value_get_enum (value));
      break;
    case PROP_CHILD_PACK_DIRECTION:
      gnomenu_menu_bar_set_child_pack_direction (menubar, g_value_get_enum (value));
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
  GnomenuMenuBar *menubar = GNOMENU_MENU_BAR (object);
  
  switch (prop_id)
    {
    case PROP_PACK_DIRECTION:
      g_value_set_enum (value, gnomenu_menu_bar_get_pack_direction (menubar));
      break;
    case PROP_CHILD_PACK_DIRECTION:
      g_value_set_enum (value, gnomenu_menu_bar_get_child_pack_direction (menubar));
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
  GnomenuMenuBar *menu_bar;
  GnomenuMenuBarPrivate *priv;
  GtkMenuShell *menu_shell;
  GtkWidget *child;
  GList *children;
  gint nchildren;
  GtkRequisition child_requisition;
  gint ipadding;
  GtkPackDirection child_pack_direction;
  GtkPackDirection pack_direction;

  g_return_if_fail (GNOMENU_IS_MENU_BAR (widget));
  g_return_if_fail (requisition != NULL);

  requisition->width = 0;
  requisition->height = 0;

  if (GTK_WIDGET_VISIBLE (widget))
    {
      menu_bar = GNOMENU_MENU_BAR (widget);
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
#ifdef OVERFLOWED_ITEMS
    priv->true_requisition = *requisition;
	{
	GtkPackDirection pack_direction;
	pack_direction = gnomenu_menu_bar_get_pack_direction(GNOMENU_MENU_BAR(widget));	
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
#endif
}

static void
gnomenu_menu_bar_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation)
{
  GnomenuMenuBar *menu_bar;
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

#ifdef OVERFLOWED_ITEMS
	GtkAllocation adjusted;
	MenuItemInfo * menu_info;
	GtkRequisition arrow_requisition;
	GtkAllocation arrow_allocation;
#endif

  g_return_if_fail (GNOMENU_IS_MENU_BAR (widget));
  g_return_if_fail (allocation != NULL);

  menu_bar = GNOMENU_MENU_BAR (widget);
  menu_shell = GTK_MENU_SHELL (widget);
  priv = GNOMENU_MENU_BAR_GET_PRIVATE (menu_bar);

	pack_direction = gnomenu_menu_bar_get_pack_direction(menu_bar);
	child_pack_direction = gnomenu_menu_bar_get_child_pack_direction(menu_bar);
#ifdef OVERFLOWED_ITEMS
	adjusted = * allocation;
	adjusted.x = 0; /*the x, y offset is taken care by either widget->window or priv->floater*/
	adjusted.y = 0;
	gtk_widget_size_request(priv->arrow_button, &arrow_requisition);

	arrow_allocation.width = arrow_requisition.width;
	arrow_allocation.height = arrow_requisition.height;
	priv->show_arrow = FALSE;
#endif
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
	  
#ifdef OVERFLOWED_ITEMS
			if(allocation->width < priv->true_requisition.width){
				if((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_LTR)){
					priv->show_arrow = TRUE;
					adjusted.width -= arrow_requisition.width;
					arrow_allocation.height = adjusted.height;
					arrow_allocation.x = adjusted.width;
					arrow_allocation.y = 0;
				} else {
					priv->show_arrow = TRUE;
					adjusted.x = arrow_requisition.width;
					adjusted.width -= arrow_requisition.width;
					arrow_allocation.height = adjusted.height;
					arrow_allocation.x = 0;
					arrow_allocation.y = 0;
				}
			}
#endif
	  children = menu_shell->children;
	  while (children)
	    {
	      gint toggle_size;          
	      
	      child = children->data;
	      children = children->next;
	      
	      gtk_menu_item_toggle_size_request (GTK_MENU_ITEM (child),
						 &toggle_size);
	      gtk_widget_get_child_requisition (child, &child_requisition);

#ifdef OVERFLOWED_ITEMS
				menu_info = g_hash_table_lookup(priv->menu_items, child);
				menu_info->overflowed = (ltr_x + child_requisition.width/2
										> adjusted.width);
#endif
	    
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
#ifdef OVERFLOWED_ITEMS
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
#else
		  if ((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_LTR))
		    child_allocation.x = ltr_x;
		  else
		    child_allocation.x = allocation->width -
		      child_requisition.width - ltr_x; 
		  child_allocation.width = child_requisition.width;
#endif		  
		  
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
#ifdef OVERFLOWED_ITEMS
			if(allocation->height < priv->true_requisition.height){
				if((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_TTB)){
					priv->show_arrow = TRUE;
					adjusted.height -= arrow_requisition.height;
					arrow_allocation.width = adjusted.width;
					arrow_allocation.y = adjusted.height;
					arrow_allocation.x = 0;
				} else {
					priv->show_arrow = TRUE;
					adjusted.y = arrow_requisition.height;
					adjusted.height -= arrow_requisition.height;
					arrow_allocation.width = adjusted.width;
					arrow_allocation.y = 0;
					arrow_allocation.x = 0;
				}
			}
#endif 
	  children = menu_shell->children;
	  while (children)
	    {
	      gint toggle_size;          
	      
	      child = children->data;
	      children = children->next;
#ifdef OVERFLOWED_ITEMS 
				menu_info = g_hash_table_lookup(priv->menu_items, child);
				menu_info->overflowed = (ltr_y + child_requisition.height/2
										> adjusted.height);
#endif
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
#ifdef OVERFLOWED_ITEMS
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
#else
		  if ((direction == GTK_TEXT_DIR_LTR) ==
		      (pack_direction == GTK_PACK_DIRECTION_TTB))
		    child_allocation.y = ltr_y;
		  else
		    child_allocation.y = allocation->height -
		      child_requisition.height - ltr_y; 
		  child_allocation.height = child_requisition.height;
#endif	  
		  gtk_menu_item_toggle_size_allocate (GTK_MENU_ITEM (child),
						      toggle_size);
		  gtk_widget_size_allocate (child, &child_allocation);
		  
		  ltr_y += child_allocation.height;
		}
	    }
	}
    }
#ifdef OVERFLOWED_ITEMS
	if(priv->show_arrow) {
		LOG("arrow_allocation: %d %d %d %d", arrow_allocation);
		gtk_widget_size_allocate(priv->arrow_button, &arrow_allocation);
		gtk_widget_show_all(priv->arrow_button);
	} else {
		gtk_widget_hide(priv->arrow_button);
	}
	LOG("show arrow = %d", priv->show_arrow);
#endif
}

static void
gnomenu_menu_bar_paint (GtkWidget    *widget,
                    GdkRectangle *area)
{
  g_return_if_fail (GNOMENU_IS_MENU_BAR (widget));

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      gint border;

      border = GTK_CONTAINER (widget)->border_width;
      
      gtk_paint_box (widget->style,
		     widget->window,
                     GTK_WIDGET_STATE (widget),
                     get_shadow_type (GNOMENU_MENU_BAR (widget)),
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
  g_return_val_if_fail (GNOMENU_IS_MENU_BAR (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      gnomenu_menu_bar_paint (widget, &event->area);

      (* GTK_WIDGET_CLASS (_menu_shell_class)->expose_event) (widget, event);
    }

  return FALSE;
}


static void
gnomenu_menu_bar_hierarchy_changed (GtkWidget *widget,
				GtkWidget *old_toplevel)
{
  GtkWidget *toplevel;  
  GnomenuMenuBar *menubar;

  menubar = GNOMENU_MENU_BAR (widget);

  toplevel = gtk_widget_get_toplevel (widget);

  if (old_toplevel) {
  
  }
  
  if (GTK_WIDGET_TOPLEVEL (toplevel)) {
  
  }
	
}

static GtkShadowType
get_shadow_type (GnomenuMenuBar *menubar)
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
gnomenu_menu_bar_get_pack_direction (GnomenuMenuBar *menubar)
{
  GnomenuMenuBarPrivate *priv;

  g_return_val_if_fail (GNOMENU_IS_MENU_BAR (menubar), 
			GTK_PACK_DIRECTION_LTR);
  
  return gtk_menu_bar_get_pack_direction(GTK_MENU_BAR(menubar));
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
gnomenu_menu_bar_set_pack_direction (GnomenuMenuBar       *menubar,
                                 GtkPackDirection  pack_dir)
{
  GnomenuMenuBarPrivate *priv;
  GList *l;

  g_return_if_fail (GNOMENU_IS_MENU_BAR (menubar));

  gtk_menu_bar_set_pack_direction(GTK_MENU_BAR(menubar), pack_dir);
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
gnomenu_menu_bar_get_child_pack_direction (GnomenuMenuBar *menubar)
{
  GnomenuMenuBarPrivate *priv;

  g_return_val_if_fail (GNOMENU_IS_MENU_BAR (menubar), 
			GTK_PACK_DIRECTION_LTR);
  
  priv = GNOMENU_MENU_BAR_GET_PRIVATE (menubar);

  return gtk_menu_bar_get_child_pack_direction(GTK_MENU_BAR(menubar));
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
gnomenu_menu_bar_set_child_pack_direction (GnomenuMenuBar       *menubar,
                                       GtkPackDirection  child_pack_dir)
{
  GnomenuMenuBarPrivate *priv;
  GList *l;

  g_return_if_fail (GNOMENU_IS_MENU_BAR (menubar));

  priv = GNOMENU_MENU_BAR_GET_PRIVATE (menubar);

  gtk_menu_bar_set_child_pack_direction(GTK_MENU_BAR(menubar), child_pack_dir);
}
#ifdef OVERFLOWED_ITEMS
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
	
	return object;
}

static void
_dispose (GObject * _object){
	GET_OBJECT(_object, menu_bar, priv);
	if(!priv->disposed){
		priv->disposed = TRUE;	
		g_hash_table_remove_all(priv->menu_items);
	}
	G_OBJECT_CLASS(_menu_shell_class)->dispose(_object);
}
static void
_finalize(GObject * _object){
	GET_OBJECT(_object, menu_bar, priv);	
	gtk_widget_unparent(priv->arrow_button);
	g_hash_table_destroy(priv->menu_items);
	gtk_widget_destroy(GTK_WIDGET(priv->popup_menu));
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
static GtkMenuItem * _get_item_for_proxy(GnomenuMenuBar * self, GtkMenuItem * proxy){
	GET_OBJECT(self, menu_bar, priv);
	GList * list = GTK_MENU_SHELL(self)->children;
	GList * node;
	for(node = g_list_first(list); node; node = g_list_next(node)){
		GtkMenuItem * item = node->data;
		MenuItemInfo * info = g_hash_table_lookup(priv->menu_items, item);
		if(info && info->proxy == proxy){
			return item;
		}
	}
}
GtkMenuItem * _get_proxy_for_item( GnomenuMenuBar * self, GtkMenuItem * item){
	GET_OBJECT(self, menu_bar, priv);

	GtkWidget * label;
	MenuItemInfo * info = g_hash_table_lookup(priv->menu_items, item);
	const gchar * text = gtk_widget_get_name(GTK_WIDGET(item));
	GtkWidget * child = gtk_bin_get_child(GTK_BIN(item));

	if(G_OBJECT_TYPE(item) != GTK_TYPE_MENU_ITEM
	&& G_OBJECT_TYPE(item) != GTK_TYPE_IMAGE_MENU_ITEM){
/* Can't handle any other subclass of GtkMenuItem */
		return NULL;
	}
	if(GTK_IS_LABEL(child)){
		text = gtk_label_get_label(GTK_LABEL(child));
	} else {
		LOG("unhandled child:%s", G_OBJECT_TYPE_NAME(child));
	}

	if(!info->proxy) {
		LOG("menuitem type: %s", G_OBJECT_TYPE_NAME(item));
		
	/* The image is then lost.*/
		if(GTK_IS_IMAGE_MENU_ITEM(item)){
			GtkImage * image = gtk_image_menu_item_get_image(item);
			GtkImage * dup = NULL;
			switch(gtk_image_get_storage_type(image)){
				case GTK_IMAGE_EMPTY:
				case GTK_IMAGE_PIXMAP:
				case GTK_IMAGE_IMAGE:
				case GTK_IMAGE_PIXBUF:
				break;
				case GTK_IMAGE_STOCK: {
					gchar * stock_id;
					GtkIconSize size;
					gtk_image_get_stock (
						image, &stock_id, &size);
					dup = gtk_image_new_from_stock(
						stock_id, size);
				}
				break;
				case GTK_IMAGE_ICON_SET:
				case GTK_IMAGE_ANIMATION:
				case GTK_IMAGE_ICON_NAME:
				break;
			}
			info->proxy = GTK_MENU_ITEM(gtk_image_menu_item_new());
			gtk_image_menu_item_set_image(info->proxy, dup);
		} else 
			info->proxy = GTK_MENU_ITEM(gtk_menu_item_new());
		g_object_ref(info->proxy);
	} else {
		gtk_container_remove(GTK_CONTAINER(info->proxy),
				gtk_bin_get_child(GTK_BIN(info->proxy)));
	} 

	LOG("text = %s", text);
	label = gtk_label_new_with_mnemonic(text);
	gtk_container_add(GTK_CONTAINER(info->proxy), label);
	
	return info->proxy;	
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
				if(GTK_WIDGET_VISIBLE(info->menu_item)) 
					gtk_widget_show_all(proxy);
				else
					gtk_widget_hide_all(proxy);
			}
		}
	}
}
static void _steal_submenu(GtkMenuItem * proxy, GnomenuMenuBar * self){
	GtkMenuItem * item = _get_item_for_proxy(self, proxy);
	GtkMenu * submenu = GTK_MENU(gtk_menu_item_get_submenu(item));
	if(submenu){
		g_object_ref(submenu);
		gtk_menu_detach(submenu);
		gtk_menu_item_set_submenu(proxy, GTK_WIDGET(submenu));	
		g_object_unref(submenu);
	}
}
static void _return_submenu(GtkMenuItem * proxy, GnomenuMenuBar * self){
	GtkMenuItem * item = _get_item_for_proxy(self, proxy);
	GtkMenu * submenu = GTK_MENU(gtk_menu_item_get_submenu(proxy));
	if(submenu){
		g_object_ref(submenu);
		gtk_menu_detach(submenu);
		gtk_menu_item_set_submenu(item, GTK_WIDGET(submenu));	
		g_object_unref(submenu);
	}
}
static void _s_arrow_button_clicked		( GtkWidget * self,
									  GtkWidget * arrow_button){
	GET_OBJECT(self, menu_bar, priv);
	gchar * intro = gtk_widget_introspect(self);
	LOG("introspect: %s", intro);
	g_free(intro);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->arrow_button)) &&
     !GTK_WIDGET_VISIBLE (priv->popup_menu)) {
		_build_popup_menu(menu_bar);
		//gtk_widget_show(GTK_WIDGET(priv->popup_menu));
		gtk_container_foreach(GTK_CONTAINER(priv->popup_menu), (GtkCallback)_steal_submenu, menu_bar);
		gtk_menu_popup(priv->popup_menu, NULL, NULL, 
			NULL, NULL, 0, gtk_get_current_event_time());
		gtk_menu_shell_select_first (GTK_MENU_SHELL (priv->popup_menu), FALSE);
	
	}
}
static void _s_popup_menu_deactivated	( GtkWidget * menubar,
									  GtkWidget * popup_menu){
	GET_OBJECT(menubar, menu_bar, priv);
	gtk_container_foreach(GTK_CONTAINER(priv->popup_menu), (GtkCallback)_return_submenu, menu_bar);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->arrow_button), FALSE);
}
#endif
