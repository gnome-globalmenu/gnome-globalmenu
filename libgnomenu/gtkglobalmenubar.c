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
#include <gtk/gtk.h>
#include "gtkglobalmenubar.h"

#define BORDER_SPACING  0
#define DEFAULT_IPADDING 1

#define GTK_GLOBAL_MENU_BAR_GET_PRIVATE(o)  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_GLOBAL_MENU_BAR, GtkGlobalMenuBarPrivate))

#define GET_OBJECT(_s, sgmb, p) \
	GtkGlobalMenuBar * sgmb = GTK_GLOBAL_MENU_BAR(_s); \
	GtkGlobalMenuBarPrivate * p = GTK_GLOBAL_MENU_BAR_GET_PRIVATE(_s);

#define LOG(fmt, args...) g_message("<GtkGlobalMenuBar>::" fmt,  ## args)
#define LOG_FUNC_NAME LOG("%s", __func__)

/* Properties */
enum {
  PROP_0,
};

typedef struct 
{
	gboolean disposed;
	gboolean detached;
	GdkColor bgcolor;
	GList * popup_items;
} GtkGlobalMenuBarPrivate;

/* GObject interface */
static GObject * _constructor 		( GType type, guint n_construct_properties, 
									  GObjectConstructParam *construct_params );
static void _dispose 				( GObject * object );
static void _finalize 				( GObject * object );
static void _set_property      		( GObject *object, guint prop_id, 
									  const GValue *value, GParamSpec * pspec );
static void _get_property			( GObject *object, guint prop_id, 
									  GValue *value, GParamSpec * pspec );

/* GtkWidget interface */
static void _size_request			( GtkWidget			* widget,
									  GtkRequisition	* requisition);
static void _size_allocate			( GtkWidget			* widget,
									  GtkAllocation		* allocation);
static void _hierarchy_changed		( GtkWidget 		* widget,
									  GtkWidget 		* old_toplevel);
static void _realize 				( GtkWidget * widget);
static void _unrealize 				( GtkWidget * widget);
static void _map 					( GtkWidget * widget);
static gint _expose 				( GtkWidget       *widget,
									  GdkEventExpose  *event);

/* GtkMenuShell Interface */
static void _insert 				( GtkMenuShell * menu_shell, 
									  GtkWidget * widget, gint pos);
/* GtkContainer Inteface */
static void _remove 				( GtkContainer * container, GtkWidget * widget);
/* Signal handler for the helper */
static void _s_size_request			( GtkWidget * widget,
									  GtkRequisition * requisition, 
									  GnomenuClientHelper * helper);

static void _s_size_allocate 		( GtkWidget       *widget,
								      GtkAllocation   *allocation, 
									  GnomenuClientHelper * helper);
static void _s_connected 			( GtkWidget  * menubar, 
									  GdkSocketNativeID target, 
									  GnomenuClientHelper * helper); 
static void _s_shutdown				( GtkWidget * menubar,
									  GnomenuClientHelper * helper);
static void _s_position_set 		( GtkWidget  * menubar, 
									  GdkPoint * pt,
									  GnomenuClientHelper * helper); 
static void _s_visibility_set 		( GtkWidget  * menubar, 
									  gboolean vis,
									  GnomenuClientHelper * helper); 
static void _s_bgcolor_set	 		( GtkWidget  * menubar, 
									  GdkColor * bgcolor,
									  GnomenuClientHelper * helper); 
/* workaround the delete event*/
static gboolean _s_delete_event			( GtkWidget * widget,
									  GdkEvent * event,
									  GnomenuClientHelper * helper);

/* utility functions*/
static void _calc_size_request		( GtkWidget * widget, GtkRequisition * requisition);
static void _do_size_allocate		( GtkWidget * widget, GtkAllocation * allocation);
static void 
	_set_child_parent_window		( GtkWidget * widget, GdkWindow * window);
static void _sync_remote_state		( GtkGlobalMenuBar * menubar);
static void _sync_local_state		( GtkGlobalMenuBar * menubar);

G_DEFINE_TYPE (GtkGlobalMenuBar, gtk_global_menu_bar, GTK_TYPE_MENU_BAR)

static void
gtk_global_menu_bar_class_init (GtkGlobalMenuBarClass *class)
{
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;
	GtkContainerClass *container_class;
	GtkMenuShellClass *menu_shell_class;

	gobject_class = (GObjectClass*) class;
	widget_class = (GtkWidgetClass*) class;
	container_class = (GtkContainerClass*) class;
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

	container_class->remove = _remove;

	g_type_class_add_private (gobject_class, sizeof (GtkGlobalMenuBarPrivate));  
}

static void
gtk_global_menu_bar_init (GtkGlobalMenuBar *object)
{
}

/**
 * gtk_global_menu_bar_new:
 *
 * Oops, this is the only function I exposed to you! Don't be mad,
 * because #GtkGlobalMenuBar is a subclass of #GtkMenuBar,
 * and you can (by definition you always can) use any function who
 * work for a #GtkMenuBar on a #GtkGlobalMenuBar.
 *
 * If you are patching GTK to replace #GtkMenuBar with 
 * #GtkGlobalMenuBar completely via #gtk_menu_bar_new, 
 * don't use this function. The interface compatibility
 * doesn't means everything. At a first glance the potentially vulnerabilities
 * are:
 *
 * 1 if some GTK client subclassed GtkMenuBar, and you rudely replace
 * #GtkMenuBar with #GtkGlobalMenuBar. The entire #GType inheriting
 * tree will be screwed. Everything will become weird. Your applications
 * might still be able to work, in a very undefined way.
 * 
 * 2 if some GTK client are just nerdy. Usually those application are written
 * by very talent hackers who understand what they are doing very well. 
 * Eclipse might be one of those examples.
 *
 * Under either cirumstances, what you'll want is Quirks. You call
 * #gtk_legacy_menu_bar_new instead.
 *
 * Returns: the created global menu bar.
 */
GtkWidget *
gtk_global_menu_bar_new (void)
{
  return g_object_new (GTK_TYPE_GLOBAL_MENU_BAR, NULL);
}


static GObject* _constructor(GType type, 
		guint n_construct_properties,
		GObjectConstructParam *construct_params){

	GObject * object;
	GtkGlobalMenuBar * menu_bar;
	GtkGlobalMenuBarPrivate * priv;
	object = (G_OBJECT_CLASS(gtk_global_menu_bar_parent_class)->constructor)(
		type, n_construct_properties, construct_params);

	menu_bar = GTK_GLOBAL_MENU_BAR(object);
	priv = GTK_GLOBAL_MENU_BAR_GET_PRIVATE(menu_bar);
	priv->disposed = FALSE;
	priv->detached = FALSE;
	priv->bgcolor = gtk_widget_get_style(menu_bar)->bg[GTK_STATE_NORMAL];
	priv->popup_items = NULL;

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

	g_signal_connect_swapped(G_OBJECT(menu_bar->helper), "position-set",
				G_CALLBACK(_s_position_set), menu_bar);
	g_signal_connect_swapped(G_OBJECT(menu_bar->helper), "bgcolor-set",
				G_CALLBACK(_s_bgcolor_set), menu_bar);
	g_signal_connect_swapped(G_OBJECT(menu_bar->helper), "visibility-set",
				G_CALLBACK(_s_visibility_set), menu_bar);

	g_signal_connect_swapped(G_OBJECT(menu_bar->helper), "connected",
				G_CALLBACK(_s_connected), menu_bar);
	g_signal_connect_swapped(G_OBJECT(menu_bar->helper), "shutdown",
				G_CALLBACK(_s_shutdown), menu_bar);
	
	g_signal_connect(G_OBJECT(menu_bar), "delete-event",
				G_CALLBACK(_s_delete_event), menu_bar->helper);
	return object;
}

static void
_dispose (GObject * _object){
	LOG_FUNC_NAME;
	GET_OBJECT(_object, menu_bar, priv);
	if(!priv->disposed){
		priv->disposed = TRUE;	
		g_object_unref(menu_bar->helper);
	}
	G_OBJECT_CLASS(gtk_global_menu_bar_parent_class)->dispose(_object);
}
static void
_finalize(GObject * _object){
	LOG_FUNC_NAME;
	GET_OBJECT(_object, menu_bar, priv);	
	g_list_free(priv->popup_items);
	G_OBJECT_CLASS(gtk_global_menu_bar_parent_class)->finalize(_object);
}
static void
_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
	GET_OBJECT(object, self, priv); 
	switch (prop_id) {
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
	GET_OBJECT(object, self, priv); 
  
	switch (prop_id) {
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
	LOG_FUNC_NAME;

	GET_OBJECT(widget, menu_bar, priv);
	if (priv->detached) {
		/* This is a quirk. Workaround for evolution and other
 		   Applications that changes menu item size and use
		   'gtk_widget_queue_resize' or whatever on the menu bar
		*/
		GtkRequisition useless;
		_calc_size_request(widget, &useless);
		requisition->width = 0;
		requisition->height = 0;
 	} else {
		_calc_size_request(widget, requisition);
	}
	widget->requisition = *requisition;
}
static void
_s_size_request (
	GtkWidget * widget,
	GtkRequisition *  requisition,
	GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	_calc_size_request (widget, requisition);
}
static void
_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation)
{
	g_return_if_fail (GTK_IS_MENU_BAR (widget));
	g_return_if_fail (allocation != NULL);
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	widget->allocation = *allocation;
	if (priv->detached) {
		if(GTK_WIDGET_REALIZED(widget)){
			gdk_window_move_resize(widget->window, 
				allocation->x,
				allocation->y,
				allocation->width,
				allocation->height);
		}
		/* This is a quirk. Workaround for evolution and other
 		   Applications that changes menu item size and use
		   'gtk_widget_queue_resize' or whatever on the menu bar
		*/
		_do_size_allocate(widget, &menu_bar->allocation);
		
	} else {
		menu_bar->allocation = *allocation;	
		if(GTK_WIDGET_REALIZED(widget)){
			gdk_window_move_resize(widget->window, 
				allocation->x,
				allocation->y,
				allocation->width,
				allocation->height);
		}
		_do_size_allocate(widget, allocation);
	}
}
static void
_s_size_allocate (GtkWidget * widget, 
	GtkAllocation * allocation,
	GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	menu_bar->allocation = *allocation;	
	if(GTK_WIDGET_REALIZED(widget)){
		gdk_window_move_resize(menu_bar->floater,
			allocation->x,
			allocation->y,
			allocation->width,
			allocation->height);
	}
	_do_size_allocate(widget, allocation);
}
/** _s_connected:
 *
 * sync the state of the client immeidately after the connection is established
 */
static void _s_connected ( GtkWidget  * widget, GdkSocketNativeID target, GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GtkWidget * toplevel;
	GET_OBJECT(widget, menu_bar, priv);
	priv->detached = TRUE;
	_sync_remote_state(menu_bar);
	_sync_local_state(menu_bar);
}
static void _s_shutdown ( GtkWidget * widget, GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	priv->detached = FALSE;	
	
	gtk_widget_reset_rc_styles(widget);
	gtk_widget_restore_default_style(widget);
	gtk_widget_queue_resize(widget);
	if(GTK_WIDGET_REALIZED(widget)){
	/* TODO: figure out how to detect a sudden death of server */
		gtk_widget_unrealize(widget);
		gtk_widget_realize(widget);
		gtk_widget_unmap(widget);
		gtk_widget_map(widget);	
	/* for a regular shutdown, following is enough 
		gdk_window_reparent(menu_bar->container, widget->window, 0, 0);
		gdk_window_show(menu_bar->container);
		gdk_window_invalidate_rect(menu_bar->container, NULL, TRUE);
	*/
	}
}
static void _s_position_set 		( GtkWidget  * widget, 
									  GdkPoint * pt,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	GtkAllocation * allocation = &menu_bar->allocation;
	menu_bar->allocation.x = pt->x;
	menu_bar->allocation.y = pt->y;

	if(GTK_WIDGET_REALIZED(widget)){
		gdk_window_move_resize(menu_bar->floater,
			allocation->x,
			allocation->y,
			allocation->width,
			allocation->height);
	}

	_do_size_allocate(widget, allocation);
}
static void
gtk_container_map_child (GtkWidget *child,
             gpointer   client_data)
{
  if (GTK_WIDGET_VISIBLE (child) &&
      !GTK_WIDGET_MAPPED (child))
    gtk_widget_map (child);
}
static void _s_visibility_set 		( GtkWidget  * widget, 
									  gboolean vis,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	if(vis){
		gtk_container_forall (GTK_CONTAINER (widget),
				gtk_container_map_child,
				NULL);
		gdk_window_show(menu_bar->container);
		gdk_window_show(menu_bar->floater);
	}else {
		gdk_window_hide(menu_bar->container);
		gdk_window_hide(menu_bar->floater);
	}	
}
static void _s_bgcolor_set	 		( GtkWidget  * widget, 
									  GdkColor * bgcolor,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	priv->bgcolor.red = bgcolor->red;
	priv->bgcolor.blue = bgcolor->blue;
	priv->bgcolor.green = bgcolor->green;
	LOG("new bg color %d, %d, %d", priv->bgcolor.red, priv->bgcolor.green, priv->bgcolor.blue);
	_sync_local_state(menu_bar);;
}
static gboolean _s_delete_event			( GtkWidget * widget,
									  GdkEvent * event,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	
	if(event->any.window == menu_bar->floater){
	LOG("it's from container window, ignore delete event");
		return TRUE;
	}
	LOG("it's from widget window, do delete event");
	return FALSE;
}

static gint
_expose (GtkWidget      *widget,
		     GdkEventExpose *event)
{
	gint border;
	GET_OBJECT(widget, menu_bar, priv);

	g_return_val_if_fail (event != NULL, FALSE);
	LOG_FUNC_NAME;

	if (GTK_WIDGET_DRAWABLE (widget))
    {
		border = GTK_CONTAINER(widget)->border_width;
		LOG("Expose from %p", event->window);
		
		if(event->window == menu_bar->container){
			if(!priv->detached){
				gtk_paint_box (widget->style,
						menu_bar->container,
						GTK_WIDGET_STATE (widget),
						GTK_SHADOW_NONE,
						&event->area, widget, "menubar",
						border, border,
						menu_bar->allocation.width - border * 2,
						menu_bar->allocation.height - border * 2);
			} else {
		/*FIXME: figure out why we always get a very small area if the client and server is in different process. */
				LOG("area=%d, %d, %d,%d", event->area);
				gdk_window_clear_area(menu_bar->container, 
					event->area.x, 
					event->area.y, 
					event->area.width, 
					event->area.height);
				
			}
		} else LOG("event not from container, ignore");

		(* GTK_WIDGET_CLASS (gtk_global_menu_bar_parent_class)->expose_event) (widget, event);
    }

  return FALSE;
}


static void
_hierarchy_changed (GtkWidget *widget,
				GtkWidget *old_toplevel)
{
	GtkWidget *toplevel;  
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
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
	GtkWidget *toplevel;  

	GdkWindowAttr attributes;
	guint attributes_mask;

	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);

/*TODO: remove calling the parent realize function. use our own instead to
 * avoid side effects.*/
	GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->realize(widget);

	attributes.x = 0;
	attributes.y = 0;
	attributes.width = menu_bar->allocation.width;
	attributes.height = menu_bar->allocation.height; 
/*NOTE: if set this to GDK_WINDOW_CHILD, we can put it anywhere we want without
 * WM's decorations! HOWever child doesn't work very well*/
	attributes.window_type = GDK_WINDOW_CHILD;

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
		/*gtk_widget_get_parent_window(widget)*/ widget->window, &attributes, attributes_mask);
	//gdk_window_set_user_data (menu_bar->container, widget);

	attributes.x = menu_bar->allocation.x;
	attributes.y = menu_bar->allocation.y;
	attributes.width = menu_bar->allocation.width;
	attributes.height = menu_bar->allocation.height; 
/*NOTE: if set this to GDK_WINDOW_CHILD, we can put it anywhere we want without
 * WM's decorations! HOWever child doesn't work very well*/
	attributes.window_type = GDK_WINDOW_TEMP;

	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget);

	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	/* FIXME: I don't think it will need visual and colormap, 
 	 * let me try to remove these later*/
	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	menu_bar->floater = gdk_window_new (
		gtk_widget_get_root_window(widget), &attributes, attributes_mask);

	gdk_window_set_user_data (menu_bar->container, widget);
	gdk_window_set_user_data (menu_bar->floater, widget);

	gtk_container_forall(GTK_CONTAINER(widget), 
           (GtkCallback)(_set_child_parent_window), 
           (gpointer)(menu_bar->container));


	_sync_remote_state(menu_bar);
	_sync_local_state(menu_bar);
}
static void
_unrealize (GtkWidget * widget){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->unrealize(widget);
	gdk_window_destroy(menu_bar->container);
	gdk_window_destroy(menu_bar->floater);
	gnomenu_client_helper_send_unrealize(menu_bar->helper);
}
static void
_map (GtkWidget * widget){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	g_return_if_fail(GTK_WIDGET_REALIZED(widget));
	if(!priv->detached){
		gdk_window_show(menu_bar->container);
	}
	gdk_window_show(widget->window);
	GTK_WIDGET_CLASS(gtk_global_menu_bar_parent_class)->map(widget);
}

static void
_insert (GtkMenuShell * menu_shell, GtkWidget * widget, gint pos){
	LOG_FUNC_NAME;
	GtkRequisition req;
	GET_OBJECT(menu_shell, menu_bar, priv);
	GTK_MENU_SHELL_CLASS(gtk_global_menu_bar_parent_class)->insert(menu_shell, widget, pos);
	LOG("widget name = %s", gtk_widget_get_name(widget));
	if(GTK_WIDGET_REALIZED(menu_shell)) {
		_set_child_parent_window(widget, menu_bar->container);
	}
	if(priv->detached){
/*		We depend on widget signal loop
		_calc_size_request(menu_bar, &req);
		LOG("widget req: %d, %d", widget->requisition);
		gnomenu_client_helper_request_size(menu_bar->helper, &req);
*/
	}
}

static void
_remove (GtkContainer * container, GtkWidget * widget){
	LOG_FUNC_NAME;
	GtkRequisition req;
	GET_OBJECT(container, menu_bar, priv);
	GTK_CONTAINER_CLASS(gtk_global_menu_bar_parent_class)->remove(container, widget);
	if(priv->detached){
/*		we depend on widget signal loop.
 *		calc_size_request(menu_bar, &req);
		gnomenu_client_helper_request_size(menu_bar->helper, &req);
*/
	}
}
static void _sync_remote_state				( GtkGlobalMenuBar * _self){
	LOG_FUNC_NAME;
	GtkWidget * toplevel;
	GET_OBJECT(_self, menu_bar, priv);
	if(priv->detached){
		if(GTK_WIDGET_REALIZED(menu_bar)){
			gnomenu_client_helper_send_realize(menu_bar->helper, menu_bar->floater);
		}
		toplevel = gtk_widget_get_toplevel(GTK_WIDGET(menu_bar));
		if(GTK_WIDGET_TOPLEVEL(toplevel)){
			if(GTK_WIDGET_REALIZED(toplevel)){
				gnomenu_client_helper_send_reparent(menu_bar->helper, toplevel->window);
			}
		}
	}
}
static void _sync_local_state				( GtkGlobalMenuBar * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, menu_bar, priv);
	if(priv->detached)
		gtk_widget_queue_resize(menu_bar);

	if(GTK_WIDGET_REALIZED(menu_bar)){
		GdkColormap * colormap = gtk_widget_get_colormap(GTK_WIDGET(menu_bar));
		gdk_rgb_find_color(colormap, &priv->bgcolor);
		gdk_window_set_background(menu_bar->container, &priv->bgcolor);
		gdk_window_invalidate_rect(menu_bar->container, NULL, TRUE);
		if(priv->detached){
			gdk_window_reparent(menu_bar->container, menu_bar->floater, 0, 0);
		}
	}
}
static void 
_calc_size_request (
	GtkWidget * widget,
	GtkRequisition * requisition) {
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
	LOG("request:%d, %d", *requisition);
}
static void
_set_child_parent_window (GtkWidget * widget, GdkWindow * window){
	LOG_FUNC_NAME;
	gtk_widget_set_parent_window(widget, window);
	if(GTK_WIDGET_REALIZED(widget)){
		gtk_widget_unrealize(widget);
		gtk_widget_realize(widget);
		LOG("realize hack");
	}
	if(GTK_WIDGET_VISIBLE(widget)){
		gtk_widget_unmap(widget);
		gtk_widget_map(widget);
		gtk_widget_show(widget);
		LOG("map hack");
	}
}
static void
_do_size_allocate (GtkWidget * widget,
	GtkAllocation * allocation){
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
	LOG("x=%d, y=%d, width=%d, height=%d\n", *allocation);
	g_return_if_fail (GTK_IS_MENU_BAR (widget));
	g_return_if_fail (allocation != NULL);

	GET_OBJECT(widget, glb_menu_bar, glb_priv);
	menu_bar = GTK_MENU_BAR (widget);
	menu_shell = GTK_MENU_SHELL (widget);

	if(GTK_WIDGET_REALIZED(widget)){
		gdk_window_move_resize(glb_menu_bar->container,
			0,
			0,
			allocation->width,
			allocation->height);
	}
	direction = gtk_widget_get_direction (widget);

	gtk_widget_style_get (widget, "internal-padding", &ipadding, NULL);
	LOG("internal-padding = %d", ipadding);
	LOG("border_widget = %d", GTK_CONTAINER(menu_bar)->border_width);
	pack_direction = gtk_menu_bar_get_pack_direction(menu_bar);
	child_pack_direction = gtk_menu_bar_get_child_pack_direction(menu_bar);
  
	if (menu_shell->children) {
		child_allocation.x = (GTK_CONTAINER (menu_bar)->border_width +
				ipadding + 
				BORDER_SPACING);
		child_allocation.y = (GTK_CONTAINER (menu_bar)->border_width +
				BORDER_SPACING);
      
		if (pack_direction == GTK_PACK_DIRECTION_LTR ||
			pack_direction == GTK_PACK_DIRECTION_RTL) {
			child_allocation.height = MAX (1, (gint)allocation->height - child_allocation.y * 2);

			offset = child_allocation.x; 	/* Window edge to menubar start */
			ltr_x = child_allocation.x;

			children = menu_shell->children;
			while (children) {
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
				&& (GTK_MENU_ITEM(child)->right_justify)) {
					ltr_x = allocation->width -
					child_requisition.width - offset;
				}
				if (GTK_WIDGET_VISIBLE (child)) {
					if ((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_LTR))
						child_allocation.x = ltr_x;
					else
						child_allocation.x = allocation->width
											- child_requisition.width - ltr_x; 

					child_allocation.width = child_requisition.width;

					gtk_menu_item_toggle_size_allocate (GTK_MENU_ITEM (child),
									toggle_size);
					gtk_widget_size_allocate (child, &child_allocation);

					ltr_x += child_allocation.width;
				}
			}
		} else {
			child_allocation.width = MAX (1, (gint)allocation->width - child_allocation.x * 2);

			offset = child_allocation.y; 	/* Window edge to menubar start */
			ltr_y = child_allocation.y;

			children = menu_shell->children;
			while (children) {
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
					&& (GTK_MENU_ITEM(child)->right_justify)) {
					ltr_y = allocation->height -
					child_requisition.height - offset;
				}
				if (GTK_WIDGET_VISIBLE (child)) {
					if ((direction == GTK_TEXT_DIR_LTR) ==
						(pack_direction == GTK_PACK_DIRECTION_TTB)) 
						child_allocation.y = ltr_y;
					else
					child_allocation.y = allocation->height
										- child_requisition.height - ltr_y; 

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

/*
vim:ts=4:sw=4
*/
