/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Yu Feng 2007 <rainwoodman@localhost.localdomain>
 * 
 * main.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * main.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with main.c.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

#include <gconf/gconf-client.h>
#include <panel-applet.h>
/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#include "typedefs.h"
#include "clients.h"
#include "misc.h"

void repaint_applet(Application * App){
	int page_num;
	guint h;
	ClientEntry * client = App->ActiveClient;
	if(client->IsDummy){
			page_num = gtk_notebook_page_num(App->Notebook, client->Widget);
	}else{
			page_num = gtk_notebook_page_num(App->Notebook, GTK_WIDGET(client->Socket));
	}
	g_assert(page_num != -1);
	gtk_notebook_set_current_page(App->Notebook, page_num);
	gtk_label_set_text(App->TitleLabel, client->Title);
	if(client->IsDummy){ /*Should load a pixmap for dummy*/
		gtk_image_set_from_pixbuf(App->ClientIcon, NULL);
	}else{
		gtk_image_set_from_pixbuf(App->ClientIcon, client->Icon);
	}
	h = GTK_WIDGET(App->Layout)->allocation.height;
	gtk_widget_set_size_request(GTK_WIDGET(App->Notebook), client->w, h);
}

static void active_window_changed_cb(WnckScreen* screen, WnckWindow *previous_window, Application * App){
	WnckWindow * active_window = NULL;
	XWindowID active_wid = 0; 
	ClientEntry * client = NULL;

	active_window = wnck_screen_get_active_window(screen);
	g_print("Active Window_changed\n");
	if(WNCK_IS_WINDOW(active_window)){
		active_wid = wnck_window_get_xid(active_window);
		g_print("Active XWin ID is %p\n", active_wid);
		client = clients_find_by_master(active_wid, App);
	}else{
		g_print("Active Window is not a window, that's Stupid!\n");
		client = NULL;
	}
		g_print("New Client is %p\n", client);
	if(client){
		clients_set_active(client, App);
	}else{
		clients_set_active(clients_find_dummy(App), App);
	}
	repaint_applet(App);
}
static void window_opened_cb(WnckScreen* screen, WnckWindow *window, Application * App){
	if(wnck_window_is_menubar(window)){
		clients_add_remote(window, App);
	}
}
static void application_free(Application * App);
static gboolean main_window_destroy_cb(GtkWindow * MainWindow, Application * App){
	g_print("Server quited\n");
	application_free(App);
	gtk_main_quit();
	return TRUE;
}

static Application * application_new(GtkContainer * mainwindow){
	Application * App = g_new0(Application, 1);
	GdkScreen * gdkscreen = NULL;
	GtkBox * basebox = NULL;
	App->Clients = g_hash_table_new_full(g_direct_hash, 
									g_direct_equal, 
									NULL, 
									(GDestroyNotify)clients_entry_free);
	App->MainWindow = mainwindow;
	g_signal_connect(G_OBJECT(App->MainWindow), "destroy",
		G_CALLBACK(main_window_destroy_cb), App);

	gdkscreen = gtk_widget_get_screen(GTK_WIDGET(App->MainWindow));
	g_print("GDK SCREEN number is %d\n", gdk_screen_get_number(gdkscreen));
	App->Screen = wnck_screen_get(gdk_screen_get_number(gdkscreen));
	g_print("App->Screen is %p\n",App->Screen);

	App->Handlers.active_window_changed = 
		g_signal_connect(G_OBJECT(App->Screen), "active-window-changed", 
			G_CALLBACK(active_window_changed_cb), App);
	App->Handlers.window_opened = 
		g_signal_connect(G_OBJECT(App->Screen), "window-opened", 
			G_CALLBACK(window_opened_cb), App);

	basebox = GTK_BOX(gtk_hbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(App->MainWindow), GTK_WIDGET(basebox));

	App->ClientIcon = GTK_IMAGE(gtk_image_new());
	gtk_box_pack_start(basebox, GTK_WIDGET(App->ClientIcon), FALSE, FALSE, 0);

	App->TitleLabel = GTK_LABEL(gtk_label_new("abced"));
	gtk_label_set_max_width_chars(App->TitleLabel, 20);
	gtk_box_pack_start(basebox, GTK_WIDGET(App->TitleLabel), FALSE, FALSE, 0);

	App->Layout = GTK_LAYOUT(gtk_layout_new(NULL, NULL));
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Layout), TRUE, TRUE, 0);

	App->Notebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_layout_put(App->Layout, GTK_WIDGET(App->Notebook), 0, 0); /*inital position*/
	
	clients_set_active(clients_add_dummy("dummy", App), App);

	gtk_widget_show_all(GTK_WIDGET(mainwindow));
	/*if we have registered the signals of App->Screen, all clients can be discovered in this function. if we haven't, here we can not find any windows.*/
	clients_discover_all(App);
	
	return App;
}
static void application_free(Application * App){
	g_signal_handler_disconnect(App->Screen, App->Handlers.active_window_changed);
	g_signal_handler_disconnect(App->Screen, App->Handlers.window_opened);
	g_hash_table_destroy(App->Clients);
	g_free(App);
}

#ifndef STANDALONE
static gboolean globalmenu_applet_factory (PanelApplet *applet,
                                        const gchar *iid,
                                        gpointer data){
  if (g_str_equal(iid, "OAFIID:GNOME_GlobalMenuApplet")){
	panel_applet_set_flags(applet, 
		PANEL_APPLET_EXPAND_MAJOR | PANEL_APPLET_EXPAND_MINOR);
    application_new(GTK_CONTAINER(applet));
    return TRUE;
  } else return FALSE;
}
#define GNOMELOCALEDIR PACKAGE_LOCALE_DIR

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:GNOME_GlobalMenuApplet_Factory",
                 PANEL_TYPE_APPLET,
                 "globalmenu",
                 "0",
                 globalmenu_applet_factory,
                 NULL)


#else

static gboolean clicked(GtkWindow * mainwindow, gpointer button, Application * App){
	static gboolean trigger=FALSE;
	if(!trigger){
		clients_discover_all(App);
		trigger = TRUE;
	}
	return TRUE;
}
static gboolean start_applet(GtkWindow * mainwindow, gpointer unused){
	Application * App;
	App = application_new(mainwindow);
//	gdk_window_set_events(GTK_WIDGET(mainwindow)->window,
//		GDK_BUTTON_PRESS_MASK);
//	g_signal_connect(G_OBJECT(mainwindow), "button-press-event", clicked, App);
	return TRUE;
}

int
main (int argc, char *argv[])
{
	GtkWindow * mainwindow;
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_set_locale ();
	gtk_init (&argc, &argv);
	mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(mainwindow), "show", start_applet, NULL);
	gtk_widget_show_all(GTK_WIDGET(mainwindow));
	gtk_main ();
	return 0;
}
#endif
