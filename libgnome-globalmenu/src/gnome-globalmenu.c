#include <config.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkprivate.h>
#include <gdk/gdkx.h>
#include "gtkintl.h"
#include "gnome-globalmenu.h"

#define GTK_MENU_INTERNALS

#ifndef GDK_WINDOWING_X11
#error this only works under x11 by now
#endif

#define INCLUDE_SOURCE
#include "gtkmenuembed-x11.h"
#undef INCLUDE_SOURCE

#define BORDER_SPACING  0
#define DEFAULT_IPADDING 1

#define LOG_FUNC_NAME g_message("%s invoked", __func__)
/* Properties */
enum {
  PROP_0,
  PROP_PACK_DIRECTION,
  PROP_CHILD_PACK_DIRECTION
};

typedef struct _GtkMenuBarPrivate GtkMenuBarPrivate;
struct _GtkMenuBarPrivate
{
  GtkPackDirection pack_direction;
  GtkPackDirection child_pack_direction;
#ifdef GNOME_GLOBAL_MENU
  GlobalMenuSocket * socket;
  gboolean connected; /*socket is connect-less, so this flag =whether menuserver exists*/
  gboolean globalized;
  gboolean detached;
  GtkRequisition container_requisition;
  GtkAllocation container_allocation;
  GdkWindow * float_window;
  GdkWindow * container_window;
  GdkWindow * master_window;
  gboolean visible;
#endif
};

#define GTK_MENU_BAR_GET_PRIVATE(o)  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_MENU_BAR, GtkMenuBarPrivate))


static void gtk_menu_bar_set_property      (GObject             *object,
					    guint                prop_id,
					    const GValue        *value,
					    GParamSpec          *pspec);
static void gtk_menu_bar_get_property      (GObject             *object,
					    guint                prop_id,
					    GValue              *value,
					    GParamSpec          *pspec);
static void gtk_menu_bar_size_request      (GtkWidget       *widget,
					    GtkRequisition  *requisition);
static void gtk_menu_bar_size_allocate     (GtkWidget       *widget,
					    GtkAllocation   *allocation);
static void gtk_menu_bar_paint             (GtkWidget       *widget,
					    GdkRectangle    *area);
static gint gtk_menu_bar_expose            (GtkWidget       *widget,
					    GdkEventExpose  *event);
static void gtk_menu_bar_hierarchy_changed (GtkWidget       *widget,
					    GtkWidget       *old_toplevel);
static gint gtk_menu_bar_get_popup_delay   (GtkMenuShell    *menu_shell);
static void gtk_menu_bar_move_current      (GtkMenuShell     *menu_shell,
                                            GtkMenuDirectionType direction);
#ifdef GNOME_GLOBAL_MENU
static void gtk_menu_bar_finalize            (GObject         *object);
static void gtk_menu_bar_realize            (GtkWidget         *widget);
static void gtk_menu_bar_unrealize            (GtkWidget         *widget);
static void gtk_menu_bar_map				(GtkWidget * widget);
static void gtk_menu_bar_unmap				(GtkWidget * widget);
static gboolean gtk_menu_bar_motion				(GtkWidget * widget, GdkEventMotion * event);
static gboolean gtk_menu_bar_delete_event				(GtkWidget * widget, GdkEventAny * event);
static void gtk_menu_bar_real_insert (GtkMenuShell * menu_shell, GtkWidget * child, gint position);
static void gtk_menu_bar_notify_size_allocate_cb (GlobalMenuSocket * socket, GlobalMenuNotify * notify, GtkMenuBar * menubar);
static void gtk_menu_bar_notify_server_destroy_cb (GlobalMenuSocket * socket, GlobalMenuNotify * notify, GtkMenuBar * menubar);
static void gtk_menu_bar_notify_server_new_cb (GlobalMenuSocket * socket, GlobalMenuNotify * notify, GtkMenuBar * menubar);
static void gtk_menu_bar_globalize(GtkMenuBar * menubar);
static void gtk_menu_bar_unglobalize(GtkMenuBar * menubar);
static void gtk_menu_bar_connect_to_menu_server(GtkMenuBar * menubar);
static void gtk_menu_bar_detach(GtkMenuBar * menubar);
#endif

static GtkShadowType get_shadow_type   (GtkMenuBar      *menubar);

G_DEFINE_TYPE (GtkMenuBar, gtk_menu_bar, GTK_TYPE_MENU_SHELL)

static void
gtk_menu_bar_class_init (GtkMenuBarClass *class)
{
  GObjectClass *gobject_class;
  GtkWidgetClass *widget_class;
  GtkMenuShellClass *menu_shell_class;

  GtkBindingSet *binding_set;

  gobject_class = (GObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  menu_shell_class = (GtkMenuShellClass*) class;

  gobject_class->get_property = gtk_menu_bar_get_property;
  gobject_class->set_property = gtk_menu_bar_set_property;

  widget_class->size_request = gtk_menu_bar_size_request;
  widget_class->size_allocate = gtk_menu_bar_size_allocate;
  widget_class->expose_event = gtk_menu_bar_expose;
  widget_class->hierarchy_changed = gtk_menu_bar_hierarchy_changed;
  
  menu_shell_class->submenu_placement = GTK_TOP_BOTTOM;
  menu_shell_class->get_popup_delay = gtk_menu_bar_get_popup_delay;
  menu_shell_class->move_current = gtk_menu_bar_move_current;

#ifdef GNOME_GLOBAL_MENU
  gobject_class->finalize = gtk_menu_bar_finalize;
  widget_class->realize = gtk_menu_bar_realize;  
  widget_class->unrealize = gtk_menu_bar_unrealize;  
  widget_class->delete_event = gtk_menu_bar_delete_event;  
  widget_class->map = gtk_menu_bar_map;
  widget_class->unmap = gtk_menu_bar_unmap;
  widget_class->motion_notify_event = gtk_menu_bar_motion;
  menu_shell_class->insert = gtk_menu_bar_real_insert;
#endif
  binding_set = gtk_binding_set_by_class (class);
  gtk_binding_entry_add_signal (binding_set,
				GDK_Left, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_PREV);
  gtk_binding_entry_add_signal (binding_set,
				GDK_KP_Left, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_PREV);
  gtk_binding_entry_add_signal (binding_set,
				GDK_Right, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_NEXT);
  gtk_binding_entry_add_signal (binding_set,
				GDK_KP_Right, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_NEXT);
  gtk_binding_entry_add_signal (binding_set,
				GDK_Up, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_PARENT);
  gtk_binding_entry_add_signal (binding_set,
				GDK_KP_Up, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_PARENT);
  gtk_binding_entry_add_signal (binding_set,
				GDK_Down, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_CHILD);
  gtk_binding_entry_add_signal (binding_set,
				GDK_KP_Down, 0,
				"move_current", 1,
				GTK_TYPE_MENU_DIRECTION_TYPE,
				GTK_MENU_DIR_CHILD);

  /**
   * GtkMenuBar:pack-direction:
   *
   * The pack direction of the menubar. It determines how
   * menuitems are arranged in the menubar.
   *
   * Since: 2.8
   */
  g_object_class_install_property (gobject_class,
				   PROP_PACK_DIRECTION,
				   g_param_spec_enum ("pack-direction",
 						      P_("Pack direction"),
 						      P_("The pack direction of the menubar"),
 						      GTK_TYPE_PACK_DIRECTION,
 						      GTK_PACK_DIRECTION_LTR,
 						      GTK_PARAM_READWRITE));
  
  /**
   * GtkMenuBar:child-pack-direction:
   *
   * The pack direction of the menubar. It determines how
   * the widgets contained in child menuitems are arranged.
   *
   * Since: 2.8
   */
  g_object_class_install_property (gobject_class,
				   PROP_CHILD_PACK_DIRECTION,
				   g_param_spec_enum ("child-pack-direction",
 						      P_("Child Pack direction"),
 						      P_("The child pack direction of the menubar"),
 						      GTK_TYPE_PACK_DIRECTION,
 						      GTK_PACK_DIRECTION_LTR,
 						      GTK_PARAM_READWRITE));
  

  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_enum ("shadow-type",
                                                              P_("Shadow type"),
                                                              P_("Style of bevel around the menubar"),
                                                              GTK_TYPE_SHADOW_TYPE,
                                                              GTK_SHADOW_OUT,
                                                              GTK_PARAM_READABLE));

  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_int ("internal-padding",
							     P_("Internal padding"),
							     P_("Amount of border space between the menubar shadow and the menu items"),
							     0,
							     G_MAXINT,
                                                             DEFAULT_IPADDING,
                                                             GTK_PARAM_READABLE));

  gtk_settings_install_property (g_param_spec_int ("gtk-menu-bar-popup-delay",
						   P_("Delay before drop down menus appear"),
						   P_("Delay before the submenus of a menu bar appear"),
						   0,
						   G_MAXINT,
						   0,
						   GTK_PARAM_READWRITE));

  g_type_class_add_private (gobject_class, sizeof (GtkMenuBarPrivate));  
}

static void
gtk_menu_bar_init (GtkMenuBar *object)
{
#ifdef GNOME_GLOBALMENU
	GtkMenuBarPrivate * priv;
	priv = GTK_MENU_BAR_GET_PRIVATE(object);

    priv->connected = FALSE;
	priv->socket = NULL;
    priv->globalized = FALSE;
	priv->container_window = NULL;
	priv->float_window = NULL;
	priv->master_window = NULL;
	priv->visible = FALSE;
	priv->detached = FALSE;
#endif
}

GtkWidget*
gtk_menu_bar_new (void)
{
  return g_object_new (GTK_TYPE_MENU_BAR, NULL);
}

static void
gtk_menu_bar_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  
  switch (prop_id)
    {
    case PROP_PACK_DIRECTION:
      gtk_menu_bar_set_pack_direction (menubar, g_value_get_enum (value));
      break;
    case PROP_CHILD_PACK_DIRECTION:
      gtk_menu_bar_set_child_pack_direction (menubar, g_value_get_enum (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_menu_bar_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  
  switch (prop_id)
    {
    case PROP_PACK_DIRECTION:
      g_value_set_enum (value, gtk_menu_bar_get_pack_direction (menubar));
      break;
    case PROP_CHILD_PACK_DIRECTION:
      g_value_set_enum (value, gtk_menu_bar_get_child_pack_direction (menubar));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
_gtk_menu_bar_size_request (GtkWidget      *widget,
			   GtkRequisition *requisition)
{
  GtkMenuBar *menu_bar;
  GtkMenuBarPrivate *priv;
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
  
  if (GTK_WIDGET_VISIBLE (widget))
    {
      menu_bar = GTK_MENU_BAR (widget);
      menu_shell = GTK_MENU_SHELL (widget);
      priv = GTK_MENU_BAR_GET_PRIVATE (menu_bar);

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

	      if (priv->child_pack_direction == GTK_PACK_DIRECTION_LTR ||
		  priv->child_pack_direction == GTK_PACK_DIRECTION_RTL)
		child_requisition.width += toggle_size;
	      else
		child_requisition.height += toggle_size;

              if (priv->pack_direction == GTK_PACK_DIRECTION_LTR ||
		  priv->pack_direction == GTK_PACK_DIRECTION_RTL)
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

#ifdef GNOME_GLOBAL_MENU
	if(!priv->detached) /*no matter realized or not*/
#endif
      if (get_shadow_type (menu_bar) != GTK_SHADOW_NONE)
	{
	  requisition->width += widget->style->xthickness * 2;
	  requisition->height += widget->style->ythickness * 2;
	}
    }
}
#ifdef GNOME_GLOBAL_MENU
static void
gtk_menu_bar_size_request (GtkWidget      *widget,
			   GtkRequisition *requisition)
{
#ifdef GNOME_GLOBAL_MENU
  GtkMenuBar *menu_bar;
  GtkMenuBarPrivate *priv;

LOG_FUNC_NAME;
  menu_bar = GTK_MENU_BAR (widget);
  priv = GTK_MENU_BAR_GET_PRIVATE (menu_bar);
/****store the requisition as container window's requistion****/
	if((!GTK_WIDGET_REALIZED(widget) && priv->globalized) ||
		(GTK_WIDGET_REALIZED(widget) &&  priv->detached)){
/******tell the parent my requisition is 0 **********/
		requisition->width = 0;
		requisition->height = 0;
	} else
#endif
	_gtk_menu_bar_size_request(widget, requisition);
}
#else
static void
gtk_menu_bar_size_request (GtkWidget      *widget,
			   GtkRequisition *requisition)
{
  _gtk_menu_bar_size_request(widget, requisition);
}
#endif

static void
_gtk_menu_bar_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation)
{
  GtkMenuBar *menu_bar;
  GtkMenuShell *menu_shell;
  GtkMenuBarPrivate *priv;
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
  priv = GTK_MENU_BAR_GET_PRIVATE (menu_bar);

  direction = gtk_widget_get_direction (widget);

#ifndef GNOME_GLOBAL_MENU /*we will deal with windows in the true size_allocate*/
  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window,
			    allocation->x, allocation->y,
			    allocation->width, allocation->height);
#endif
LOG_FUNC_NAME;
  gtk_widget_style_get (widget, "internal-padding", &ipadding, NULL);
  
  if (menu_shell->children)
    {
      child_allocation.x = (GTK_CONTAINER (menu_bar)->border_width +
			    ipadding + 
			    BORDER_SPACING);
      child_allocation.y = (GTK_CONTAINER (menu_bar)->border_width +
			    BORDER_SPACING);
      
#ifdef GNOME_GLOBAL_MENU
	g_message("detached = %d", priv->detached);
	g_message("thickness = %d", widget->style->ythickness);
	g_message("shadowtype<>shandow_now = %d", get_shadow_type(menu_bar)!=GTK_SHADOW_NONE);
	if(!priv->detached)
#endif
      if (get_shadow_type (menu_bar) != GTK_SHADOW_NONE)
	{
	  child_allocation.x += widget->style->xthickness;
	  child_allocation.y += widget->style->ythickness;
	}
      
      if (priv->pack_direction == GTK_PACK_DIRECTION_LTR ||
	  priv->pack_direction == GTK_PACK_DIRECTION_RTL)
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
#ifndef GNOME_GLOBAL_MENU
	      gtk_widget_get_child_requisition (child, &child_requisition);
#else
	      gtk_widget_size_request (child, &child_requisition);
#endif 
	      if (priv->child_pack_direction == GTK_PACK_DIRECTION_LTR ||
		  priv->child_pack_direction == GTK_PACK_DIRECTION_RTL)
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
		  if ((direction == GTK_TEXT_DIR_LTR) == (priv->pack_direction == GTK_PACK_DIRECTION_LTR))
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
	      
	      if (priv->child_pack_direction == GTK_PACK_DIRECTION_LTR ||
		  priv->child_pack_direction == GTK_PACK_DIRECTION_RTL)
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
		      (priv->pack_direction == GTK_PACK_DIRECTION_TTB))
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
/**either the parent widget or the menu server can invoke this,
 * Hope we can distinguish them*/
static void
gtk_menu_bar_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation){
#ifdef GNOME_GLOBAL_MENU
  GtkMenuBar *menu_bar;
  GtkMenuShell *menu_shell;
  GtkMenuBarPrivate *priv;

  g_return_if_fail (GTK_IS_MENU_BAR (widget));
  g_return_if_fail (allocation != NULL);
LOG_FUNC_NAME;
  menu_bar = GTK_MENU_BAR (widget);
  menu_shell = GTK_MENU_SHELL (widget);
  priv = GTK_MENU_BAR_GET_PRIVATE (menu_bar);

  if (GTK_WIDGET_REALIZED (widget)){
	  if (!priv->detached){
	    priv->container_allocation = *allocation;
		gdk_window_move_resize (priv->container_window,
					0, 0,
					allocation->width, allocation->height);
		gdk_window_move_resize (widget->window,
						allocation->x, allocation->y,
						allocation->width, allocation->height);
      }else{
		gdk_window_move_resize (widget->window,
						allocation->x, allocation->y - 2, 
				/*change to y-2 Dirty fix to make it invisible while keep the window mapped*/
						allocation->width, allocation->height);
	  }

  }
  widget->allocation = *allocation;
  if((GTK_WIDGET_REALIZED(widget) && !priv->detached) 
	|| !priv->globalized){ /*if globalized, allocate the childrens in _cb instead of parent's request*/
  	_gtk_menu_bar_size_allocate(widget, allocation);
  } else {
  	_gtk_menu_bar_size_allocate(widget, &priv->container_allocation);
  }
#else
	_gtk_menu_bar_size_allocate(widget, allocation);
#endif
}

static void
gtk_menu_bar_paint (GtkWidget    *widget,
                    GdkRectangle *area)
{
#ifdef GNOME_GLOBAL_MENU
  GtkMenuBar * menubar;
  GtkMenuBarPrivate * priv;

  menubar = GTK_MENU_BAR(widget);
  priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
LOG_FUNC_NAME;
#endif
  g_return_if_fail (GTK_IS_MENU_BAR (widget));

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      gint border;

      border = GTK_CONTAINER (widget)->border_width;
      
#ifdef GNOME_GLOBAL_MENU
	  if(!priv->detached) /*if we reach here, GTK_WIDGET_REALIZED==TRUE*/
      gtk_paint_box (widget->style,
		     priv->container_window,
                     GTK_WIDGET_STATE (widget),
                     get_shadow_type (GTK_MENU_BAR (widget)),
		     area, widget, "menubar",
		     border, border,
		     priv->container_allocation.width - border * 2,
                     priv->container_allocation.height - border * 2);
#else
      gtk_paint_box (widget->style,
		     widget->window,
                     GTK_WIDGET_STATE (widget),
                     get_shadow_type (GTK_MENU_BAR (widget)),
		     area, widget, "menubar",
		     border, border,
		     widget->allocation.width - border * 2,
                     widget->allocation.height - border * 2);
#endif
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
#ifdef GNOME_GLOBAL_MENU
	GtkMenuBarPrivate * priv = GTK_MENU_BAR_GET_PRIVATE(widget);
	if(event->window == priv->container_window ||
/*globalized and*/	event->window == priv->float_window ||
/*not globalized and*/	event->window == widget->window)
#endif
      gtk_menu_bar_paint (widget, &event->area);

      (* GTK_WIDGET_CLASS (gtk_menu_bar_parent_class)->expose_event) (widget, event);
    }

  return FALSE;
}

static GList *
get_menu_bars (GtkWindow *window)
{
  return g_object_get_data (G_OBJECT (window), "gtk-menu-bar-list");
}

static GList *
get_viewable_menu_bars (GtkWindow *window)
{
  GList *menu_bars;
  GList *viewable_menu_bars = NULL;

  for (menu_bars = get_menu_bars (window);
       menu_bars;
       menu_bars = menu_bars->next)
    {
      GtkWidget *widget = menu_bars->data;
      gboolean viewable = TRUE;
      
      while (widget)
	{
	  if (!GTK_WIDGET_MAPPED (widget))
	    viewable = FALSE;
	  
	  widget = widget->parent;
	}

      if (viewable)
	viewable_menu_bars = g_list_prepend (viewable_menu_bars, menu_bars->data);
    }

  return g_list_reverse (viewable_menu_bars);
}

static void
set_menu_bars (GtkWindow *window,
	       GList     *menubars)
{
  g_object_set_data (G_OBJECT (window), I_("gtk-menu-bar-list"), menubars);
}

/*since gtk don't export this function, I have to copy it form gtk source!*/
void
_gtk_menu_shell_activate (GtkMenuShell *menu_shell)
{
  if (!menu_shell->active)
    {
      gtk_grab_add (GTK_WIDGET (menu_shell));
      menu_shell->have_grab = TRUE;
      menu_shell->active = TRUE;
    }
}
/*workaround, gtk don't export this function!*/
GList * my_gtk_container_focus_sort (GtkContainer * container, GList * source, 
	GtkDirectionType dir, GtkWidget *old_focus){
	return g_list_copy(source);
}
#define _gtk_container_focus_sort my_gtk_container_focus_sort

static gboolean
window_key_press_handler (GtkWidget   *widget,
                          GdkEventKey *event,
                          gpointer     data)
{
  gchar *accel = NULL;
  gboolean retval = FALSE;
  
  g_object_get (gtk_widget_get_settings (widget),
                "gtk-menu-bar-accel", &accel,
                NULL);

  if (accel && *accel)
    {
      guint keyval = 0;
      GdkModifierType mods = 0;

      gtk_accelerator_parse (accel, &keyval, &mods);

      if (keyval == 0)
        g_warning ("Failed to parse menu bar accelerator '%s'\n", accel);

      /* FIXME this is wrong, needs to be in the global accel resolution
       * thing, to properly consider i18n etc., but that probably requires
       * AccelGroup changes etc.
       */
      if (event->keyval == keyval &&
          ((event->state & gtk_accelerator_get_default_mod_mask ()) ==
	   (mods & gtk_accelerator_get_default_mod_mask ())))
        {
	  GList *tmp_menubars = get_viewable_menu_bars (GTK_WINDOW (widget));
	  GList *menubars;

	  menubars = _gtk_container_focus_sort (GTK_CONTAINER (widget), tmp_menubars,
						GTK_DIR_TAB_FORWARD, NULL);
	  g_list_free (tmp_menubars);

	  if (menubars)
	    {
	      GtkMenuShell *menu_shell = GTK_MENU_SHELL (menubars->data);

	      _gtk_menu_shell_activate (menu_shell);
	      gtk_menu_shell_select_first (menu_shell, FALSE);
	      
	      g_list_free (menubars);
	      
	      retval = TRUE;	      
	    }
        }
    }

  g_free (accel);

  return retval;
}

static void
add_to_window (GtkWindow  *window,
               GtkMenuBar *menubar)
{
  GList *menubars = get_menu_bars (window);

  if (!menubars)
    {
#ifdef GNOME_GLOBAL_MENU
      GtkMenuBarPrivate * priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
	  gtk_menu_bar_connect_to_menu_server(menubar);
	  if(priv->connected){ /**/
			gtk_menu_bar_globalize(menubar);
	  }
#endif
      g_signal_connect (window,
			"key_press_event",
			G_CALLBACK (window_key_press_handler),
			NULL);
    }

  set_menu_bars (window, g_list_prepend (menubars, menubar));
}

static void
remove_from_window (GtkWindow  *window,
                    GtkMenuBar *menubar)
{
  GList *menubars = get_menu_bars (window);

  menubars = g_list_remove (menubars, menubar);

  if (!menubars)
    {
      g_signal_handlers_disconnect_by_func (window,
					    window_key_press_handler,
					    NULL);
    }

  set_menu_bars (window, menubars);
}

static void
gtk_menu_bar_hierarchy_changed (GtkWidget *widget,
				GtkWidget *old_toplevel)
{
  GtkWidget *toplevel;  
  GtkMenuBar *menubar;

  menubar = GTK_MENU_BAR (widget);

  toplevel = gtk_widget_get_toplevel (widget);

  if (old_toplevel)
    remove_from_window (GTK_WINDOW (old_toplevel), menubar);
  
  if (GTK_WIDGET_TOPLEVEL (toplevel))
    add_to_window (GTK_WINDOW (toplevel), menubar);
}

/**
 * _gtk_menu_bar_cycle_focus:
 * @menubar: a #GtkMenuBar
 * @dir: direction in which to cycle the focus
 * 
 * Move the focus between menubars in the toplevel.
 **/
void
_gtk_menu_bar_cycle_focus (GtkMenuBar       *menubar,
			   GtkDirectionType  dir)
{
  GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (menubar));
  GtkMenuItem *to_activate = NULL;

  if (GTK_WIDGET_TOPLEVEL (toplevel))
    {
      GList *tmp_menubars = get_viewable_menu_bars (GTK_WINDOW (toplevel));
      GList *menubars;
      GList *current;

      menubars = _gtk_container_focus_sort (GTK_CONTAINER (toplevel), tmp_menubars,
					    dir, GTK_WIDGET (menubar));
      g_list_free (tmp_menubars);

      if (menubars)
	{
	  current = g_list_find (menubars, menubar);

	  if (current && current->next)
	    {
	      GtkMenuShell *new_menushell = GTK_MENU_SHELL (current->next->data);
	      if (new_menushell->children)
		to_activate = new_menushell->children->data;
	    }
	}
	  
      g_list_free (menubars);
    }

  gtk_menu_shell_cancel (GTK_MENU_SHELL (menubar));

  if (to_activate)
    g_signal_emit_by_name (to_activate, "activate_item");
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
gtk_menu_bar_get_popup_delay (GtkMenuShell *menu_shell)
{
  gint popup_delay;
  
  g_object_get (gtk_widget_get_settings (GTK_WIDGET (menu_shell)),
		"gtk-menu-bar-popup-delay", &popup_delay,
		NULL);

  return popup_delay;
}

static void
gtk_menu_bar_move_current (GtkMenuShell         *menu_shell,
			   GtkMenuDirectionType  direction)
{
  GtkMenuBar *menubar = GTK_MENU_BAR (menu_shell);
  GtkTextDirection text_dir;
  GtkPackDirection pack_dir;

  text_dir = gtk_widget_get_direction (GTK_WIDGET (menubar));
  pack_dir = gtk_menu_bar_get_pack_direction (menubar);
  
  if (pack_dir == GTK_PACK_DIRECTION_LTR || pack_dir == GTK_PACK_DIRECTION_RTL)
     {
      if ((text_dir == GTK_TEXT_DIR_RTL) == (pack_dir == GTK_PACK_DIRECTION_LTR))
	{
	  switch (direction) 
	    {      
	    case GTK_MENU_DIR_PREV:
	      direction = GTK_MENU_DIR_NEXT;
	      break;
	    case GTK_MENU_DIR_NEXT:
	      direction = GTK_MENU_DIR_PREV;
	      break;
	    default: ;
	    }
	}
    }
  else
    {
      switch (direction) 
	{
	case GTK_MENU_DIR_PARENT:
	  if ((text_dir == GTK_TEXT_DIR_LTR) == (pack_dir == GTK_PACK_DIRECTION_TTB))
	    direction = GTK_MENU_DIR_PREV;
	  else
	    direction = GTK_MENU_DIR_NEXT;
	  break;
	case GTK_MENU_DIR_CHILD:
	  if ((text_dir == GTK_TEXT_DIR_LTR) == (pack_dir == GTK_PACK_DIRECTION_TTB))
	    direction = GTK_MENU_DIR_NEXT;
	  else
	    direction = GTK_MENU_DIR_PREV;
	  break;
	case GTK_MENU_DIR_PREV:
	  if (text_dir == GTK_TEXT_DIR_RTL)	  
	    direction = GTK_MENU_DIR_CHILD;
	  else
	    direction = GTK_MENU_DIR_PARENT;
	  break;
	case GTK_MENU_DIR_NEXT:
	  if (text_dir == GTK_TEXT_DIR_RTL)	  
	    direction = GTK_MENU_DIR_PARENT;
	  else
	    direction = GTK_MENU_DIR_CHILD;
	  break;
	default: ;
	}
    }
  
  GTK_MENU_SHELL_CLASS (gtk_menu_bar_parent_class)->move_current (menu_shell, direction);
}

/**
 * gtk_menu_bar_get_pack_direction:
 * @menubar: a #GtkMenuBar
 * 
 * Retrieves the current pack direction of the menubar. See
 * gtk_menu_bar_set_pack_direction().
 *
 * Return value: the pack direction
 *
 * Since: 2.8
 **/
GtkPackDirection
gtk_menu_bar_get_pack_direction (GtkMenuBar *menubar)
{
  GtkMenuBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_MENU_BAR (menubar), 
			GTK_PACK_DIRECTION_LTR);
  
  priv = GTK_MENU_BAR_GET_PRIVATE (menubar);

  return priv->pack_direction;
}

/**
 * gtk_menu_bar_set_pack_direction:
 * @menubar: a #GtkMenuBar.
 * @pack_dir: a new #GtkPackDirection.
 * 
 * Sets how items should be packed inside a menubar.
 * 
 * Since: 2.8
 **/
void
gtk_menu_bar_set_pack_direction (GtkMenuBar       *menubar,
                                 GtkPackDirection  pack_dir)
{
  GtkMenuBarPrivate *priv;
  GList *l;

  g_return_if_fail (GTK_IS_MENU_BAR (menubar));

  priv = GTK_MENU_BAR_GET_PRIVATE (menubar);

  if (priv->pack_direction != pack_dir)
    {
      priv->pack_direction = pack_dir;

      gtk_widget_queue_resize (GTK_WIDGET (menubar));

      for (l = GTK_MENU_SHELL (menubar)->children; l; l = l->next)
	gtk_widget_queue_resize (GTK_WIDGET (l->data));

      g_object_notify (G_OBJECT (menubar), "pack-direction");
    }
}

/**
 * gtk_menu_bar_get_child_pack_direction:
 * @menubar: a #GtkMenuBar
 * 
 * Retrieves the current child pack direction of the menubar. See
 * gtk_menu_bar_set_child_pack_direction().
 *
 * Return value: the child pack direction
 *
 * Since: 2.8
 **/
GtkPackDirection
gtk_menu_bar_get_child_pack_direction (GtkMenuBar *menubar)
{
  GtkMenuBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_MENU_BAR (menubar), 
			GTK_PACK_DIRECTION_LTR);
  
  priv = GTK_MENU_BAR_GET_PRIVATE (menubar);

  return priv->child_pack_direction;
}

/**
 * gtk_menu_bar_set_child_pack_direction:
 * @menubar: a #GtkMenuBar.
 * @child_pack_dir: a new #GtkPackDirection.
 * 
 * Sets how widgets should be packed inside the children of a menubar.
 * 
 * Since: 2.8
 **/
void
gtk_menu_bar_set_child_pack_direction (GtkMenuBar       *menubar,
                                       GtkPackDirection  child_pack_dir)
{
  GtkMenuBarPrivate *priv;
  GList *l;

  g_return_if_fail (GTK_IS_MENU_BAR (menubar));

  priv = GTK_MENU_BAR_GET_PRIVATE (menubar);

  if (priv->child_pack_direction != child_pack_dir)
    {
      priv->child_pack_direction = child_pack_dir;

      gtk_widget_queue_resize (GTK_WIDGET (menubar));

      for (l = GTK_MENU_SHELL (menubar)->children; l; l = l->next)
	gtk_widget_queue_resize (GTK_WIDGET (l->data));

      g_object_notify (G_OBJECT (menubar), "child-pack-direction");
    }
}

#ifdef GNOME_GLOBAL_MENU
/************* Global menu patched methods ***************/
static void gtk_menu_bar_finalize            (GObject         *object){

	GtkMenuBar * menubar;
	GtkMenuBarPrivate * priv;
LOG_FUNC_NAME;
	
	menubar = GTK_MENU_BAR(object);
	priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
    if(priv->globalized){
		gtk_menu_bar_unglobalize(menubar);
	}
	if(priv->connected){
		global_menu_socket_free(priv->socket);
	}
	G_OBJECT_CLASS(gtk_menu_bar_parent_class)->finalize(object);
}

static void
gtk_menu_bar_detach(GtkMenuBar * menubar){
  GlobalMenuNotify notify;
  GtkMenuBarPrivate * priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
  GtkWidget * toplevel = NULL;

  toplevel = gtk_widget_get_toplevel(GTK_WIDGET(menubar));
  if(GTK_WIDGET_TOPLEVEL(toplevel) 
	&& !GTK_WIDGET_NO_WINDOW(toplevel)
	&& gdk_window_get_type_hint(toplevel->window) 
	== GDK_WINDOW_TYPE_HINT_NORMAL){

	  priv->master_window = toplevel->window;
	  g_message("Master window found:%x", priv->master_window);

	  gdk_window_hide(priv->container_window);
	  gdk_window_hide(priv->float_window); /*ensure it is hidden*/
	  gdk_window_reparent(priv->container_window, priv->float_window, 0, 0); 
	  notify.type = GM_NOTIFY_NEW;
	  notify.ClientNew.client_xid = GDK_WINDOW_XWINDOW(priv->socket->window);
	  notify.ClientNew.float_xid = GDK_WINDOW_XWINDOW(priv->float_window);
	  notify.ClientNew.master_xid = GDK_WINDOW_XWINDOW(priv->master_window);
	  global_menu_socket_send(priv->socket, &notify);
			/*  Then we shall wait the server's ack by a SetVisibile Notify*/
	  priv->detached = TRUE;
	}
}
static void 
gtk_menu_bar_globalize(GtkMenuBar * menubar){
  GtkMenuBarPrivate * priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
  GtkWidget * toplevel = NULL;
LOG_FUNC_NAME;
  if(priv->globalized == FALSE){
	  priv->globalized = TRUE;
	  gtk_widget_queue_resize(menubar);
	  toplevel = gtk_widget_get_toplevel(GTK_WIDGET(menubar));
	  if(GTK_WIDGET_REALIZED(menubar)){
		      gtk_menu_bar_detach( menubar);
	  }
  }
}

static void 
gtk_menu_bar_unglobalize(GtkMenuBar * menubar){
  GtkMenuBarPrivate * priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
LOG_FUNC_NAME;
  if(priv->globalized){
	  GlobalMenuNotify notify;
	  priv->globalized = FALSE;
	  priv->pack_direction = GTK_PACK_DIRECTION_LTR; /*perhaps need to restore users pref here(for RTL languages)*/
	  notify.type = GM_NOTIFY_DESTROY;
      notify.ClientDestroy.client_xid = GDK_WINDOW_XWINDOW(priv->socket->window);
	  notify.ClientDestroy.float_xid = GDK_WINDOW_XWINDOW(priv->float_window);
	  notify.ClientDestroy.master_xid = GDK_WINDOW_XWINDOW(priv->master_window);
	  global_menu_socket_send(priv->socket, &notify);
	  priv->detached = FALSE;
	  if(GTK_WIDGET_REALIZED(menubar)){
		  gdk_window_reparent(priv->container_window, 
				GTK_WIDGET(menubar)->window, 0, 0); 
		  gtk_widget_queue_resize_no_redraw(GTK_WIDGET(menubar));
		  if(GTK_WIDGET_MAPPED(menubar)){
			  gdk_window_hide(priv->float_window);
			  gdk_window_show(GTK_WIDGET(menubar)->window);
			  gdk_window_show(priv->container_window);
			  gdk_window_invalidate_rect(priv->container_window, NULL, TRUE);
		  }
	  }
  }
}
static void
gtk_menu_bar_realize(GtkWidget * widget)
{
  GtkMenuBar * menubar = NULL;
  GtkMenuBarPrivate * priv = NULL;
  GdkWindowAttr attributes;
  gint attributes_mask = 0;

  menubar = GTK_MENU_BAR(widget);
  priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
LOG_FUNC_NAME;

  g_return_if_fail (GTK_IS_MENU_SHELL (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
/*********the widget window **********/
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
                GDK_BUTTON_PRESS_MASK |
                GDK_BUTTON_RELEASE_MASK |
                GDK_KEY_PRESS_MASK |
                GDK_ENTER_NOTIFY_MASK |
                GDK_LEAVE_NOTIFY_MASK);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, widget);


/*Then we create the container window*/

/* First allocate the size according to the state*/
  priv->container_allocation.x = 0;
  priv->container_allocation.y = 0;
  if(!priv->globalized){
	  priv->container_allocation.width = widget->allocation.width;
	  priv->container_allocation.height = widget->allocation.height;
  } else {
	  priv->container_allocation.width = 0; //priv->container_requisition.width;
	  priv->container_allocation.height = 0; //priv->container_requisition.height;
  }

/*********Then the container window itself*********/
  attributes.x = 0;
  attributes.y = 0;
  attributes.width = priv->container_allocation.width;
  attributes.height = priv->container_allocation.height;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = (gtk_widget_get_events (widget) |
               GDK_EXPOSURE_MASK |
                GDK_BUTTON_PRESS_MASK |
                GDK_BUTTON_RELEASE_MASK |
                GDK_KEY_PRESS_MASK |
                GDK_ENTER_NOTIFY_MASK |
                GDK_LEAVE_NOTIFY_MASK |
                GDK_BUTTON1_MOTION_MASK |
                GDK_POINTER_MOTION_HINT_MASK);
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
/*I don't think they it need visual and colormap, let me try to remove these later*/
  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  priv->container_window = gdk_window_new (widget->window, &attributes, attributes_mask);
  gdk_window_set_user_data (priv->container_window, widget);

/********next, the float window ************/
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = priv->container_allocation.width;
  attributes.height = priv->container_allocation.height;
  attributes.window_type = GDK_WINDOW_TEMP;
  attributes.wclass = GDK_INPUT_OUTPUT;

  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
				GDK_STRUCTURE_MASK |
			    GDK_KEY_PRESS_MASK |
			    GDK_ENTER_NOTIFY_MASK |
			    GDK_LEAVE_NOTIFY_MASK);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  priv->float_window = gdk_window_new (gtk_widget_get_root_window (widget), &attributes, attributes_mask);

  gdk_window_stick(priv->float_window); 
  /*Sticky so it shows on every desktop, perhaps not useful*/
  gdk_window_set_user_data (priv->float_window, widget);

/*Set some painting hints*/
  /*attach the style, so widget will get painting visual and colormap from container_window*/
  widget->style = gtk_style_attach(widget->style, priv->container_window); 
  gtk_style_set_background (widget->style, priv->container_window, GTK_WIDGET_STATE (menubar));
/*Then set all the exsited menuitems's gdk parent window to the container, also not useful, remove it */
  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);
  gtk_container_forall(GTK_CONTAINER(widget), 
			(GtkCallback)(gtk_widget_set_parent_window), 
			(gpointer)(priv->container_window));

  if(priv->globalized) gtk_menu_bar_detach(menubar);
}

static void
gtk_menu_bar_unrealize(GtkWidget * widget)
{
  GtkMenuBar * menubar = NULL;
  GtkMenuBarPrivate * priv = NULL;

LOG_FUNC_NAME;

  menubar = GTK_MENU_BAR(widget);
  priv = GTK_MENU_BAR_GET_PRIVATE(menubar);


  if(GTK_WIDGET_MAPPED(widget))
	  gtk_menu_bar_unmap(widget);
  gdk_window_set_user_data (priv->container_window, NULL);
  gdk_window_destroy (priv->container_window);
  priv->container_window = NULL;

  gdk_window_set_user_data (priv->float_window, NULL);
  gdk_window_destroy (priv->float_window);
  priv->float_window = NULL;
  GTK_WIDGET_CLASS(gtk_menu_bar_parent_class)->unrealize(widget); 
}

static void
gtk_menu_bar_map_child (GtkWidget *child,
             gpointer   client_data)
{
  if (GTK_WIDGET_VISIBLE (child) &&
      GTK_WIDGET_CHILD_VISIBLE (child) &&
      !GTK_WIDGET_MAPPED (child))
    gtk_widget_map (child);
}

static void
gtk_menu_bar_map(GtkWidget * widget){
  GtkMenuBar * menubar = NULL;
  GtkMenuBarPrivate * priv = NULL;
LOG_FUNC_NAME;

  menubar = GTK_MENU_BAR(widget);
  priv = GTK_MENU_BAR_GET_PRIVATE(menubar);

  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);

  gdk_window_show(widget->window);
  if(!priv->detached){ /*If we reach here, GTK_WIDGET_REALIZE == TRUE*/
  gdk_window_show(priv->container_window);
  }

  gtk_container_forall (GTK_CONTAINER (widget),
            gtk_menu_bar_map_child,
            NULL);

}

static void
gtk_menu_bar_unmap(GtkWidget * widget){
  GtkMenuBar * menubar = NULL;
  GtkMenuBarPrivate * priv = NULL;
LOG_FUNC_NAME;

  menubar = GTK_MENU_BAR(widget);
  priv = GTK_MENU_BAR_GET_PRIVATE(menubar);

  GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED); 

  if(priv->globalized) gtk_menu_bar_unglobalize(menubar);

  gdk_window_hide(priv->float_window);
  gdk_window_hide(widget->window);
}

/*****************************
 * overide gtk_menu_shell_insert, because we want to set proper parent gdkwindow
 * *****************/
static void
gtk_menu_bar_real_insert(GtkMenuShell * menu_shell, GtkWidget * child, gint position){
  GtkMenuBar * menubar = NULL;
  GtkMenuBarPrivate * priv = NULL;
LOG_FUNC_NAME;

  menubar = GTK_MENU_BAR(menu_shell);
  priv = GTK_MENU_BAR_GET_PRIVATE(menubar);

  if(GTK_WIDGET_REALIZED(GTK_WIDGET(menubar))){ /*if we have a container_window, move the child to container_window*/
	g_message("set parent window to container_window");
  gtk_widget_set_parent_window(child, priv->container_window);
	}

  GTK_MENU_SHELL_CLASS(gtk_menu_bar_parent_class)->insert(menu_shell, child, position);
}
static gboolean 
gtk_menu_bar_motion (GtkWidget * widget, 
		GdkEventMotion * event){
  GtkMenuBar * menubar = NULL;
  GtkMenuBarPrivate * priv = NULL;

  menubar = GTK_MENU_BAR(widget);
  priv = GTK_MENU_BAR_GET_PRIVATE(menubar);

	if(GTK_WIDGET_CLASS(gtk_menu_bar_parent_class)->motion_notify_event)
		return GTK_WIDGET_CLASS(gtk_menu_bar_parent_class)->motion_notify_event(widget, event);
	if(event->is_hint){
		LOG_FUNC_NAME;
	}
	return TRUE;
}
static gboolean gtk_menu_bar_delete_event(GtkWidget * widget, GdkEventAny * event){
	GtkMenuBar * menubar = GTK_MENU_BAR(widget);
	GtkMenuBarPrivate * priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
LOG_FUNC_NAME;
	if(priv->globalized && event->window == priv->float_window){
		/*If we receive a delete_event from float_window, and we are globalized,
          it must be because of the server is dying. a signal to unglobalize. */
		gtk_menu_bar_unglobalize(menubar);
		priv->connected = FALSE;
		return TRUE;
	}
	return FALSE;		
}
static void
gtk_menu_bar_notify_size_allocate_cb(GlobalMenuSocket * socket, 
		GlobalMenuNotify * notify,
		GtkMenuBar * menubar){
	GtkMenuBarPrivate * priv;
	GtkAllocation allocation;
	priv = GTK_MENU_BAR_GET_PRIVATE(menubar);

    g_return_if_fail( priv->connected );
	g_return_if_fail( notify->SizeAllocate.server_xid == socket->dest_xid); /*if not from out server*/

	allocation.width = notify->SizeAllocate.width;
	allocation.height = notify->SizeAllocate.height;
	g_message("Size Allocate Notify Received: %d, %d",allocation.width, allocation.height);

	priv->container_allocation.width = allocation.width;
	priv->container_allocation.height = allocation.height;

	gdk_window_resize(priv->float_window, 
		allocation.width, allocation.height);
	gdk_window_resize(priv->container_window, 
		allocation.width, allocation.height);

	//gtk_menu_bar_ensure_style(GTK_WIDGET(menubar));

/****auto change direction if size is not enough******/
    if(allocation.width > allocation.height){
/****Perhaps we need new variable to store the user's prefs of direction****/
		priv->pack_direction = GTK_PACK_DIRECTION_LTR;
		priv->child_pack_direction = GTK_PACK_DIRECTION_LTR;
	} else {
		priv->pack_direction = GTK_PACK_DIRECTION_TTB;
		priv->child_pack_direction = GTK_PACK_DIRECTION_LTR;
	}

//	_gtk_menu_bar_size_allocate(GTK_WIDGET(menubar), &priv->container_allocation);
	gtk_widget_queue_resize(GTK_WIDGET(menubar));
//	gdk_window_invalidate_rect(priv->container_window, NULL, TRUE);

}
static void
gtk_menu_bar_notify_set_visible_cb(GlobalMenuSocket * socket,
		GlobalMenuNotify * notify,
		GtkMenuBar * menubar){
	GtkMenuBarPrivate * priv;
LOG_FUNC_NAME;
	priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
	priv->visible = TRUE;
    g_return_if_fail( priv->connected );
	g_return_if_fail( notify->SetVisible.server_xid == socket->dest_xid); /*if not from our server*/
	if(notify->SetVisible.visible == TRUE) {
	  gdk_window_show(priv->container_window);
	  gdk_window_show(priv->float_window);
	  gtk_widget_queue_draw(menubar);
	} else{
	  gdk_window_hide(priv->container_window);
	  gdk_window_hide(priv->float_window);
	}

}
static void
gtk_menu_bar_notify_server_destroy_cb(GlobalMenuSocket * socket, 
		GlobalMenuNotify * notify,
		GtkMenuBar * menubar){
	GtkMenuBarPrivate * priv;

	priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
    g_return_if_fail( priv->connected );
	g_return_if_fail( notify->ServerDestroy.server_xid == socket->dest_xid); /*if not from our server*/
    g_message("Menu Server dies, move the menubar back!!");
		priv->connected = FALSE;
		gtk_menu_bar_unglobalize(menubar);
}
static void
gtk_menu_bar_notify_server_new_cb(GlobalMenuSocket * socket, 
		GlobalMenuNotify * notify,
		GtkMenuBar * menubar){
	GtkMenuBarPrivate * priv;
	priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
    g_return_if_fail( !priv->connected ); 
	/*if alread connected to another server, do nothing.*/
	if(priv->connected == FALSE){
	  priv->connected = global_menu_socket_connect_by_xid(priv->socket, 
									notify->ServerNew.server_xid);
		if(priv->connected){
		  g_message("Menu Server Launched, float the menubar !!");
		  gtk_menu_bar_globalize(menubar);
		}
	}
}
static void
gtk_menu_bar_notify_set_background_cb(GlobalMenuSocket * socket,
		GlobalMenuNotify * notify,
		GtkMenuBar * menubar){
	GtkMenuBarPrivate * priv;
	GdkColor color;
	GdkPixmap * pixmap;
LOG_FUNC_NAME;
	priv = GTK_MENU_BAR_GET_PRIVATE(menubar);
    g_return_if_fail( priv->connected );
	g_return_if_fail( notify->SetBackground.server_xid == socket->dest_xid); /*if not from our server*/
	if(notify->SetBackground.color_atom){
		if(gdk_color_parse(
			gdk_x11_get_xatom_name(notify->SetBackground.color_atom),
			&color)){
		g_message("Set bg color to %s\n", gdk_color_to_string(&color));
		gtk_widget_modify_bg(menubar, GTK_STATE_NORMAL, &color);
		}
	}else{
		pixmap = gdk_pixmap_foreign_new(notify->SetBackground.pixmap_xid);
		gdk_window_set_back_pixmap(priv->container_window, pixmap, FALSE);
		g_object_unref(pixmap);
	}
	gtk_widget_queue_draw(menubar);
}
static void gtk_menu_bar_connect_to_menu_server(GtkMenuBar * menubar){
  GtkMenuBarPrivate * priv = GTK_MENU_BAR_GET_PRIVATE(menubar);

  priv->socket = global_menu_socket_new(MENU_CLIENT_NAME, menubar);
  global_menu_socket_set_callback(priv->socket, 
	GM_NOTIFY_SIZE_ALLOCATE, 
	(GlobalMenuCallback) gtk_menu_bar_notify_size_allocate_cb);
  global_menu_socket_set_callback(priv->socket,
	GM_NOTIFY_SERVER_NEW,
	(GlobalMenuCallback) gtk_menu_bar_notify_server_new_cb);
  global_menu_socket_set_callback(priv->socket,
	GM_NOTIFY_SERVER_DESTROY,
	(GlobalMenuCallback) gtk_menu_bar_notify_server_destroy_cb);
  global_menu_socket_set_callback(priv->socket,
	GM_NOTIFY_SET_VISIBLE,
	(GlobalMenuCallback) gtk_menu_bar_notify_set_visible_cb);
  global_menu_socket_set_callback(priv->socket, 
	GM_NOTIFY_SET_BACKGROUND,
	(GlobalMenuCallback) gtk_menu_bar_notify_set_background_cb);
/**we will receive notifications even if don't connect, so always check priv->connect (or priv->globalized)
 * if we want to make sure we are connected!***/
  priv->connected = global_menu_socket_connect_by_name(priv->socket, 
									MENU_SERVER_NAME);
}
#endif
#define __GTK_MENU_BAR_C__
