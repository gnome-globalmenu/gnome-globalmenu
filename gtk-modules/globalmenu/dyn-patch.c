#include <gtk/gtk.h>
#include "dyn-patch.h"
#include "dyn-patch-private.h"

extern void dyn_patch_widget_patcher();
extern void	dyn_patch_menu_shell_patcher();
extern void	dyn_patch_menu_bar_patcher();
extern void dyn_patch_widget_unpatcher();
extern void	dyn_patch_menu_shell_unpatcher();
extern void	dyn_patch_menu_bar_unpatcher();

static void dyn_patch_type_r(GType type, DynPatcherFunc patcher);
/*
 * _USE_CLOSURES doesn't help improving the performance.
 * */

static GQuark __MENUBAR__ = 0;
static GQuark __DIRTY__ = 0;
static GQuark __OLD_SUBMENU__ = 0;
static GQuark __ITEM__  =  0;
static GQuark __LABEL_NOTIFY_CLOSURE__ = 0;
static GQuark __SUBMENU_NOTIFY_CLOSURE__ = 0;
static gulong SIGNAL_NOTIFY = 0;
static GQuark DETAIL_SUBMENU = 0;
static GQuark DETAIL_LABEL = 0;

static GTimer * timer = NULL;
static gulong buffered_changes = 0;
static GHashTable * old_vfuncs = NULL;
static GHashTable * classes = NULL;
static GHashTable * notifiers = NULL;

void dyn_patch_init () {
	
	GDK_THREADS_ENTER();
	__MENUBAR__ = g_quark_from_string("__menubar__");
	__DIRTY__ = g_quark_from_string("__dirty__");
	__OLD_SUBMENU__ = g_quark_from_string("__old_submenu__");
	__ITEM__ = g_quark_from_string("__item__");

	SIGNAL_NOTIFY = g_signal_lookup("notify", G_TYPE_OBJECT);

	old_vfuncs = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	classes = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_type_class_unref);

	notifiers = g_hash_table_new_full(g_direct_hash, g_direct_equal, g_object_unref, g_source_remove);

	dyn_patch_type_r(GTK_TYPE_WIDGET, dyn_patch_widget_patcher);
	dyn_patch_type_r(GTK_TYPE_MENU_SHELL, dyn_patch_menu_shell_patcher);
	dyn_patch_type_r(GTK_TYPE_MENU_BAR, dyn_patch_menu_bar_patcher);

	timer = g_timer_new();
	g_timer_stop(timer);
	
	GDK_THREADS_LEAVE();
}

void dyn_patch_uninit() {
	GList * toplevels = gtk_window_list_toplevels();
	GList * node;
	for(node = toplevels; node; node = node->next) {
		GtkWidget * toplevel = node->data;
		dyn_patch_set_menubar_r(toplevel, NULL);
	}
	g_list_free(toplevels);
	g_timer_destroy(timer);
	dyn_patch_type_r(GTK_TYPE_MENU_BAR, dyn_patch_menu_bar_unpatcher);
	dyn_patch_type_r(GTK_TYPE_MENU_SHELL, dyn_patch_menu_shell_unpatcher);
	dyn_patch_type_r(GTK_TYPE_WIDGET, dyn_patch_widget_unpatcher);

	g_hash_table_unref(notifiers);
	g_hash_table_unref(old_vfuncs);
	g_hash_table_unref(classes);

}

void dyn_patch_save_vfunc(const char * type, const char * name, gpointer vfunc) {
	char * long_name = g_strdup_printf("%s_%s", type, name);
	g_hash_table_insert(old_vfuncs, long_name, vfunc);
}

gpointer dyn_patch_hold_type(GType type) {
	gpointer klass = g_type_class_ref(type);
	g_hash_table_insert(classes, type, klass);
	return klass;
}
void dyn_patch_release_type(GType type) {
	g_hash_table_remove(classes, type);
}

gpointer dyn_patch_load_vfunc(const char * type, const char * name) {
	char * long_name = g_strdup_printf("%s_%s", type, name);
	gpointer rt = g_hash_table_lookup(old_vfuncs, long_name);
	g_free(long_name);
	return rt;
}

static void dyn_patch_type_r(GType type, DynPatcherFunc patcher) {
	GType * children = g_type_children(type, NULL);
	int i;
	patcher(type);
	for(i = 0; children[i]; i++) {
		dyn_patch_type_r(children[i], patcher);
	}
	g_free(children);
}


static gboolean _dyn_patch_emit_changed(GtkMenuBar * menubar) {
	GDK_THREADS_ENTER();
	g_debug("Changed: %p", menubar);
	g_object_set_qdata((GObject*)menubar, __DIRTY__, NULL);
	g_signal_emit_by_name(menubar, "changed", 0, NULL);
	g_debug("_dyn_patch_set_menu_bar_r consumption: %lf, buffered_changes = %ld ", g_timer_elapsed(timer, NULL), buffered_changes);
	buffered_changes = 0;

	g_timer_reset(timer);
	g_timer_stop(timer);
	/* The source and the reference to the menubar are removed by the hash
	 * table*/
	/* The reference on the menubar by this notifier is removed after
	 * the source is removed by the hash table.*/
	g_hash_table_remove(notifiers, menubar);
	GDK_THREADS_LEAVE();
	/*already removed, do'nt want glib get into troubles for removing the
	 * source again.*/
	return TRUE;
}
void dyn_patch_queue_changed(GtkMenuBar * menubar, GtkWidget * widget) {
	guint source_id;
	buffered_changes++;
	/* if their is a pending notifier, do nothing. wait for that notifier
	 * to notifier the changes of the menubar.*/
	if(g_hash_table_lookup(notifiers, menubar)) return;

	source_id = g_idle_add_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc) _dyn_patch_emit_changed, g_object_ref(menubar), g_object_unref);

	if(source_id) {
	/* to make sure the menubar is alive when it is in the hash table.
	 * This might be a redundancy as we also refer it in the notifier
	 * above. but I don't want to think too much into this.*/
		g_hash_table_insert(notifiers, g_object_ref(menubar), source_id);
	} else {
		/* should never get to here */
	}
}

GtkMenuBar * dyn_patch_get_menubar(GtkWidget * widget) {
	if(GTK_IS_MENU_BAR(widget)) return widget;
	return g_object_get_qdata((GObject*)widget, __MENUBAR__);
}
void dyn_patch_set_menubar(GtkWidget * widget, GtkMenuBar * menubar) {
	if(menubar != NULL) {
		g_object_set_qdata_full((GObject*) widget, __MENUBAR__, g_object_ref(menubar), g_object_unref);
	} else {
		g_object_set_qdata((GObject*) widget, __MENUBAR__, NULL);
	}
}
static void _dyn_patch_simple_notify(GtkWidget * widget, GParamSpec * pspec, GtkMenuBar * menubar) {
	g_debug("simple notify: %s on widget %p (%s) of bar %p", pspec->name,
			widget, gtk_widget_get_name(widget), menubar);
	dyn_patch_queue_changed(menubar, widget);
}
static void _dyn_patch_submenu_notify(GtkWidget * widget, GParamSpec * pspec, GtkMenuBar * menubar) {
	GtkWidget * old_submenu = g_object_get_qdata((GObject*) widget, __OLD_SUBMENU__);
	GtkWidget * submenu = gtk_menu_item_get_submenu((GtkMenuItem*)widget);
	g_debug("submenu on Widget %p changed from %p to %p", widget, old_submenu, submenu);
	if(submenu != old_submenu) {
		if(old_submenu) {
			dyn_patch_set_menubar_r(old_submenu, NULL);
		}
		if(submenu) {
			dyn_patch_set_menubar_r(submenu, menubar);
			g_object_set_qdata_full((GObject*) widget, __OLD_SUBMENU__, g_object_ref(submenu), g_object_unref); 
		} else {
			g_object_set_qdata((GObject*) widget, __OLD_SUBMENU__, NULL); 
		}
		/* although the property already hold a reference, 
		 * we want to ensure old_submenu above is still alive
		 * */
		dyn_patch_queue_changed(menubar, widget);
	}
}

void dyn_patch_set_menubar_r(GtkWidget * widget, GtkMenuBar * menubar) {
	g_timer_continue(timer);
	GtkWidget * old = (GtkWidget*) dyn_patch_get_menubar(widget);
	if(old && old != menubar) {
		g_debug("Detaching hooks on Widget %p of menubar %p", widget, old);
		if(GTK_IS_LABEL(widget))
			g_signal_handlers_disconnect_by_func(widget, 
					_dyn_patch_simple_notify, 
					old);
		if(GTK_IS_MENU_ITEM(widget)) {
			g_signal_handlers_disconnect_by_func(widget, 
					_dyn_patch_submenu_notify, 
					old);
			g_signal_handlers_disconnect_by_func(widget, 
					_dyn_patch_simple_notify, 
					old);
		}
		if(GTK_IS_CHECK_MENU_ITEM(widget)) {
			g_signal_handlers_disconnect_by_func(widget, 
					_dyn_patch_simple_notify, 
					old);
		}
	}
	g_timer_stop(timer);
	dyn_patch_set_menubar(widget, menubar);

	if(GTK_IS_CONTAINER(widget)) {
		GList * children = gtk_container_get_children((GtkContainer*)widget);
		GList * node;
		for(node = children; node; node = node->next) {
			dyn_patch_set_menubar_r(node->data, menubar);
		}
	}
	if(GTK_IS_MENU_ITEM(widget)) {
		GtkWidget * submenu = gtk_menu_item_get_submenu((GtkMenuItem*)widget);
		if(submenu) {
			g_object_set_qdata_full((GObject*) submenu, __ITEM__, g_object_ref(widget), g_object_unref);
			dyn_patch_set_menubar_r(submenu, menubar);
		}
	}
	g_timer_continue(timer);
	if(menubar && menubar != old) {
		g_debug("Registering hooks on %p of %p", widget, menubar);
		if(GTK_IS_LABEL(widget)) {
			g_signal_connect(widget, "notify::label", 
					_dyn_patch_simple_notify, menubar);
		}
		if(GTK_IS_MENU_ITEM(widget)) {
			g_signal_connect(widget, "notify::submenu", 
					_dyn_patch_submenu_notify, menubar);
			g_signal_connect(widget, "notify::visible", 
					_dyn_patch_simple_notify, menubar);
			g_signal_connect(widget, "notify::sensitive", 
					_dyn_patch_simple_notify, menubar);
		}
		if(GTK_IS_CHECK_MENU_ITEM(widget)) {
			g_signal_connect(widget, "notify::active", 
					_dyn_patch_simple_notify, menubar);
			g_signal_connect(widget, "notify::inconsistent", 
					_dyn_patch_simple_notify, menubar);
			g_signal_connect(widget, "notify::draw-as-radio", 
					_dyn_patch_simple_notify, menubar);
		}
	}
	g_timer_stop(timer);
}

