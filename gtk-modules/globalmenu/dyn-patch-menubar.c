#include <gtk/gtk.h>

#include "dyn-patch-vfunc.h"
#include "dyn-patch-utils.h"

#define PROP_LOCAL 9999
extern GQuark __IS_LOCAL__;

static GtkMenuShellClass * _gtk_menu_bar_parent_class = NULL;
guint SIGNAL_CHANGED = 0;
guint SIGNAL_ATTACHED = 0;
guint SIGNAL_DETACHED = 0;


DEFINE_FUNC(void, gtk_menu_bar, hierarchy_changed, (GtkWidget * widget, GtkWidget * old_toplevel)) {
	VFUNC_TYPE(gtk_menu_bar, hierarchy_changed) vfunc = CHAINUP(gtk_menu_bar, hierarchy_changed);
	if(vfunc) vfunc(widget, old_toplevel);

	GtkWindow * old = dyn_patch_get_window(GTK_MENU_BAR(widget));
	GtkWindow * toplevel = GTK_WINDOW(gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW));
	g_debug("widget hierarchy changed old = %p, toplevel=%p(%s)", old, toplevel, toplevel?gtk_widget_get_name(GTK_WIDGET(toplevel)):NULL);
	if(old != toplevel) {
		if(old) {
			dyn_patch_detach_menubar(old, GTK_MENU_BAR(widget));
		}
		if(toplevel) {
			dyn_patch_attach_menubar(toplevel, GTK_MENU_BAR(widget));
		}
	}
}

DEFINE_FUNC(void, gtk_menu_bar, map, (GtkWidget * widget)) {
	gboolean local = TRUE;
	g_object_get(widget, "local", &local, NULL);
  if(!local) {

	  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
	  (* GTK_WIDGET_CLASS (_gtk_menu_bar_parent_class)->map) (widget);

	  if (!GTK_WIDGET_NO_WINDOW (widget))
		gdk_window_hide (widget->window);
	  return;
  }
  VFUNC_TYPE(gtk_menu_bar, map) vfunc = CHAINUP(gtk_menu_bar, map);
	if(vfunc) vfunc(widget);
}

DEFINE_FUNC(void, gtk_menu_bar, get_property, (GObject * object, guint prop_id, GValue * value, GParamSpec *pspec)) {
  switch (prop_id)
    {
	case PROP_LOCAL:
		{
		gint val = GPOINTER_TO_INT(g_object_get_qdata(object, __IS_LOCAL__));
		gboolean prop_value = TRUE;
		if(val == 0) prop_value = TRUE; /*default to true*/
		if(val == 100) prop_value = TRUE;
		if(val == -100) prop_value = FALSE;
	  g_value_set_boolean (value, prop_value);
		}
	  break;
    default:
	  {
	  VFUNC_TYPE(gtk_menu_bar, get_property) vfunc = CHAINUP(gtk_menu_bar, get_property);
	  if(vfunc) vfunc(object, prop_id, value, pspec);
	  }
      break;
    }
}

DEFINE_FUNC(void, gtk_menu_bar, set_property, 
	(GObject * object, guint prop_id, const GValue * value, GParamSpec *pspec)) {
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  gboolean is_local;
  switch (prop_id)
    {
	case PROP_LOCAL:
		is_local = g_value_get_boolean(value);
		if(is_local) {
		  g_object_set_qdata(object, __IS_LOCAL__, GINT_TO_POINTER(100));
		} else {
		  g_object_set_qdata(object, __IS_LOCAL__, GINT_TO_POINTER(-100));
		}
	  if(GTK_WIDGET_MAPPED (menubar))
		  _gtk_menu_bar_map (GTK_WIDGET(menubar));
	  gtk_widget_queue_resize(GTK_WIDGET(menubar));
	  break;
    default:
	  {
	  VFUNC_TYPE(gtk_menu_bar, set_property) vfunc = CHAINUP(gtk_menu_bar, set_property);
	  if(vfunc) vfunc(object, prop_id, value, pspec);
	  }
      break;
    }
}

DEFINE_FUNC(void, gtk_menu_bar, size_request, (GtkWidget * widget, GtkRequisition * requisition)) {
	VFUNC_TYPE(gtk_menu_bar, size_request) vfunc = CHAINUP(gtk_menu_bar, size_request);
	if(vfunc) vfunc(widget, requisition);
	gboolean local = TRUE;
	g_object_get(widget, "local", &local, NULL);
	if(!local) {
	  requisition->width = 0;
	  requisition->height = 0;
	}
}

DEFINE_FUNC(gboolean, gtk_menu_bar, can_activate_accel, (GtkWidget * widget, guint signal_id)) {
	return GTK_WIDGET_IS_SENSITIVE (widget);
}

void dyn_patch_menu_bar_patcher (GType menu_bar_type) {
	GObjectClass * klass = dyn_patch_hold_type(menu_bar_type);
	GtkWidgetClass * widget_klass =  (GtkWidgetClass*) klass;

	if(menu_bar_type == GTK_TYPE_MENU_BAR) {
		_gtk_menu_bar_parent_class = g_type_class_peek_parent(klass);

		OVERRIDE_SAVE(klass, gtk_menu_bar, get_property);
		OVERRIDE_SAVE(klass, gtk_menu_bar, set_property);
		OVERRIDE_SAVE(widget_klass, gtk_menu_bar, map);
		OVERRIDE_SAVE(widget_klass, gtk_menu_bar, can_activate_accel);
		OVERRIDE_SAVE(widget_klass, gtk_menu_bar, size_request);
		OVERRIDE_SAVE(widget_klass, gtk_menu_bar, hierarchy_changed);
		
		if(g_object_class_find_property (klass, "local") == NULL) {
			g_object_class_install_property (klass,
					   PROP_LOCAL,
					   g_param_spec_boolean ("local",
								  ("Local Menu or Global Menu"),
								  ("Whether the menu is a local one"),
								  TRUE,
								  G_PARAM_READWRITE));
		}

		SIGNAL_CHANGED = g_signal_lookup("dyn-patch-changed", G_OBJECT_CLASS_TYPE (klass));
		if (SIGNAL_CHANGED == 0) {
			SIGNAL_CHANGED =
				g_signal_new (("dyn-patch-changed"),
				  G_OBJECT_CLASS_TYPE (klass),
				  G_SIGNAL_RUN_FIRST,
				  0, 
				  NULL, NULL,
				  gtk_marshal_VOID__VOID,
				  G_TYPE_NONE, 0);
		}
		SIGNAL_ATTACHED = g_signal_lookup("dyn-patch-attached", G_OBJECT_CLASS_TYPE (klass));
		if (SIGNAL_ATTACHED == 0) {
			SIGNAL_ATTACHED =
				g_signal_new (("dyn-patch-attached"),
				  G_OBJECT_CLASS_TYPE (klass),
				  G_SIGNAL_RUN_FIRST,
				  0, 
				  NULL, NULL,
				  gtk_marshal_VOID__OBJECT,
				  G_TYPE_NONE, 1, GTK_TYPE_WINDOW);
		}
		SIGNAL_DETACHED = g_signal_lookup("dyn-patch-detached", G_OBJECT_CLASS_TYPE (klass));
		if (SIGNAL_DETACHED == 0) {
			SIGNAL_DETACHED =
				g_signal_new (("dyn-patch-detached"),
				  G_OBJECT_CLASS_TYPE (klass),
				  G_SIGNAL_RUN_FIRST,
				  0, 
				  NULL, NULL,
				  gtk_marshal_VOID__OBJECT,
				  G_TYPE_NONE, 1, GTK_TYPE_WINDOW);
		}
	} else {	

		OVERRIDE(klass, gtk_menu_bar, get_property);
		OVERRIDE(klass, gtk_menu_bar, set_property);
		OVERRIDE(widget_klass, gtk_menu_bar, map);
		OVERRIDE(widget_klass, gtk_menu_bar, can_activate_accel);
		OVERRIDE(widget_klass, gtk_menu_bar, size_request);
		OVERRIDE(widget_klass, gtk_menu_bar, hierarchy_changed);
	}
}
void dyn_patch_menu_bar_unpatcher(GType menu_bar_type) {
	GObjectClass * klass = g_type_class_ref(menu_bar_type);
	if(!klass) return;
	GtkWidgetClass * widget_klass =  (GtkWidgetClass*) klass;

	RESTORE(klass, gtk_menu_bar, get_property);
	RESTORE(klass, gtk_menu_bar, set_property);
	RESTORE(widget_klass, gtk_menu_bar, map);
	RESTORE(widget_klass, gtk_menu_bar, can_activate_accel);
	RESTORE(widget_klass, gtk_menu_bar, size_request);
	RESTORE(widget_klass, gtk_menu_bar, hierarchy_changed);
	
	g_type_class_unref(klass);
	dyn_patch_release_type(menu_bar_type);
}
