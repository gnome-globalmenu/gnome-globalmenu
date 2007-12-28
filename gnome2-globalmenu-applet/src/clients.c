#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include <config.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

#include "typedefs.h"
#include "misc.h"

#include "clients.h"

static ClientEntry * clients_entry_new(gchar * title, Application * App){
	ClientEntry * rt = g_new0(ClientEntry, 1);
	rt->Title = g_strdup(title);
	rt->w = -1;
	rt->h = -1;
	rt->x = 0;
	rt->y = 0;
	rt->App = App;
	return rt;
}
ClientEntry * clients_add_local(char * title, GtkWidget * menubar,
	Application * App){
	ClientEntry * rt = clients_entry_new(title, App);
	rt->Type = MENUBAR_LOCAL;
	rt->Widget = menubar;
	gtk_notebook_append_page(App->Notebook, rt->Widget, NULL);
	g_hash_table_insert(App->Clients, NULL, rt);
	return rt;
}

ClientEntry * clients_add_dummy(char * title,
	Application * App){
	GtkWidget * label = gtk_label_new(title);
	ClientEntry * rt = clients_add_local(title, label, App);
	rt->IsDummy = TRUE;
	return rt;
}
void clients_discover_all(Application * App){
	g_print("then add all existed menubars\n");
	{
		GList* windows = wnck_screen_get_windows_stacked(App->Screen);
		GList* node = windows;
		while (node != NULL) {
			WnckWindow* wnckwin = (WnckWindow*) node->data;
			if (wnck_window_is_stealable_menubar(wnckwin))
			  clients_add_remote_by_stealing(wnckwin, App);
			node = node->next;
		}
	}
}

static void container_destroy_cb(GtkContainer * container, Application * App){
	ClientEntry * target = clients_find_by_container(container, App);
//	target->IsDead = TRUE;	 just do nothing.
}

static void container_size_allocate_cb(GtkContainer * container, GtkAllocation * allocation, Application * App){
	g_print("container resized!\n");
//	target->IsDead = TRUE;	 just do nothing.
}
ClientEntry * clients_add_remote_by_stealing(WnckWindow * remoteMenuBarWindow,
	Application * App){
	gint x, y; /*drop these values after obtained*/
	WnckApplication * remoteApp;
	remoteApp = wnck_window_get_application(remoteMenuBarWindow);
	gchar * title = NULL; 
	GdkWindow * foreign_window = NULL;
	
	ClientEntry * rt = NULL;
	XWindowID wid = 0;

	title = wnck_application_get_name(remoteApp);
	rt = clients_entry_new(title, App);
	rt->Icon = wnck_application_get_icon(remoteApp);
	g_object_ref(G_OBJECT(rt->Icon));
	rt->Type = MENUBAR_REMOTE;
	rt->IsDead = FALSE;
	rt->Container = GTK_CONTAINER(gtk_fixed_new()); //GTK_SOCKET(gtk_socket_new());
	gtk_fixed_set_has_window(GTK_FIXED(rt->Container), TRUE);

	rt->MasterWID = menubar_window_get_master(remoteMenuBarWindow); /*

	Use X's Window ID because we are uncertain whether wnck's WnckWindow* can be a handle for windows */
	g_signal_connect(GTK_WIDGET(rt->Container),
					"destroy", G_CALLBACK(container_destroy_cb), App);
	g_signal_connect(GTK_WIDGET(rt->Container),
					"size-allocate", G_CALLBACK(container_size_allocate_cb), App);	
	wnck_window_get_geometry(remoteMenuBarWindow, 
		&x, &y, &rt->w, &rt->h);
	
	gtk_notebook_append_page(App->Notebook, GTK_WIDGET(rt->Container), NULL);
	gtk_widget_realize(GTK_WIDGET(rt->Container));

	wid = wnck_window_get_xid(remoteMenuBarWindow);
	rt->Window = gdk_window_foreign_new(wid);
	//gtk_socket_steal(rt->Container, wid);
	gdk_window_reparent(rt->Window, GTK_WIDGET(rt->Container)->window, 0, 0);
	gtk_widget_show_all(GTK_WIDGET(rt->Container));

	g_hash_table_insert(App->Clients, (gpointer) wid, rt);
	return rt;
}

static gboolean clients_find_by_container_cb(XWindowID menubar_xwid,
				ClientEntry * entry, GtkContainer * container){
	if(entry->Type != MENUBAR_REMOTE) return FALSE;
	if(entry->Container == container){
		return TRUE;
	}
	return FALSE;
}
#define clients_remove_by_container_cb clients_find_by_container_cb
static gboolean clients_find_by_master_cb(XWindowID menubar_xwid,
				ClientEntry * entry, XWindowID master_xwid){
	if(entry->Type != MENUBAR_REMOTE) return FALSE;
	if(entry->MasterWID == master_xwid){
		return TRUE;
	}
	return FALSE;
}
static gboolean clients_find_dummy_cb(gpointer dontcare, 
				ClientEntry *entry, gpointer dontcare2){
	if(entry->Type == MENUBAR_LOCAL && entry->IsDummy == TRUE) return TRUE;
	return FALSE;
}

ClientEntry * clients_find_by_container(GtkContainer * container, Application * App){
	return g_hash_table_find(App->Clients, 
		(GHRFunc)clients_find_by_container_cb, container);
}

void clients_remove_by_container(GtkContainer * container, Application * App){
	g_hash_table_foreach_remove(App->Clients, 
		(GHRFunc)clients_remove_by_container_cb, container);
}

ClientEntry * clients_find_by_master(XWindowID master, Application * App){
	return g_hash_table_find(App->Clients, 
		(GHRFunc)clients_find_by_master_cb, (gpointer)master);
}

void clients_remove_by_master(XWindowID master, Application * App){
	g_hash_table_foreach_remove(App->Clients, 
		(GHRFunc)clients_find_by_master_cb, master);
}

ClientEntry * clients_find_dummy(Application * App){
	ClientEntry * rt = g_hash_table_find(App->Clients, (GHRFunc)clients_find_dummy_cb, NULL);
	g_assert(rt);
	return rt;
}
void clients_set_active(ClientEntry * client, Application * App){
	g_assert(!client->IsDead);
	App->ActiveClient = client;
}
void clients_entry_free(ClientEntry * entry){
/* Two way leads to this function:
 * 1, application_free: all non dead containers are freed, and no destroy signal is emited since we disconnect it first;
 * 2, clients_remove_by_container: then the container is already dead and there is no need to destroy it.
 * NOTE: Is this still the case if we don't use socket?
 * */
	if(entry->Type == MENUBAR_REMOTE){
		g_print("Freeing the a remote Menubar.\n");
		if(!entry->IsDead){
			g_print("Destroying Container.\n");
			g_signal_handlers_disconnect_by_func(GTK_WIDGET(entry->Container),
						container_size_allocate_cb, entry->App);
			g_signal_handlers_disconnect_by_func(GTK_WIDGET(entry->Container),
						container_destroy_cb, entry->App);
			gdk_window_reparent(entry->Window, 
				gtk_widget_get_root_window(entry->Container), 0, 0);
			gtk_widget_destroy(GTK_WIDGET(entry->Container));
		}else{
			g_print("Already dead, don't destroy Container.\n");
		}
		g_object_unref(G_OBJECT(entry->Icon));
	} else{ /*MENUBAR_LOCAL*/
		g_print("Freeing the Dummy Menubar. Do you really want it?\n");
		gtk_widget_destroy(entry->Widget);
	}
	g_free(entry->Title);
	g_free(entry);
}
