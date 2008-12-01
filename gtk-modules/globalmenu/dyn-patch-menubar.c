#include <gtk/gtk.h>

#include "dyn-patch-helper.h"

#define PROP_LOCAL 9999

static GtkMenuShellClass * _gtk_menu_bar_parent_class = NULL;
guint SIGNAL_CHANGED = 0;


DEFINE_FUNC(void, gtk_menu_bar, map, (GtkWidget * widget)) {
  if(!g_object_get_data(widget, "is-local")) {

	  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
	  (* GTK_WIDGET_CLASS (_gtk_menu_bar_parent_class)->map) (widget);

	  if (!GTK_WIDGET_NO_WINDOW (widget))
		gdk_window_hide (widget->window);
	  return;
  }
  _old_gtk_menu_bar_map(widget);
}

DEFINE_FUNC(void, gtk_menu_bar, get_property, (GObject * object, guint prop_id, const GValue * value, GParamSpec *pspec)) {
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  switch (prop_id)
    {
	case PROP_LOCAL:
	  g_value_set_boolean (value, g_object_get_data(object, "is-local"));
	  break;
    default:
		_old_gtk_menu_bar_get_property(object, prop_id, value, pspec);
      break;
    }
}

DEFINE_FUNC(void, gtk_menu_bar, set_property, 
	(GObject * object, guint prop_id, const GValue * value, GParamSpec *pspec)) {
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  switch (prop_id)
    {
	case PROP_LOCAL:
	  g_object_set_data(object, "is-local", (gpointer) g_value_get_boolean (value));
	  if(GTK_WIDGET_MAPPED (menubar))
		  _gtk_menu_bar_map (menubar);
	  gtk_widget_queue_resize(menubar);
	  break;
    default:
		_old_gtk_menu_bar_set_property(object, prop_id, value, pspec);
      break;
    }
}

DEFINE_FUNC(void, gtk_menu_bar, size_request, (GtkWidget * widget, GtkRequisition * requisition)) {
  if(!g_object_get_data(widget, "is-local")) {
	  requisition->width = 0;
	  requisition->height = 0;
	  return ;
  }
  _old_gtk_menu_bar_size_request(widget, requisition);
}

DEFINE_FUNC(gboolean, gtk_menu_bar, can_activate_accel, (GtkWidget * widget, guint signal_id)) {
	return GTK_WIDGET_IS_SENSITIVE (widget);
}

void dyn_patch_menu_bar() {
	GObjectClass * klass = g_type_class_ref(GTK_TYPE_MENU_BAR);
	GtkWidgetClass * widget_klass =  (GtkWidgetClass*) klass;
	_gtk_menu_bar_parent_class = g_type_class_peek_parent(klass);

	g_object_class_install_property (klass,
				   PROP_LOCAL,
				   g_param_spec_boolean ("local",
 						      ("Local Menu or Global Menu"),
 						      ("Whether the menu is a local one"),
 						      FALSE,
 						      G_PARAM_READWRITE));

	SIGNAL_CHANGED =
		g_signal_new (("changed"),
		  G_OBJECT_CLASS_TYPE (klass),
		  G_SIGNAL_RUN_FIRST,
		  0, 
		  NULL, NULL,
		  gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

	OVERRIDE(klass, gtk_menu_bar, get_property);
	OVERRIDE(klass, gtk_menu_bar, set_property);
	OVERRIDE(widget_klass, gtk_menu_bar, map);
	OVERRIDE(widget_klass, gtk_menu_bar, can_activate_accel);
	OVERRIDE(widget_klass, gtk_menu_bar, size_request);
}
