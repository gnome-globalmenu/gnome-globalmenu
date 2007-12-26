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
#include "ui.h"


static void active_window_changed_cb(WnckScreen* screen, WnckWindow *previous_window, Application * App){
	WnckWindow * active_window = NULL;
	XWindowID active_wid = 0; 
	ClientEntry * client = NULL;

	if(App->Mode == APP_STANDALONE && gtk_window_has_toplevel_focus(
		GTK_WIDGET(App->MainWindow))) {
				/*Don't change if I am activted*/
		return;
	}
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
	ui_repaint_all(App);
}
static void window_opened_cb(WnckScreen* screen, WnckWindow *window, Application * App){
	if(wnck_window_is_stealable_menubar(window)){
		clients_add_remote_by_stealing(window, App);
	}
}
static void application_free(Application * App);
static gboolean main_window_destroy_cb(GtkWindow * MainWindow, Application * App){
	g_print("Server quited\n");
	application_free(App);
	gtk_main_quit();
	return TRUE;
}
static void label_area_button_press_cb(GtkWidget * widget, GdkEventButton* button, Application * App){
	g_print("Button Pressed on client icon.\n");
	if(App->Mode == APP_APPLET){ /*Emit the applet popup menu*/
		if(button->button == 1){
			g_signal_emit_by_name(G_OBJECT(App->MainWindow), "popup_menu", NULL);
		}
	}
}
static void backward_button_press_cb(GtkWidget * widget, GdkEventButton * button, Application * App){
	g_print("Button Pressed on backward icon.\n");
	App->ActiveClient->x -=10;
	ui_repaint_all(App);
}
static void forward_button_press_cb(GtkWidget * widget, GdkEventButton * button, Application * App){
	g_print("Button Pressed on forward icon.\n");
	App->ActiveClient->x +=10;
	ui_repaint_all(App);
}
static Application * application_new(GtkContainer * mainwindow, enum AppMode Mode){
	Application * App = g_new0(Application, 1);
	GdkScreen * gdkscreen = NULL;
	GtkBox * basebox = NULL;
	GtkEventBox * label_area = NULL;

	GtkButton * button = NULL; /*So the dashed box will cover the button instead of the Applet, and the menu will response when clicked at the very top.*/

	App->Clients = g_hash_table_new_full(g_direct_hash, 
									g_direct_equal, 
									NULL, 
									(GDestroyNotify)clients_entry_free);
	App->Mode = Mode;
	App->MainWindow = mainwindow;
	g_signal_connect(G_OBJECT(App->MainWindow), "destroy",
		G_CALLBACK(main_window_destroy_cb), App);

/**The Screen***/
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
/****** Applet's Base Horizontal Box*******/
	basebox = GTK_BOX(gtk_hbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(App->MainWindow), GTK_WIDGET(basebox));

/********Label Area********/
	App->ClientIcon = GTK_IMAGE(gtk_image_new());
	App->TitleLabel = GTK_LABEL(gtk_label_new(""));
	gtk_label_set_max_width_chars(App->TitleLabel, 10);

	label_area = ui_create_label_area(App);
	gtk_box_pack_start(basebox, GTK_WIDGET(label_area), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(label_area), "button-press-event",
			G_CALLBACK(label_area_button_press_cb), App);

/****** Move Backward ***********/
	App->Backward = ui_create_event_box_with_icon(GTK_STOCK_GO_BACK);
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Backward), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(App->Backward), "button-press-event",
			G_CALLBACK(backward_button_press_cb), App);

/**********Layout and Notebook: Menubars Shows here**********/
	App->Layout = GTK_LAYOUT(gtk_layout_new(NULL, NULL));
	App->Notebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_layout_put(App->Layout, GTK_WIDGET(App->Notebook), 0, 0); /*inital position*/
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Layout), TRUE, TRUE, 0);

/****** Move Forward ***********/
	App->Forward = ui_create_event_box_with_icon(GTK_STOCK_GO_FORWARD);
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Forward), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(App->Forward), "button-press-event",
			G_CALLBACK(forward_button_press_cb), App);

/********Button: focus hack*********/
	button = GTK_BUTTON(gtk_button_new());
	gtk_button_set_relief(button, GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
	gtk_box_pack_start(basebox, GTK_WIDGET(button),
				FALSE, FALSE, 0);
/**********All done**********/
	clients_set_active(clients_add_dummy("    ", App), App);

	if(App->Mode == APP_APPLET){ /*setup a packed visual if in a panel*/
		gtk_container_set_border_width(GTK_CONTAINER(basebox), 0);
		gtk_container_set_border_width(GTK_CONTAINER(App->Layout), 0);
		/*layout don't care about border width, anyway, set it*/
		gtk_container_set_border_width(GTK_CONTAINER(App->Notebook), 0);
		gtk_notebook_set_show_tabs(App->Notebook, FALSE);
		gtk_notebook_set_show_border(App->Notebook, FALSE);
	}

	gtk_widget_show_all(GTK_WIDGET(mainwindow));
	/*I think it is not nessary since we have an app icon already. Maybe can let user choose whether use TitleLabel or Icon*/
	gtk_widget_hide(GTK_WIDGET(App->TitleLabel)); 
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


static gboolean clicked(GtkWindow * mainwindow, gpointer button, Application * App){
	static gboolean trigger=FALSE;
	if(!trigger){
		clients_discover_all(App);
		trigger = TRUE;
	}
	return TRUE;
}
static gboolean start_standalone(GtkWindow * mainwindow, gpointer unused){
	Application * App;
	App = application_new(GTK_CONTAINER(mainwindow), APP_STANDALONE);
//	gdk_window_set_events(GTK_WIDGET(mainwindow)->window,
//		GDK_BUTTON_PRESS_MASK);
//	g_signal_connect(G_OBJECT(mainwindow), "button-press-event", clicked, App);
	return TRUE;
}

#define FACTORY_IID "OAFIID:GNOME_GlobalMenuApplet_Factory"
#define APPLET_IID "OAFIID:GNOME_GlobalMenuApplet"
#define APP_NAME "globalmenu-applet"
#define APP_VERSION "0"

static gboolean globalmenu_applet_factory (PanelApplet *applet,
                                        const gchar *iid,
                                        gpointer data){
	Application * App;
  if (g_str_equal(iid, APPLET_IID)){
	panel_applet_set_flags(applet, 
		PANEL_APPLET_EXPAND_MAJOR | PANEL_APPLET_EXPAND_MINOR);
    App = application_new(GTK_CONTAINER(applet), APP_APPLET);
    return TRUE;
  } else return FALSE;
}

int main (int argc, char *argv [])
{
	GnomeProgram *program;
	GOptionContext *context;
	int           retval;
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	if(argc == 1){
	/*standalone mode*/
		GtkWindow * mainwindow;
		gtk_set_locale ();
		gtk_init (&argc, &argv);
		mainwindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
		g_signal_connect(G_OBJECT(mainwindow), "show", G_CALLBACK(start_standalone), NULL);
		gtk_widget_show_all(GTK_WIDGET(mainwindow));
		gtk_main ();
		return 0;
	} else{
	/*bonobo server mode*/
		context = g_option_context_new("");
		program = gnome_program_init (APP_NAME, APP_VERSION,
						  LIBGNOMEUI_MODULE,
						  argc, argv,
						  GNOME_PARAM_GOPTION_CONTEXT, context,	
						  GNOME_CLIENT_PARAM_SM_CONNECT, FALSE,	
						  GNOME_PARAM_NONE);
		retval = panel_applet_factory_main (FACTORY_IID, PANEL_TYPE_APPLET, globalmenu_applet_factory, NULL);
		g_object_unref (program);
		return retval;
	}
}
