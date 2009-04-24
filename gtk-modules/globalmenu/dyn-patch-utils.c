#include <gtk/gtk.h>
#include "dyn-patch-utils.h"
extern GQuark __MENUBAR__;
extern GQuark __DIRTY__;
extern GQuark __OLD_SUBMENU__;
extern GQuark __ITEM__;
extern GQuark __IS_LOCAL__;
extern GQuark __TOPLEVEL__;
extern GTimer * timer;
extern GHashTable * notifiers;

static GStaticRecMutex _menubar_mutex = G_STATIC_MUTEX_INIT;

static gulong buffered_changes = 0;
static gboolean _dyn_patch_emit_changed(GtkMenuBar * menubar);
static void dpdm_transverse(GtkWidget * widget, DiscoverMode * mode);
static void _dyn_patch_simple_notify(GtkWidget * widget, GParamSpec * pspec, GtkMenuBar * menubar);
static void _dyn_patch_submenu_notify(GtkWidget * widget, GParamSpec * pspec, GtkMenuBar * menubar);
static void dyn_patch_set_menubar(GtkWidget * widget, GtkMenuBar * menubar);

void dyn_patch_discover_menubars(DiscoverMode mode) {
	g_static_rec_mutex_lock(&_menubar_mutex);
	GList * toplevels = gtk_window_list_toplevels();
	GList * iter;
	for(iter = toplevels; iter; iter = iter->next) {
		GtkWindow * window = iter->data;
		dpdm_transverse(GTK_WIDGET(window), &mode);
	}
	g_list_free(toplevels);
	g_static_rec_mutex_unlock(&_menubar_mutex);
}

void dyn_patch_queue_changed(GtkMenuBar * menubar, GtkWidget * widget) {
	g_static_rec_mutex_lock(&_menubar_mutex);
	guint source_id;
	buffered_changes++;
	/* if their is a pending notifier, do nothing. wait for that notifier
	 * to notifier the changes of the menubar.*/
	if(g_hash_table_lookup(notifiers, menubar)) return;

	source_id = g_timeout_add_full(G_PRIORITY_HIGH_IDLE, 200, (GSourceFunc) _dyn_patch_emit_changed, g_object_ref(menubar), g_object_unref);

	if(source_id) {
	/* to make sure the menubar is alive when it is in the hash table.
	 * This might be a redundancy as we also refer it in the notifier
	 * above. but I don't want to think too much into this.*/
		g_hash_table_insert(notifiers, g_object_ref(menubar), GINT_TO_POINTER(source_id));
	} else {
		/* should never get to here */
	}
	g_static_rec_mutex_unlock(&_menubar_mutex);
}

GtkMenuBar * dyn_patch_get_menubar(GtkWidget * widget) {
	GtkMenuBar * rt = NULL;
	g_static_rec_mutex_lock(&_menubar_mutex);
	if(GTK_IS_MENU_BAR(widget)) rt = GTK_MENU_BAR(widget);
	rt = g_object_get_qdata((GObject*)widget, __MENUBAR__);
	g_static_rec_mutex_unlock(&_menubar_mutex);
	return rt;
}



void dyn_patch_attach_menubar(GtkWindow * window, GtkMenuBar * menubar) {
	g_static_rec_mutex_lock(&_menubar_mutex);
	g_object_set_qdata_full(G_OBJECT(menubar), __TOPLEVEL__, g_object_ref(window), g_object_unref);
	g_object_set_qdata_full(G_OBJECT(window), __MENUBAR__, g_object_ref(menubar), g_object_unref);
	g_signal_emit_by_name(menubar, "dyn-patch-attached", window, NULL);
	g_static_rec_mutex_unlock(&_menubar_mutex);
}
void dyn_patch_detach_menubar(GtkWindow * window, GtkMenuBar * menubar) {
	g_static_rec_mutex_lock(&_menubar_mutex);
	g_signal_emit_by_name(menubar, "dyn-patch-detached", window, NULL);
	g_object_set_qdata(G_OBJECT(window), __MENUBAR__, NULL);
	g_object_set_qdata(G_OBJECT(menubar), __TOPLEVEL__, NULL);
	g_static_rec_mutex_unlock(&_menubar_mutex);
}

GtkWindow * dyn_patch_get_window(GtkMenuBar * menubar) {
	return g_object_get_qdata(G_OBJECT(menubar), __TOPLEVEL__);
}
void dyn_patch_set_menubar_r(GtkWidget * widget, GtkMenuBar * menubar) {
	g_timer_continue(timer);
	g_static_rec_mutex_lock(&_menubar_mutex);
	GtkMenuBar * old = dyn_patch_get_menubar(widget);
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

	if(menubar != NULL) {
		gboolean local = FALSE;
		g_object_get(menubar, "local", &local, NULL);
		if(local) return;
	}
	dyn_patch_set_menubar(widget, menubar);

	if(GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach((GtkContainer*)widget, 
				(GtkCallback)dyn_patch_set_menubar_r, menubar);
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
					(GCallback)_dyn_patch_simple_notify, menubar);
		}
		if(GTK_IS_MENU_ITEM(widget)) {
			g_signal_connect(widget, "notify::submenu", 
					(GCallback)_dyn_patch_submenu_notify, menubar);
			g_signal_connect(widget, "notify::visible", 
					(GCallback)_dyn_patch_simple_notify, menubar);
			g_signal_connect(widget, "notify::sensitive", 
					(GCallback)_dyn_patch_simple_notify, menubar);
		}
		if(GTK_IS_CHECK_MENU_ITEM(widget)) {
			g_signal_connect(widget, "notify::active", 
					(GCallback)_dyn_patch_simple_notify, menubar);
			g_signal_connect(widget, "notify::inconsistent", 
					(GCallback)_dyn_patch_simple_notify, menubar);
			g_signal_connect(widget, "notify::draw-as-radio", 
					(GCallback)_dyn_patch_simple_notify, menubar);
		}
	}
	g_timer_stop(timer);
	g_static_rec_mutex_unlock(&_menubar_mutex);
}

static void dpdm_transverse(GtkWidget * widget, DiscoverMode * mode) {
	if(GTK_IS_MENU_BAR(widget)) {
		GtkWindow * toplevel = GTK_WINDOW(gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW));
		switch(*mode) {
			case DISCOVER_MODE_INIT:
				dyn_patch_set_menubar_r(widget, GTK_MENU_BAR(widget));
				dyn_patch_queue_changed(GTK_MENU_BAR(widget), widget);

				if(toplevel) {
					dyn_patch_set_menubar(GTK_WIDGET(toplevel), GTK_MENU_BAR(widget));
					dyn_patch_attach_menubar(toplevel, GTK_MENU_BAR(widget));
				}
			break;
			case DISCOVER_MODE_UNINIT_VFUNCS:
				dyn_patch_set_menubar_r(widget, NULL);
			break;
			case DISCOVER_MODE_UNINIT_FINAL:
				if(toplevel) {
					dyn_patch_detach_menubar(toplevel, GTK_MENU_BAR(widget));
				}
				dyn_patch_set_menubar(GTK_WIDGET(toplevel), NULL);
			break;
		}
		return;
	} 
	/* else */
	if(GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(GTK_CONTAINER(widget), 
				(GtkCallback) dpdm_transverse, 
				mode);
	}
}

static void dyn_patch_set_menubar(GtkWidget * widget, GtkMenuBar * menubar) {
	if(menubar != NULL) {
		g_object_set_qdata_full((GObject*) widget, __MENUBAR__, g_object_ref(menubar), g_object_unref);
	} else {
		g_object_set_qdata((GObject*) widget, __MENUBAR__, NULL);
	}
}

static gboolean _dyn_patch_emit_changed(GtkMenuBar * menubar) {
	GDK_THREADS_ENTER();
	g_static_rec_mutex_lock(&_menubar_mutex);
	g_debug("Changed: %p", menubar);
	g_object_set_qdata((GObject*)menubar, __DIRTY__, NULL);
	g_signal_emit_by_name(menubar, "dyn-patch-changed", NULL);
	g_debug("_dyn_patch_set_menu_bar_r consumption: %lf, buffered_changes = %ld ", g_timer_elapsed(timer, NULL), buffered_changes);
	buffered_changes = 0;

	g_timer_reset(timer);
	g_timer_stop(timer);
	/* The source and the reference to the menubar are removed by the hash
	 * table*/
	/* The reference on the menubar by this notifier is removed after
	 * the source is removed by the hash table.*/
	g_hash_table_remove(notifiers, menubar);
	g_static_rec_mutex_unlock(&_menubar_mutex);
	GDK_THREADS_LEAVE();
	/*already removed, do'nt want glib get into troubles for removing the
	 * source again.*/
	return TRUE;
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


