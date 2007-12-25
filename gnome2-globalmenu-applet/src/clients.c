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

ClientEntry * clients_add_remote_by_stealing(WnckWindow * remoteMenuBarWindow,
	Application * App){
	gint x, y; /*drop these values after obtained*/
	WnckApplication * remoteApp;
	remoteApp = wnck_window_get_application(remoteMenuBarWindow);
	gchar * title = NULL; 
	
	ClientEntry * rt = NULL;
	XWindowID wid = 0;

	title = wnck_application_get_name(remoteApp);
	rt = clients_entry_new(title, App);
	rt->Icon = wnck_application_get_icon(remoteApp);
	rt->Type = MENUBAR_REMOTE;
	rt->IsDead = FALSE;
	rt->Socket = GTK_SOCKET(gtk_socket_new());
	rt->MasterWID = menubar_window_get_master(remoteMenuBarWindow); /*
	Use X's Window ID because we are uncertain whether wnck's WnckWindow* can be a handle for windows */
	rt->Handlers.destroy =
		g_signal_connect(GTK_WIDGET(rt->Socket),
					"destroy", G_CALLBACK(clients_remove_by_socket), App);
	
	wnck_window_get_geometry(remoteMenuBarWindow, 
		&x, &y, &rt->w, &rt->h);
	
	gtk_notebook_append_page(App->Notebook, GTK_WIDGET(rt->Socket), NULL);

	wid = wnck_window_get_xid(remoteMenuBarWindow);
	gtk_socket_steal(rt->Socket, wid);

	gtk_widget_show_all(GTK_WIDGET(rt->Socket));
	g_hash_table_insert(App->Clients, (gpointer) wid, rt);
	return rt;
}

static gboolean clients_find_by_socket_cb(XWindowID menubar_xwid,
				ClientEntry * entry, GtkSocket * socket){
	if(entry->Type != MENUBAR_REMOTE) return FALSE;
	if(entry->Socket == socket){
		return TRUE;
	}
	return FALSE;
}
#define clients_remove_by_socket_cb clients_find_by_socket_cb
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

ClientEntry * clients_find_by_socket(GtkSocket * socket, Application * App){
	return g_hash_table_find(App->Clients, 
		(GHRFunc)clients_find_by_socket_cb, (gpointer)socket);
}

void clients_remove_by_socket(GtkSocket * socket, Application * App){
	ClientEntry * target = clients_find_by_socket(socket, App);
	if(target) target->IsDead = TRUE;
	g_hash_table_foreach_remove(App->Clients, 
		(GHRFunc)clients_remove_by_socket_cb, (gpointer)socket);
}

ClientEntry * clients_find_by_master(XWindowID master, Application * App){
	return g_hash_table_find(App->Clients, 
		(GHRFunc)clients_find_by_master_cb, (gpointer)master);
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
 * 1, application_free: all non dead sockets are freed, and no destroy signal is emited since we disconnect it first;
 * 2, socket_destroy_cb: then the socket is already dead and there is no need to destroy it.
 * */
	if(entry->Type == MENUBAR_REMOTE){
		g_print("Freeing the a remote Menubar.\n");
		if(!entry->IsDead){
			g_print("Destroying Socket.\n");
			g_signal_handler_disconnect(GTK_WIDGET(entry->Socket),
						entry->Handlers.destroy);
			gtk_widget_destroy(GTK_WIDGET(entry->Socket));
		}else{
			g_print("Already dead, don't destroy Socket.\n");
		}
	} else{ /*MENUBAR_LOCAL*/
		g_print("Freeing the Dummy Menubar. Do you really want it?\n");
		gtk_widget_destroy(entry->Widget);
	}
	g_free(entry->Title);
	g_free(entry);
}
