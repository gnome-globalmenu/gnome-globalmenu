#include <gtk/gtk.h>
#define PROP_LOCAL 9999
typedef void (*SetPropertyFunc)(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspes);
typedef void (*GetPropertyFunc)(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspes);
typedef void (*MapFunc) (GtkWidget    *widget);
typedef void (*SizeRequestFunc) (GtkWidget      *widget, GtkRequisition *requisition);
typedef void (*RealInsertFunc) (GtkMenuShell *menu_shell, GtkWidget    *child, gint          position);

static SetPropertyFunc old_set = NULL;
static GetPropertyFunc old_get = NULL;
static MapFunc old_map = NULL;
static RealInsertFunc old_real_insert = NULL;
static SizeRequestFunc old_size_request = NULL;

static void _set_property      (GObject             *object,
					    guint                prop_id,
					    const GValue        *value,
					    GParamSpec          *pspec);
static void _get_property      (GObject             *object,
					    guint                prop_id,
					    GValue              *value,
					    GParamSpec          *pspec);
static void
_map (GtkWidget    *widget);
static void _size_request (GtkWidget      *widget, GtkRequisition *requisition);
static void
_real_insert (GtkMenuShell *menu_shell,
			    GtkWidget    *child,
			    gint          position);


static guint SignalIDInsert = 0;
static guint SignalIDLabelSet = 0;

void _patch_menu_bar() {
	GObjectClass * klass =  g_type_class_ref(GTK_TYPE_MENU_BAR);
	GtkWidgetClass * widget_klass =  (GtkWidgetClass*) klass;
	g_object_class_install_property (klass,
				   PROP_LOCAL,
				   g_param_spec_boolean ("local",
 						      ("Local Menu or Global Menu"),
 						      ("Whether the menu is a local one"),
 						      FALSE,
 						      G_PARAM_READWRITE));
	old_get = klass->get_property;	
	old_set = klass->set_property;	
	old_map = widget_klass->map;
	old_size_request = widget_klass->size_request;
	klass->set_property = _set_property;
	klass->get_property = _get_property;
	widget_klass->map = _map;
	widget_klass->size_request = _size_request;
}
void _patch_menu_shell() {
	GObjectClass * klass =  g_type_class_ref(GTK_TYPE_MENU_SHELL);
	GtkMenuShellClass * menu_shell_klass = (GtkMenuShellClass*)klass;
	SignalIDInsert =
		g_signal_new (("insert"),
		  G_OBJECT_CLASS_TYPE (klass),
		  G_SIGNAL_RUN_FIRST,
		  0, /*NOTE: if apply the patch to GTK, should use klass->insert*/
		  NULL, NULL,
		  gtk_marshal_VOID__POINTER_INT,
		  G_TYPE_NONE, 2, GTK_TYPE_WIDGET, G_TYPE_INT);
	old_real_insert = menu_shell_klass->insert;
	menu_shell_klass->insert = _real_insert;
}
static gboolean menu_item_parent_set_hook(GSignalInvocationHint * hint,
			int value_count, GValue values[], gpointer data) {
	GObject * self = g_value_get_object(&values[0]);
	if(!GTK_IS_WIDGET(self)) return TRUE;
	GtkWidget * label = NULL;
	GQueue queue = G_QUEUE_INIT;
	g_queue_push_tail(&queue, self);
	while(!g_queue_is_empty(&queue)){
		GtkWidget* head = g_queue_pop_head(&queue);
		if(GTK_IS_CONTAINER(head)) {
			GList * list = gtk_container_get_children(head);
			GList * node;
			for(node = list; node; node = node->next) {
				g_queue_push_tail(&queue, node->data);
			}
		}
		if(GTK_IS_LABEL(head)) {
			g_queue_clear(&queue);
			label = head;
			break;
		}
	}
	GObject * parent = gtk_widget_get_parent(self);
	for(; parent; parent = gtk_widget_get_parent(parent)) {
		if(GTK_IS_MENU_ITEM(parent)) {
			if(label != g_object_get_data(parent, "label")) {
				g_object_set_data(parent, "label", label);
				g_signal_emit(parent, SignalIDLabelSet, 0, label);
			}
			break;
		}
	}
	return TRUE;
}
void _patch_menu_item() {
	GObjectClass * klass =  g_type_class_ref(GTK_TYPE_MENU_ITEM);
	GtkMenuItemClass * menu_tem_klass = (GtkMenuItemClass*)klass;
	SignalIDLabelSet =
		g_signal_new (("label-set"),
		  G_OBJECT_CLASS_TYPE (klass),
		  G_SIGNAL_RUN_FIRST,
		  0, 
		  NULL, NULL,
		  gtk_marshal_VOID__POINTER,
		  G_TYPE_NONE, 1, G_TYPE_POINTER);
	g_signal_add_emission_hook(
			g_signal_lookup("parent-set", GTK_TYPE_WIDGET),
			0,
			menu_item_parent_set_hook, NULL, NULL);
}
static void
_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  switch (prop_id)
    {
	case PROP_LOCAL:
	  g_object_set_data(object, "is-local", (gpointer) g_value_get_boolean (value));
	  if(GTK_WIDGET_MAPPED (menubar))
		  _map (menubar);
	  gtk_widget_queue_resize(menubar);
	  break;
    default:
		old_set(object, prop_id, value, pspec);
      break;
    }
}

static void
_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
  GtkMenuBar *menubar = GTK_MENU_BAR (object);
  
  switch (prop_id)
    {
	case PROP_LOCAL:
	  g_value_set_boolean (value, g_object_get_data(object, "is-local"));
	  break;
    default:
		old_get(object, prop_id, value, pspec);
      break;
    }
}
static void
_map (GtkWidget    *widget) {
  
  if(!g_object_get_data(widget, "is-local")) {
	  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
	  if (!GTK_WIDGET_NO_WINDOW (widget))
		gdk_window_hide (widget->window);
	  return;
  }
  old_map(widget);
}
static void
_size_request (GtkWidget      *widget,
			   GtkRequisition *requisition)
{
  if(!g_object_get_data(widget, "is-local")) {
	  requisition->width = 0;
	  requisition->height = 0;
	  return ;
  }
  old_size_request(widget, requisition);
}
static void
_real_insert (GtkMenuShell *menu_shell,
			    GtkWidget    *child,
			    gint          position)
{
	old_real_insert(menu_shell, child, position);
	g_signal_emit(menu_shell, SignalIDInsert, 0, child, position);
}
