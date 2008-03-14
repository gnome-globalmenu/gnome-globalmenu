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
#include "menubar.h"
#include "quirks.h"

#define BORDER_SPACING  0
#define DEFAULT_IPADDING 1
#define GNOMENU_MENU_BAR_GET_PRIVATE(o)  \
	_get_private(o)

#define GET_OBJECT(_s, sgmb, p) \
	GnomenuMenuBar * sgmb = (GnomenuMenuBar*)_s; \
	GnomenuMenuBarPrivate * p = GNOMENU_MENU_BAR_GET_PRIVATE(_s);

#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("<GnomenuMenuBar>::" fmt,  ## args)
#else
#define LOG(fmt, args...)
#endif
#define LOG_FUNC_NAME LOG("%s", __func__)

/* Properties */
enum {
  PROP_0,
  PROP_PACK_DIRECTION,
  PROP_CHILD_PACK_DIRECTION,
  PROP_QUIRK,
};
typedef struct {
	gboolean overflowed;
	GtkMenuItem * menu_item;
} MenuItemInfo;

typedef struct 
{
/*Data struct level compatability with GtkMenuBar*/
	GtkPackDirection pack_direction; 
	GtkPackDirection child_pack_direction; 

	gboolean disposed;
	gboolean detached;

	gboolean widget_visible;
	gboolean widget_pack_direction;
	gboolean widget_child_pack_direction;

	GHashTable * menu_items;
	GtkWidget * arrow_button;
	GtkWidget * arrow;
	gboolean show_arrow;
	GtkMenu	* popup_menu;

	GnomenuClientHelper * helper;
	GdkWindow * container;
	GdkWindow * floater;
	GtkAllocation allocation;
	GtkRequisition requisition;
	gint x;
	gint y;
	GnomenuQuirkMask quirk;
} GnomenuMenuBarPrivate;

static GnomenuMenuBarPrivate * _get_private(gpointer o){
	if(GNOMENU_IS_MENU_BAR(o)){
  		return (G_TYPE_INSTANCE_GET_PRIVATE ((o), GNOMENU_TYPE_MENU_BAR, GnomenuMenuBarPrivate));
	} else {
  		return (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_MENU_BAR, GnomenuMenuBarPrivate));
	}
}


/* GObject interface */
static GObject * _constructor 		( GType type, guint n_construct_properties, 
									  GObjectConstructParam *construct_params );
static void _dispose 				( GObject * object );
static void _finalize 				( GObject * object );
static void _set_property      		( GObject *object, guint prop_id, 
									  const GValue *value, GParamSpec * pspec );
static void _get_property			( GObject *object, guint prop_id, 
									  GValue *value, GParamSpec * pspec );
static void _s_notify		( GObject * object, GParamSpec * pspec, gpointer data);

/* GtkWidget interface */
static void _size_request			( GtkWidget			* widget,
									  GtkRequisition	* requisition);
static void _size_allocate			( GtkWidget			* widget,
									  GtkAllocation		* allocation);
static void _realize 				( GtkWidget * widget);
static void _unrealize 				( GtkWidget * widget);
static void _map 					( GtkWidget * widget);
static void _style_set				( GtkWidget * widget, GtkStyle * previous_style);
static gint _expose 				( GtkWidget       *widget,
									  GdkEventExpose  *event);
/* GtkWidget interface handlers */
static gboolean _s_motion_notify_event( GtkWidget * widget, 
									  GdkEventMotion * event,
									  gpointer userdata);
static gboolean _s_button_press_event ( GtkWidget * widget,
									  GdkEventButton * event,
									  gpointer userdata);
static gboolean _s_configure_event	( GtkWidget * widget,
									  GdkEventConfigure * event,
									  gpointer * data);
/* GtkMenuShell Interface */
static void _insert 				( GtkMenuShell * menu_shell, 
									  GtkWidget * widget, gint pos);
/* GtkContainer Inteface */
static void _remove 				( GtkContainer * container, GtkWidget * widget);
static void _forall					( GtkContainer    *container,
									  gboolean     include_internals,
									  GtkCallback      callback,
									  gpointer     callback_data);
/* Signal handler for the helper */
static void _s_size_request			( GtkWidget * widget,
									  GtkRequisition * requisition, 
									  GnomenuClientHelper * helper);

static void _s_size_allocate 		( GtkWidget       *widget,
								      GtkAllocation   *allocation, 
									  GnomenuClientHelper * helper);
static void _s_connected 			( GtkWidget  * menubar, 
									  GnomenuSocketNativeID target, 
									  GnomenuClientHelper * helper); 
static void _s_shutdown				( GtkWidget * menubar,
									  GnomenuClientHelper * helper);
static void _s_position_set 		( GtkWidget  * menubar, 
									  GdkPoint * pt,
									  GnomenuClientHelper * helper); 
static void _s_visibility_set 		( GtkWidget  * menubar, 
									  gboolean vis,
									  GnomenuClientHelper * helper); 
static void _s_orientation_set 		( GtkWidget  * menubar, 
									  GtkOrientation orientation,
									  GnomenuClientHelper * helper); 
static void _s_background_set	 	( GtkWidget  * menubar, 
									  GdkColor * bgcolor,
									  GdkPixmap * pixmap,
									  GnomenuClientHelper * helper); 
static void _s_arrow_button_clicked		( GtkWidget * menubar,
									  GtkWidget * arrow_button);
static void _s_popup_menu_deactivated	( GtkWidget * menubar,
									  GtkWidget * popup_menu);
/* workaround the delete event*/
static gboolean _s_delete_event			( GtkWidget * widget,
									  GdkEvent * event,
									  GnomenuClientHelper * helper);
static void _s_hierarchy_changed		( GtkWidget 		* widget,
									  GtkWidget 		* old_toplevel,
									  gpointer data);
static void _s_notify_has_toplevel_focus ( GnomenuMenuBar * self, GParamSpec * pspec, GtkWindow * window);
										
/* utility functions*/
static void _calc_size_request		( GtkWidget * widget, GtkRequisition * requisition);
static void _do_size_allocate		( GtkWidget * widget, GtkAllocation * allocation);
static void 
	_set_child_parent_window		( GtkWidget * widget, GdkWindow * window);
static void _sync_remote_state		( GnomenuMenuBar * menubar);
static void _sync_local_state		( GnomenuMenuBar * menubar);
static void _reset_style			( GtkWidget * widget);
static void _build_popup_menu 		( GnomenuMenuBar * self);


//#define gnomenu_menu_bar_get_type _gnomenu_menu_bar_get_type
//G_DEFINE_TYPE (GnomenuMenuBar, gnomenu_menu_bar, GTK_TYPE_MENU_BAR)
//#undef gnomenu_menu_bar_get_type
void gnomenu_menu_bar_init (GnomenuMenuBar * self);
static void gnomenu_menu_bar_class_init (GnomenuMenuBarClass * klass);
//static gpointer gnomenu_menu_bar_parent_class = NULL;
static gpointer gnomenu_menu_bar_menu_shell_class = NULL;

void gnomenu_menu_bar_class_intern_init (gpointer klass){
//	gnomenu_menu_bar_parent_class = g_type_class_peek_parent(klass);
	gnomenu_menu_bar_menu_shell_class = g_type_class_peek(GTK_TYPE_MENU_SHELL);
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

static void
gnomenu_menu_bar_class_init (GnomenuMenuBarClass *class)
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
	widget_class->map = _map;
	widget_class->style_set = _style_set;
//	menu_shell_class->submenu_placement = GTK_TOP_BOTTOM;
	menu_shell_class->insert = _insert;

//	menu_shell_class->get_popup_delay = gtk_menu_bar_get_popup_delay; 
//	menu_shell_class->move_current = gtk_menu_bar_move_current; 

	container_class->remove = _remove;
	container_class->forall = _forall;

	g_type_class_add_private (gobject_class, sizeof (GnomenuMenuBarPrivate));  

	g_object_class_install_property (gobject_class, 
			PROP_QUIRK,
			g_param_spec_flags ("quirk",
						"quirk",
						"quirk",
						GNOMENU_TYPE_QUIRK_MASK, gnomenu_get_default_quirk(),
						G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
}

void
gnomenu_menu_bar_init (GnomenuMenuBar *object)
{
	LOG_FUNC_NAME;
	GET_OBJECT(object, menu_bar, priv);
	GtkWidget * arrow;
	priv->helper = gnomenu_client_helper_new();
	priv->menu_items = g_hash_table_new_full(NULL, NULL, NULL, g_free);
	priv->popup_menu = gtk_menu_new();
	priv->arrow_button = gtk_toggle_button_new();
	priv->arrow = gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_NONE);
	priv->show_arrow = FALSE;
	priv->disposed = FALSE;
	priv->detached = FALSE;


	priv->allocation.width = 200;
	priv->allocation.height = 20;
	priv->allocation.x = 0;
	priv->allocation.y = 0;
	priv->requisition.width = 0;
	priv->requisition.height = 0;
	priv->x = 0;
	priv->y = 0;

	priv->widget_pack_direction = GTK_PACK_DIRECTION_LTR;
	priv->widget_child_pack_direction = GTK_PACK_DIRECTION_LTR;
}

/**
 * gnomenu_menu_bar_new:
 *
 * Oops, this is the only function I exposed to you! Don't be mad,
 * because #GnomenuMenuBar is a subclass of #GtkMenuBar,
 * and you can (by definition you always can) use any function who
 * work for a #GtkMenuBar on a #GnomenuMenuBar.
 *
 * If you are patching GTK to replace #GtkMenuBar with 
 * #GnomenuMenuBar completely via #gtk_menu_bar_new, 
 * don't use this function. The interface compatibility
 * doesn't means everything. At a first glance the potentially vulnerabilities
 * are:
 *
 * 1 if some GTK client subclassed GtkMenuBar, and you rudely replace
 * #GtkMenuBar with #GnomenuMenuBar. The entire #GType inheriting
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
gnomenu_menu_bar_new (void)
{
  return g_object_new (GNOMENU_TYPE_MENU_BAR, NULL);
}


static GObject* _constructor(GType type, 
		guint n_construct_properties,
		GObjectConstructParam *construct_params){

	GObject * object = (G_OBJECT_CLASS(gnomenu_menu_bar_menu_shell_class)->constructor)(
		type, n_construct_properties, construct_params);

	GET_OBJECT(object, menu_bar, priv);

	gtk_container_set_border_width(object, 0);

	gtk_widget_show(priv->arrow);
	gtk_container_add(GTK_CONTAINER(priv->arrow_button), priv->arrow);
	gtk_button_set_relief(priv->arrow_button, GTK_RELIEF_NONE);
	gtk_widget_set_parent(priv->arrow_button, GTK_CONTAINER(menu_bar));

	g_signal_connect_swapped(G_OBJECT(priv->helper), "size-allocate",
				G_CALLBACK(_s_size_allocate), menu_bar);
	g_signal_connect_swapped(G_OBJECT(priv->helper), "size-query",
				G_CALLBACK(_s_size_request), menu_bar);

	g_signal_connect_swapped(G_OBJECT(priv->helper), "position-set",
				G_CALLBACK(_s_position_set), menu_bar);
	g_signal_connect_swapped(G_OBJECT(priv->helper), "background-set",
				G_CALLBACK(_s_background_set), menu_bar);
	g_signal_connect_swapped(G_OBJECT(priv->helper), "visibility-set",
				G_CALLBACK(_s_visibility_set), menu_bar);
	g_signal_connect_swapped(G_OBJECT(priv->helper), "orientation-set",
				G_CALLBACK(_s_orientation_set), menu_bar);

	g_signal_connect_swapped(G_OBJECT(priv->helper), "connected",
				G_CALLBACK(_s_connected), menu_bar);
	g_signal_connect_swapped(G_OBJECT(priv->helper), "shutdown",
				G_CALLBACK(_s_shutdown), menu_bar);

	g_signal_connect_swapped(G_OBJECT(priv->arrow_button), "clicked",
				G_CALLBACK(_s_arrow_button_clicked), menu_bar);
	g_signal_connect_swapped(G_OBJECT(priv->popup_menu), "deactivate",
				G_CALLBACK(_s_popup_menu_deactivated), menu_bar);
	
	g_signal_connect(G_OBJECT(menu_bar), "delete-event",
				G_CALLBACK(_s_delete_event), priv->helper);
	g_signal_connect(G_OBJECT(menu_bar), "hierarchy-changed",
				G_CALLBACK(_s_hierarchy_changed), priv->helper);
	g_signal_connect(G_OBJECT(menu_bar), "button-press-event",
				G_CALLBACK(_s_button_press_event), priv->helper);
	g_signal_connect(G_OBJECT(menu_bar), "configure-event",
				G_CALLBACK(_s_configure_event), priv->helper);
	g_signal_connect(G_OBJECT(menu_bar), "motion-notify-event",
				G_CALLBACK(_s_motion_notify_event), priv->helper);
	g_signal_connect(G_OBJECT(menu_bar), "notify",
				G_CALLBACK(_s_notify), NULL);

	gnomenu_client_helper_start(priv->helper);
	return object;
}

static void
_dispose (GObject * _object){
	LOG_FUNC_NAME;
	GET_OBJECT(_object, menu_bar, priv);
	if(!priv->disposed){
		priv->disposed = TRUE;	
		g_object_unref(priv->helper);
	}
	g_hash_table_remove_all(priv->menu_items);
	G_OBJECT_CLASS(gnomenu_menu_bar_menu_shell_class)->dispose(_object);
}
static void
_finalize(GObject * _object){
	LOG_FUNC_NAME;
	GET_OBJECT(_object, menu_bar, priv);	
	gtk_widget_unparent(priv->arrow_button);
	g_hash_table_destroy(priv->menu_items);
	gtk_widget_destroy(priv->popup_menu);
	G_OBJECT_CLASS(gnomenu_menu_bar_menu_shell_class)->finalize(_object);
}
static void
_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
	GET_OBJECT(object, self, priv); 
	switch (prop_id) {
		case PROP_PACK_DIRECTION:
			gtk_menu_bar_set_pack_direction (self, g_value_get_enum (value));
		break;
		case PROP_CHILD_PACK_DIRECTION:
			gtk_menu_bar_set_child_pack_direction (self, g_value_get_enum (value));
		break;
		case PROP_QUIRK:
			{
			GnomenuQuirkMask q = g_value_get_flags (value);
			if(priv->quirk != q){
				priv->quirk = q;
				if(GNOMENU_HAS_QUIRK(priv->quirk, FORCE_SHOW_ALL) &&!GTK_WIDGET_REALIZED(self)){
					gtk_widget_realize(self);
				}
			}
			}
		break;
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
		case PROP_PACK_DIRECTION:
		  g_value_set_enum (value, gtk_menu_bar_get_pack_direction (self));
		  break;
		case PROP_CHILD_PACK_DIRECTION:
		  g_value_set_enum (value, gtk_menu_bar_get_child_pack_direction (self));
		  break;
		case PROP_QUIRK:
			g_value_set_flags (value, priv->quirk);
		break;
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
	GtkPackDirection pack_direction;
	LOG_FUNC_NAME;

	GET_OBJECT(widget, menu_bar, priv);
	if (priv->detached) {
		/* This is a quirk. Workaround for evolution and other
 		   Applications that changes menu item size and use
		   'gtk_widget_queue_resize' or whatever on the menu bar
		*/
		_calc_size_request(widget, &priv->requisition);
		LOG("size_request %d, %d", priv->requisition);
		gnomenu_client_helper_request_size(priv->helper, &priv->requisition); 
		requisition->width = 0;
		requisition->height = 0;
 	} else {
		if(!GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)){
			_calc_size_request(widget, requisition);
			priv->requisition = * requisition;
		} else { /* ROAMING */
			requisition->width = 0;
			requisition->height = 0;
			_calc_size_request(widget, &priv->requisition);
		}
	}
	pack_direction = gtk_menu_bar_get_pack_direction(widget);	
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
	widget->requisition = *requisition;
}
static void
_s_size_request (
	GtkWidget * widget,
	GtkRequisition *  requisition,
	GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	_calc_size_request (widget, requisition);
	priv->requisition = *requisition;
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
	if (priv->detached || GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)) {
		if(GTK_WIDGET_REALIZED(widget)){
			gdk_window_move_resize(widget->window, 
				/*allocation->x*/ -1,
				/*allocation->y*/ -1,
				/*allocation->width*/ 1,
				/*allocation->height*/ 1) ;
		}
		/* This is a quirk. Workaround for evolution and other
 		   Applications that changes menu item size and use
		   'gtk_widget_queue_resize' or whatever on the menu bar
		*/
		_do_size_allocate(widget, &priv->allocation);
		
	} else {
		if(GTK_WIDGET_REALIZED(widget)){
			gdk_window_move_resize(widget->window, 
				allocation->x,
				allocation->y,
				allocation->width,
				allocation->height);
		}
		if(!GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)){
			priv->allocation = *allocation;	
			_do_size_allocate(widget, allocation);
		}else { /* ROAMING */
			/*rely on the configure_event */
			/*
			priv->allocation = *allocation;	
			if(GTK_WIDGET_REALIZED(widget)){
				gdk_window_resize(priv->floater,
					priv->allocation.width,
					priv->allocation.height);
			}
			_do_size_allocate(widget, &priv->allocation);
			*/
		}
	}
}
static void
_s_size_allocate (GtkWidget * widget, 
	GtkAllocation * allocation,
	GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);

	if(!memcmp(&priv->allocation, allocation, sizeof(GtkAllocation))) return;

	priv->allocation = *allocation;	
	if(GTK_WIDGET_REALIZED(widget)){
		gdk_window_move_resize(priv->floater,
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
static void _s_connected ( GtkWidget  * widget, GnomenuSocketNativeID target, GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GtkWidget * toplevel;
	GET_OBJECT(widget, menu_bar, priv);
	priv->detached = TRUE;
	if(GTK_WIDGET_REALIZED(widget)){
		gtk_widget_unrealize(widget);
		gtk_widget_realize(widget);
		if(GTK_WIDGET_VISIBLE(widget)){
			gtk_widget_map(widget);
		}
	}
}
static void _s_shutdown ( GtkWidget * widget, GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	GList * l;
	priv->detached = FALSE;	

	_reset_style(widget);	

	if(GTK_WIDGET_REALIZED(widget)){
		if(!GNOMENU_HAS_QUIRK(priv->quirk, HIDE_ON_QUIT)){
			gtk_widget_unrealize(widget);

			priv->pack_direction = priv->widget_pack_direction;
			priv->child_pack_direction = priv->widget_child_pack_direction;
			gtk_widget_queue_resize(widget);
			for (l = GTK_MENU_SHELL (menu_bar)->children; l; l = l->next)
				gtk_widget_queue_resize (GTK_WIDGET (l->data));

			if(priv->widget_visible){ /* fake to be unvisible, so
										that _show will do real show work*/
				GTK_WIDGET_UNSET_FLAGS(widget, GTK_VISIBLE);
				gtk_widget_show(widget);
			} else /*we were realized*/
				gtk_widget_realize(widget);
		} else {
			LOG("use quirk HIDE_ON_QUIT");
		//	gtk_widget_unrealize(widget);
			gtk_widget_hide_all(widget);
		}
	}
}
static void _s_position_set 		( GtkWidget  * widget, 
									  GdkPoint * pt,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);

	priv->x = pt->x;
	priv->y = pt->y;
	if(GTK_WIDGET_REALIZED(widget)){
		gdk_window_move(priv->container,
			priv->x,
			priv->y);
	}
}
static void
gtk_container_map_child (GtkWidget *child,
             gpointer   client_data)
{
	if(GTK_WIDGET_VISIBLE(child) && 	
		!GTK_WIDGET_MAPPED(child))
		gtk_widget_map(child);
}
static void _s_visibility_set 		( GtkWidget  * widget, 
									  gboolean vis,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	LOG("vis=%d", vis);
	if(!GTK_WIDGET_REALIZED(widget))
		return;

	if(vis){
		if(GNOMENU_HAS_QUIRK(priv->quirk, FORCE_SHOW_ALL)){
			gtk_widget_show_all(widget);
		}
		gtk_container_forall(GTK_CONTAINER(widget),
			gtk_container_map_child,
			NULL);
		gdk_window_show(priv->container);
		gdk_window_show(priv->floater);

		GTK_WIDGET_SET_FLAGS(widget, GTK_VISIBLE);
	}else {
		gdk_window_hide(priv->container);
		gdk_window_hide(priv->floater);

		GTK_WIDGET_UNSET_FLAGS(widget, GTK_VISIBLE);
	}	
	LOG("done");
}
static void _s_orientation_set 		( GtkWidget  * widget, 
									  GtkOrientation orientation,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	GList * l;
	switch(orientation){
		case GTK_ORIENTATION_HORIZONTAL:
			priv->pack_direction = GTK_PACK_DIRECTION_LTR;
			priv->child_pack_direction = GTK_PACK_DIRECTION_LTR;
		break;
		case GTK_ORIENTATION_VERTICAL:
			priv->pack_direction = GTK_PACK_DIRECTION_TTB;
			priv->child_pack_direction = GTK_PACK_DIRECTION_TTB;
		break;
	}
	gtk_widget_queue_resize(widget);
	for (l = GTK_MENU_SHELL (menu_bar)->children; l; l = l->next)
		gtk_widget_queue_resize (GTK_WIDGET (l->data));
}
static void _s_background_set	 		( GtkWidget  * widget, 
									  GdkColor * color,
									  GdkPixmap * pixmap,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
/*mostly based on panel-applet.c::panel_applet_update_background_for_widget */
	GtkStyle   *style;

	GET_OBJECT(widget, menu_bar, priv);
	_reset_style(widget);
	if(color){
		LOG("new bg color %d, %d, %d", color->red, color->green, color->blue);
		gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, color);
	}
	if(pixmap){
		GdkPixmap * adjusted;
		gint w, h;
		gint d;
		gint dw;
		gdk_drawable_get_size(pixmap, &w, &h);
		LOG("size of pixmap, %d, %d", w, h);
		d = gdk_drawable_get_depth(pixmap);
		dw = gdk_drawable_get_depth(priv->container);
		LOG("d(pixmap)=%d, d(container)=%d", d, dw);
		{
			GdkGC * gc;
			GdkPixbuf * pixbuf = gdk_pixbuf_get_from_drawable(NULL,
							pixmap, gdk_drawable_get_colormap(pixmap) , 
							0, 0, 0, 0, w, h);
			adjusted = gdk_pixmap_new(priv->container, w, h, -1);
			gc = gdk_gc_new(adjusted);
			gdk_draw_pixbuf(adjusted, gc, pixbuf, 0, 0, 0, 0, w, h, 
							GDK_RGB_DITHER_NONE, 0, 0);
			g_object_unref(pixbuf);
			g_object_unref(gc);
		} 
		style = gtk_style_copy (widget->style);
		if (style->bg_pixmap[GTK_STATE_NORMAL])
			g_object_unref (style->bg_pixmap[GTK_STATE_NORMAL]);
		style->bg_pixmap[GTK_STATE_NORMAL] = adjusted;
		gtk_widget_set_style (widget, style);
		g_object_unref (style);
	}
	_sync_local_state(menu_bar);;
}
static gboolean _s_delete_event			( GtkWidget * widget,
									  GdkEvent * event,
									  GnomenuClientHelper * helper){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	
	if(event->any.window == priv->floater){
	LOG("it's from container window, ignore delete event");
		return TRUE;
	}
	LOG("it's from widget window, do delete event");
	return FALSE;
}

static gboolean _s_button_press_event (GtkWidget * widget,
								GdkEventButton * event, gpointer userdata){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	return FALSE;	
}
static gboolean _s_configure_event	( GtkWidget * widget,
									  GdkEventConfigure * event,
									  gpointer * data){
	GET_OBJECT(widget, menu_bar, priv);
	if(event->window == priv->floater) {
		LOG("send=%d, %d %d %d %d", event->send_event, event->x, event->y, event->width, event->height);
		if(!priv->detached && GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)){
			/* it is user resizing.*/
			GtkAllocation allocation = {event->x, event->y, event->width, event->height};
			priv->allocation = allocation;
			_do_size_allocate(menu_bar, &allocation);
		}
	}
	return FALSE;
}
static gboolean _s_motion_notify_event	( GtkWidget * widget, 
									  GdkEventMotion * event, gpointer userdata){

	gboolean (* func)(GtkWidget * widget, GdkEventMotion * event);
	GET_OBJECT(widget, menu_bar, priv);
	
	if(event && event->is_hint){
	LOG_FUNC_NAME;
	}
	return FALSE;
}
static void _style_set				( GtkWidget * widget, GtkStyle * previous_style){
	GET_OBJECT(widget, menu_bar, priv);
	if (GTK_WIDGET_REALIZED(widget))
    gtk_style_set_background (widget->style, priv->container, widget->state);
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
		LOG("Expose from %p, area = %d, %d, %d, %d", event->window, event->area);
		
		if(event->window == widget->window){
			LOG("event from widget->window");
			if(priv->detached){
			/*work around, quirk, to paint widget->window with parent background, since it really has nothing*/
			GtkStyle * style = gtk_widget_get_style(gtk_widget_get_parent(widget));
				gtk_paint_flat_box (style,
						widget->window,
						GTK_WIDGET_STATE (widget),
						GTK_SHADOW_NONE,
						&event->area, widget, NULL,
						0, 0,
						widget->allocation.width,
						widget->allocation.height);
			}
		}
		if(event->window == priv->container){
			if(!priv->detached){
				gtk_paint_box (widget->style,
						priv->container,
						GTK_WIDGET_STATE (widget),
						GTK_SHADOW_NONE,
						&event->area, widget, "menubar",
						border, border,
						priv->allocation.width - border * 2,
						priv->allocation.height - border * 2);
			} else {
				gtk_paint_flat_box (widget->style,
						priv->container,
						GTK_WIDGET_STATE (widget),
						GTK_SHADOW_NONE,
						&event->area, widget, NULL,
						0, 0,
						priv->allocation.width,
						priv->allocation.height);
				
			}
		} else LOG("event not from container, ignore");
		{ 
			(* GTK_WIDGET_CLASS(gnomenu_menu_bar_menu_shell_class)->expose_event) (widget, event);
		}
    } else {
			LOG("visible: %d, mapped %d",  GTK_WIDGET_VISIBLE(widget),
					GTK_WIDGET_MAPPED(widget));

	}

  return FALSE;
}

static void _s_notify_title (GnomenuMenuBar * menu_bar, GParamSpec * spec, 
	GtkWindow * toplevel){
	const gchar * detail = gtk_window_get_role(toplevel);
	if(!detail) detail = gtk_window_get_title(toplevel);
	LOG("detail = %s", detail);
	g_object_set(menu_bar, 
			"quirk",
			gnomenu_get_detail_quirk(detail),
			NULL
			);
}
static void _s_notify_has_toplevel_focus ( GnomenuMenuBar * self, GParamSpec * pspec, GtkWindow * window){
	if(gtk_window_has_toplevel_focus(window)){
		GET_OBJECT(self, menu_bar, priv);
		LOG("receive toplevel focus");
		gnomenu_client_helper_send_parent_focus(priv->helper);
	}
}
static void
_s_hierarchy_changed (GtkWidget *widget,
				GtkWidget *old_toplevel, gpointer userdata)
{
	GtkWidget *toplevel;  
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	if(old_toplevel){
		g_signal_handlers_disconnect_by_func(
			old_toplevel, _s_notify_title, menu_bar);
		g_signal_handlers_disconnect_by_func(
			old_toplevel, _s_notify_has_toplevel_focus, menu_bar);
	}

	toplevel = gtk_widget_get_toplevel(widget);
	if(GTK_WIDGET_TOPLEVEL(toplevel)){
		if(GTK_IS_WINDOW(toplevel)){
			g_signal_connect_swapped(toplevel, "notify::role", 
			G_CALLBACK(_s_notify_title), menu_bar);
			g_signal_connect_swapped(toplevel, "notify::title", 
			G_CALLBACK(_s_notify_title), menu_bar);
			g_signal_connect_swapped(toplevel, "notify::has-toplevel-focus",
			G_CALLBACK(_s_notify_has_toplevel_focus), menu_bar);

			_s_notify_title(menu_bar, NULL, GTK_WINDOW(toplevel));
		}
		if(GTK_WIDGET_REALIZED(toplevel)){
/* NOTE: This signal is rarely captured, because usually a menubar is added to a toplevel
 * BEFORE the toplevel is realized. So we need to handle this in _realize. */
			gnomenu_client_helper_send_reparent(priv->helper, toplevel->window);
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

	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

	if(!priv->detached && !GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)){
		attributes.x = widget->allocation.x;
		attributes.y = widget->allocation.y;
		attributes.width = widget->allocation.width;
		attributes.height = widget->allocation.height;
	} else {
		attributes.x = -1; 
		attributes.y = -1; 
		attributes.width = 1;
		attributes.height = 1;
	}
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

	attributes.x = priv->allocation.x;
	attributes.y = priv->allocation.y;
	attributes.width = priv->allocation.width;
	attributes.height = priv->allocation.height; 
/*NOTE: if set this to GDK_WINDOW_CHILD, we can put it anywhere we want without
 * WM's decorations! HOWever child doesn't work very well*/

	if(priv->detached || !GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)){
		attributes.window_type = GDK_WINDOW_TEMP;
	} else {
		attributes.window_type = GDK_WINDOW_TOPLEVEL;
	}
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget);

	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	/* FIXME: I don't think it will need visual and colormap, 
 	 * let me try to remove these later*/
	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	priv->floater = gdk_window_new (
		gtk_widget_get_root_window(widget), &attributes, attributes_mask);
	attributes.x = 0;
	attributes.y = 0;
	attributes.width = MAX(priv->requisition.width, priv->allocation.width);
	attributes.height = MAX(priv->requisition.height, priv->allocation.height);
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
                GDK_LEAVE_NOTIFY_MASK |
				GDK_BUTTON2_MOTION_MASK |
				GDK_POINTER_MOTION_HINT_MASK);

	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	/* FIXME: I don't think it will need visual and colormap, 
 	 * let me try to remove these later*/
	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	if(!GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)){
		priv->container = gdk_window_new (
			 widget->window, &attributes, attributes_mask);
	} else {
		priv->container = gdk_window_new (
			 priv->floater, &attributes, attributes_mask);
	}

	gdk_window_set_user_data (priv->container, widget);
	gdk_window_set_user_data (priv->floater, widget);

	widget->style = gtk_style_attach (widget->style, priv->container);

	gtk_container_forall(GTK_CONTAINER(widget), 
           (GtkCallback)(_set_child_parent_window), 
           (gpointer)(priv->container));
	{
	GtkWidget * toplevel = gtk_widget_get_toplevel(widget);
	gdk_window_set_transient_for(priv->floater, toplevel->window);
	gdk_window_set_group(priv->floater, toplevel->window);
	}
	_reset_style(GTK_WIDGET(menu_bar));	
	_sync_remote_state(menu_bar);
	_sync_local_state(menu_bar);
}
static void
_unrealize (GtkWidget * widget){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	GTK_WIDGET_CLASS(gnomenu_menu_bar_menu_shell_class)->unrealize(widget);
	gdk_window_destroy(priv->container);
	gdk_window_destroy(priv->floater);
	gnomenu_client_helper_send_unrealize(priv->helper);
	GTK_WIDGET_UNSET_FLAGS(widget, GTK_REALIZED);
}
static void
_map (GtkWidget * widget){
	LOG_FUNC_NAME;
	GET_OBJECT(widget, menu_bar, priv);
	g_return_if_fail(GTK_WIDGET_REALIZED(widget));
	if(!priv->detached){
		gdk_window_show(priv->container);
		if(GNOMENU_HAS_QUIRK(priv->quirk, ROAMING)){
			gdk_window_show(priv->floater);
		}
	}
	gdk_window_show(widget->window);
	GTK_WIDGET_CLASS(gnomenu_menu_bar_menu_shell_class)->map(widget);
}

static void _s_notify 				( GObject * object, GParamSpec * pspec, gpointer data){
	GET_OBJECT(object, menu_bar, priv);
	const gchar * name = g_param_spec_get_name(pspec);
	struct {
		gchar * name;
		gpointer value;
	} * p, li [] = {
		{"visible", &priv->widget_visible},
		{"pack-direction", &priv->widget_pack_direction},
		{"child-pack-direction", &priv->widget_child_pack_direction},
		{NULL, NULL}
	};
	for(p = li; p->name; p++){
		if(g_str_equal(p->name, name)){
			LOG("widget property %s is modified. back it up",
				name);
			g_object_get(object, p->name, p->value, NULL);
			break;
		}
	}
	if(!p->name){
			LOG("other property %s", name);
	}
}
static void
_insert (GtkMenuShell * menu_shell, GtkWidget * widget, gint pos){
	LOG_FUNC_NAME;
	GtkRequisition req;
	MenuItemInfo * item_info = g_new0(MenuItemInfo, 1);

	GET_OBJECT(menu_shell, menu_bar, priv);
	GTK_MENU_SHELL_CLASS(gnomenu_menu_bar_menu_shell_class)->insert(menu_shell, widget, pos);
	LOG("widget name = %s", gtk_widget_get_name(widget));
	if(GTK_WIDGET_REALIZED(menu_shell)) {
		_set_child_parent_window(widget, priv->container);
	}
	if(GTK_IS_MENU_ITEM(widget)){
		item_info->menu_item = GTK_MENU_ITEM(widget);
	}
	g_hash_table_insert(priv->menu_items, widget, item_info);
	if(priv->detached){
/*		We depend on widget signal loop
		_calc_size_request(menu_bar, &req);
		LOG("widget req: %d, %d", widget->requisition);
		gnomenu_client_helper_request_size(priv->helper, &req);
*/
	}
}

static void
_remove (GtkContainer * container, GtkWidget * widget){
	LOG_FUNC_NAME;
	GtkRequisition req;

	GET_OBJECT(container, menu_bar, priv);
	GTK_CONTAINER_CLASS(gnomenu_menu_bar_menu_shell_class)->remove(container, widget);

	g_hash_table_remove(priv->menu_items, widget);

	if(priv->detached){
/*		we depend on widget signal loop.
 *		calc_size_request(menu_bar, &req);
		gnomenu_client_helper_request_size(priv->helper, &req);
*/
	}
}
static void _forall					( GtkContainer    *container,
									  gboolean     include_internals,
									  GtkCallback      callback,
									  gpointer     callback_data){
	GET_OBJECT(container, menu_bar, priv);
	GTK_CONTAINER_CLASS(gnomenu_menu_bar_menu_shell_class)->forall(container, 
					include_internals, callback, callback_data);
	
	if(include_internals){
		callback(priv->arrow_button, callback_data);
	}
}
static void _sync_remote_state				( GnomenuMenuBar * _self){
	LOG_FUNC_NAME;
	GtkWidget * toplevel;
	GET_OBJECT(_self, menu_bar, priv);
	if(priv->detached){
		if(GTK_WIDGET_REALIZED(menu_bar)){
			gnomenu_client_helper_send_realize(priv->helper, priv->floater);
		}
		toplevel = gtk_widget_get_toplevel(GTK_WIDGET(menu_bar));
		if(GTK_WIDGET_TOPLEVEL(toplevel)){
			if(GTK_WIDGET_REALIZED(toplevel)){
				gnomenu_client_helper_send_reparent(priv->helper, toplevel->window);
			}
		}
	}
}
static void _sync_local_state				( GnomenuMenuBar * _self){
	LOG_FUNC_NAME;
	GET_OBJECT(_self, menu_bar, priv);
	if(priv->detached)
		gtk_widget_queue_resize(GTK_WIDGET(menu_bar));

	if(GTK_WIDGET_REALIZED(menu_bar)){
		gdk_window_invalidate_rect(priv->container, NULL, TRUE);
		if(priv->detached){
			gdk_window_reparent(priv->container, priv->floater, 0, 0);
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
	GtkAllocation adjusted;
	GtkRequisition arrow_requisition;
	GtkAllocation arrow_allocation;
	LOG_FUNC_NAME;
	LOG("x=%d, y=%d, width=%d, height=%d\n", *allocation);

	g_return_if_fail (GTK_IS_MENU_BAR (widget));
	g_return_if_fail (allocation != NULL);

	GET_OBJECT(widget, menu_bar, priv);
	menu_shell = GTK_MENU_SHELL (widget);

	direction = gtk_widget_get_direction (widget);

	gtk_widget_style_get (widget, "internal-padding", &ipadding, NULL);

	pack_direction = gtk_menu_bar_get_pack_direction(GTK_MENU_BAR(menu_bar));
	child_pack_direction = gtk_menu_bar_get_child_pack_direction(GTK_MENU_BAR(menu_bar));

	adjusted = * allocation;
	adjusted.x = 0; /*the x, y offset is taken care by either widget->window or priv->floater*/
	adjusted.y = 0;
	gtk_widget_size_request(priv->arrow_button, &arrow_requisition);

	arrow_allocation.width = arrow_requisition.width;
	arrow_allocation.height = arrow_requisition.height;
	priv->show_arrow = FALSE;
	switch(pack_direction){
		case GTK_PACK_DIRECTION_LTR:
			if(allocation->width < priv->requisition.width){
				adjusted.width -= arrow_requisition.width;
				priv->show_arrow = TRUE;
			}
		break;
		case GTK_PACK_DIRECTION_RTL:
			if(allocation->width < priv->requisition.width){
				adjusted.x = arrow_requisition.width;
				priv->show_arrow = TRUE;
			}
		break;
		case GTK_PACK_DIRECTION_BTT:
			if(allocation->height < priv->requisition.height){
				adjusted.y = arrow_requisition.height;
				priv->show_arrow = TRUE;
			}
		break;
		case GTK_PACK_DIRECTION_TTB:
			if(allocation->height < priv->requisition.height){
				adjusted.height -= arrow_requisition.height;
				priv->show_arrow = TRUE;
			}
		break;
	}
	LOG("requisition = %d, %d", priv->requisition);
	LOG("show_arrow = %d", priv->show_arrow);
	LOG("adjusted = %d %d %d %d", adjusted);

	if(GTK_WIDGET_REALIZED(widget)){
		gdk_window_move_resize(priv->container,
			0,
			0,
			allocation->width,
			allocation->height);
	}
  
	if (menu_shell->children) {
		child_allocation.x = (GTK_CONTAINER (menu_bar)->border_width +
				ipadding + 
				BORDER_SPACING + adjusted.x);
		child_allocation.y = (GTK_CONTAINER (menu_bar)->border_width +
				BORDER_SPACING + adjusted.y);
      
		if (pack_direction == GTK_PACK_DIRECTION_LTR ||
			pack_direction == GTK_PACK_DIRECTION_RTL) {
			child_allocation.height = MAX (1, (gint)adjusted.height - child_allocation.y * 2);

			offset = child_allocation.x; 	/* Window edge to menubar start */
			ltr_x = child_allocation.x;

			children = menu_shell->children;
			if(priv->show_arrow) {
				arrow_allocation.height = adjusted.height;
				if(pack_direction == GTK_PACK_DIRECTION_LTR){
					arrow_allocation.x = adjusted.width;
					arrow_allocation.y = 0;
				} else { /*RTL*/
					arrow_allocation.x = 0;
					arrow_allocation.y = 0;
				}
			}
			while (children) {
				gint toggle_size;          
				MenuItemInfo * menu_info;

				child = children->data;
				children = children->next;

				menu_info = g_hash_table_lookup(priv->menu_items, child);
				menu_info->overflowed = (ltr_x > adjusted.width);

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
					ltr_x = adjusted.width -
					child_requisition.width - offset;
				}
				if (GTK_WIDGET_VISIBLE (child)) {
					if ((direction == GTK_TEXT_DIR_LTR) == (pack_direction == GTK_PACK_DIRECTION_LTR))
						child_allocation.x = ltr_x;
					else
						child_allocation.x = adjusted.width
											- child_requisition.width - ltr_x; 

					child_allocation.width = MAX(MIN(child_requisition.width, adjusted.width - ltr_x), 0);

					gtk_menu_item_toggle_size_allocate (GTK_MENU_ITEM (child),
									toggle_size);
					gtk_widget_size_allocate (child, &child_allocation);

					ltr_x += child_requisition.width;

				}
			}
		} else {
			child_allocation.width = MAX (1, (gint)adjusted.width - child_allocation.x * 2);

			offset = child_allocation.y; 	/* Window edge to menubar start */
			ltr_y = child_allocation.y;

			children = menu_shell->children;
			if(priv->show_arrow) {
				arrow_allocation.width = adjusted.width;
				if(pack_direction == GTK_PACK_DIRECTION_TTB){
					arrow_allocation.x = 0;
					arrow_allocation.y = adjusted.height;
				} else { /*BTT*/
					arrow_allocation.x = 0;
					arrow_allocation.y = 0;
				}
			}
			while (children) {
				gint toggle_size;          
				MenuItemInfo * menu_info;

				child = children->data;
				children = children->next;

				menu_info = g_hash_table_lookup(priv->menu_items, child);
				menu_info->overflowed = (ltr_y > adjusted.height);

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
					ltr_y = adjusted.height -
					child_requisition.height - offset;
				}
				if (GTK_WIDGET_VISIBLE (child)) {
					if ((direction == GTK_TEXT_DIR_LTR) ==
						(pack_direction == GTK_PACK_DIRECTION_TTB)) 
						child_allocation.y = ltr_y;
					else
					child_allocation.y = adjusted.height
										- child_requisition.height - ltr_y; 

					child_allocation.height = MAX(MIN(child_requisition.height,
									adjusted.height - ltr_y), 0);

					gtk_menu_item_toggle_size_allocate (GTK_MENU_ITEM (child),
					toggle_size);
					gtk_widget_size_allocate (child, &child_allocation);

					ltr_y += child_requisition.height;
				}
			}
		}
	}

	if(priv->show_arrow) {
		LOG("arrow_allocation: %d %d %d %d", arrow_allocation);
		gtk_widget_size_allocate(priv->arrow_button, &arrow_allocation);
		gtk_widget_show_all(priv->arrow_button);
	} else {
		gtk_widget_hide(priv->arrow_button);
	}
}
static void _reset_style			( GtkWidget * widget){
	GtkRcStyle * rc_style;
	gtk_widget_set_style (widget, NULL);
	rc_style = gtk_rc_style_new ();
	gtk_widget_modify_style (widget, rc_style);
	gtk_rc_style_unref (rc_style);
}
static void _remove_child ( GtkWidget * widget, GtkContainer * container){
	gtk_container_remove(container, widget);
}

GtkMenuItem * _get_proxy_for_item( GtkMenuItem * item){
	GtkMenuItem * proxy = NULL;
	GtkWidget * child = gtk_bin_get_child(GTK_BIN(item));
	GtkMenu * submenu;
	GtkWidget * label;
	const gchar * text = gtk_widget_get_name(GTK_WIDGET(item));
	LOG("menuitem type: %s", G_OBJECT_TYPE_NAME(item));
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
	LOG("proxy created, text = %s", text);
	
	label = gtk_label_new_with_mnemonic(text);
/* The image is then lost.*/
	proxy = GTK_MENU_ITEM(gtk_menu_item_new());
	gtk_container_add(GTK_CONTAINER(proxy), label);
	
	submenu = GTK_MENU(gtk_menu_item_get_submenu(item));
	if(submenu){
/*This is buggy. one menu can't be submenu of two menu items.*/
		gtk_menu_item_set_submenu(proxy, GTK_WIDGET(submenu));	
	}
	return proxy;	
}
static void _build_popup_menu 	(GnomenuMenuBar * self){
	GET_OBJECT(self, menu_bar, priv);
	GList * list;
	GList * node;
	gtk_container_foreach(GTK_CONTAINER(priv->popup_menu), (GtkCallback)_remove_child, priv->popup_menu);
	list = g_hash_table_get_values(priv->menu_items);
	for(node = g_list_first(list); node; node = g_list_next(node)){
		MenuItemInfo * info = node->data;
		if(info->overflowed) {
			GtkMenuItem * proxy = _get_proxy_for_item(info->menu_item);
			if(proxy) gtk_menu_shell_append(GTK_MENU_SHELL(priv->popup_menu), GTK_WIDGET(proxy));
		}
	}
	g_list_free(list);
}

static void _s_arrow_button_clicked		( GtkWidget * self,
									  GtkWidget * arrow_button){
	GET_OBJECT(self, menu_bar, priv);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->arrow_button)) &&
     !GTK_WIDGET_VISIBLE (priv->popup_menu)) {
		_build_popup_menu(menu_bar);
		gtk_widget_show_all(GTK_WIDGET(priv->popup_menu));
		gtk_menu_popup(priv->popup_menu, NULL, NULL, 
			NULL, NULL, 0, gtk_get_current_event_time());
		gtk_menu_shell_select_first (GTK_MENU_SHELL (priv->popup_menu), FALSE);
	
	}
}
static void _s_popup_menu_deactivated	( GtkWidget * menubar,
									  GtkWidget * popup_menu){
	GET_OBJECT(menubar, menu_bar, priv);
	gtk_container_foreach(GTK_CONTAINER(priv->popup_menu), (GtkCallback)_remove_child, priv->popup_menu);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->arrow_button), FALSE);
}
/*
vim:ts=4:sw=4
*/
