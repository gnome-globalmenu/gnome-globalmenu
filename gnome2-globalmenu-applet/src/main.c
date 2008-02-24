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
#include <X11/Xlib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4panel/xfce-panel-plugin-iface.h>
//workaround a weird bug in xfce4 includes
#undef _
#undef Q_
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
#include "gtkmenuembed-x11.h"
#include "application.h"
#include "menuclients.h"
#include "menuserver.h"
#include "ui.h"
#include "preference.h"

typedef struct _ClientInfo{
	MenuClient * menu_client;
	GdkWindow * float_window;
	int x;
	int y;
} ClientInfo;

//whether we are running in XFCE mode
gboolean is_xfce = FALSE;

//static gboolean get_cardinal_by_atom(Window xwin, Display *display, Atom atom, int* ret);

static void return_client_float_window(ClientInfo * client, Application * App){
	g_print("Return client float window\n");
	GlobalMenuNotify notify;
	notify.type = GM_NOTIFY_SET_VISIBLE;
	notify.SetVisible.visible = FALSE;
	menu_server_send_to(App->Server, client->menu_client, &notify);	
	gdk_window_reparent(client->float_window, 
		gtk_widget_get_root_window(GTK_WIDGET(App->Holder)), 0, 0);
	gdk_window_hide(client->float_window);
}
static void steal_client_float_window(ClientInfo * client, Application * App){
	g_print("Steal client float window\n");
	GlobalMenuNotify notify;
	notify.type = GM_NOTIFY_SET_VISIBLE;
	notify.SetVisible.visible = TRUE;
	gdk_window_reparent(client->float_window, 
		GTK_WIDGET(App->Holder)->window, client->x, client->y);
	gdk_window_show(client->float_window);
	menu_server_send_to(App->Server, client->menu_client, &notify);	
}
ClientInfo * find_client_info_by_master(Window master_xid, Application * App){
	GList * node;
	for(node = g_list_first(App->Clients); node ; node = g_list_next(node)){
		if(((ClientInfo*)node->data)->menu_client->master_xid == master_xid){
			g_print("Client menubar found\n");
			return node->data;
		}
	}
	return NULL;
}
static void active_window_changed_cb(WnckScreen* screen, WnckWindow *previous_window, Application * App){
	WnckWindow * active_window = NULL;
	WnckWindow * parent_window;
	Window active_xid = 0; 
	Window parent_xid = 0;
	gboolean client_known = FALSE;
	ClientInfo * info;
	ClientInfo * info_fallback;
	//GList * node = NULL;

	if(App->ActiveTitle){
		g_free(App->ActiveTitle);
		App->ActiveTitle = NULL;
	}
	if(App->ActiveIcon){
		g_object_unref(App->ActiveIcon);
		App->ActiveIcon = NULL;
	}

	active_window = wnck_screen_get_active_window(screen);
/* XXX:why sometimes active_window is NULL? 
 * Because it is destroyed before wnck return it to us.
 * */
	if(active_window == NULL){
		g_warning("active window is NULL!");
		if(App->ActiveClient)
			return_client_float_window(App->ActiveClient, App);
		App->ActiveClient = NULL;
		ui_repaint_all(App);
		return;
	}

	g_print("Active Window_changed\n");
	if(WNCK_IS_WINDOW(active_window)){
		active_xid = wnck_window_get_xid(active_window);

		parent_window = wnck_window_get_transient(active_window);
		if(WNCK_IS_WINDOW(parent_window))
			parent_xid = wnck_window_get_xid(parent_window);
		else parent_xid = 0;
		
		g_print("Active window ID is %p\n", (gpointer) active_xid);
		g_print("Parent window (of active window) xid: %p\n", (gpointer)parent_xid);

		g_debug("Group leader xid = %p\n", wnck_window_get_group_leader(active_window));

		App->ActiveTitle = g_strdup(wnck_window_get_name(active_window));
		App->ActiveIcon = wnck_window_get_icon(active_window);
		g_object_ref(App->ActiveIcon);

		info = find_client_info_by_master(active_xid, App);
/* Try to find if its parent has menubar. */
		info_fallback = find_client_info_by_master(parent_xid, App);
		if(info == NULL) info = info_fallback;
		if(info == NULL) client_known = FALSE;
			else client_known = TRUE;
				
	}else {
		g_print("Active Window is not a window, that's Stupid!\n");
	}


/* if the active_window has menu, we always use its own menu */
	if(client_known){
		if(App->ActiveClient != info){
			if(App->ActiveClient)
				return_client_float_window(App->ActiveClient, App);
			steal_client_float_window(info, App);
			App->ActiveClient = info;
		}
	}else if(App->ActiveClient ){
		/* both the new active winodw and group leader don't have the menbar, return the
		 * menubar to owner.*/
		return_client_float_window(App->ActiveClient, App);
		App->ActiveClient = NULL;
	}
	ui_repaint_all(App);
}

gboolean is_kde_topmenu(WnckWindow * window, Window * ptransient){
	Display * display = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
	Window w = wnck_window_get_xid(window);
	Atom win_type_property = gdk_x11_atom_to_xatom(gdk_atom_intern("_NET_WM_WINDOW_TYPE", FALSE));
	Atom transient_property = gdk_x11_atom_to_xatom(gdk_atom_intern(
		"WM_TRANSIENT_FOR", FALSE));

	glong long_offset = 0;
	glong long_length = 128;
	gboolean delete = FALSE;
//gdk_x11_atom_to_xatom(gdk_atom_intern("AnyPropertyType", FALSE));
	Atom actual_type_return;
	gint actual_format_return;
	gulong nitems_return;
	gulong bytes_after_return;
	unsigned char * prop_return;
	Atom window_type ;
	Atom kde_type = XInternAtom(display, "_KDE_NET_WM_WINDOW_TYPE_TOPMENU", FALSE);
	Window transient = NULL;
	gdk_error_trap_push();
	if( Success != XGetWindowProperty(display, w, 
						win_type_property, long_offset, 
						long_length, delete, XA_ATOM, 
                        &actual_type_return, &actual_format_return, &nitems_return, &bytes_after_return, 
                        &prop_return)){
		g_print("Failed to get property\n");
	}else{
		if(prop_return){
		window_type = * (Atom *)prop_return;
		XFree(prop_return);
		g_print("windowtype =  %s\n", XGetAtomName(display, window_type));
		} else
		window_type = NULL;
	}
	if( Success != XGetWindowProperty(display, w, 
						transient_property, 
						long_offset, long_length, 
						delete, XA_WINDOW, 
                        &actual_type_return, &actual_format_return, 
						&nitems_return, &bytes_after_return, 
                        &prop_return)){
		g_print("Failed to get property\n");
	}else{
		if(prop_return){
			transient = * (Window *) prop_return;
			XFree(prop_return);
			g_print("transient = %p\n", transient);
		}
	}
	if(kde_type == window_type) *ptransient = transient;
	gdk_flush();
	gdk_error_trap_pop();
	
	return kde_type == window_type;	
}
static void window_opened_cb(WnckScreen* screen, WnckWindow *window, Application * App){
	g_print("Window opened: %s\n", wnck_window_get_name(window));
	Window transient;
	if(is_kde_topmenu(window, &transient)) {
		g_print("a kde top menu\n");
		GlobalMenuNotify notify;
		notify.ClientNew.client_xid = 0; //wnck_window_get_xid(window); /*0?*/
		notify.ClientNew.float_xid = wnck_window_get_xid(window);
		notify.ClientNew.master_xid = transient;
		/*simulate a client new. hope it works*/
		menu_server_client_new_cb(NULL, &notify, App->Server);
	}
}
static void window_closed_cb(WnckScreen* screen, WnckWindow *window, Application * App){
	g_print("Window closed: %s\n", wnck_window_get_name(window));
}

static void client_new_cb(MenuServer * server, MenuClient * client, Application * App){
	ClientInfo * info = g_new0(ClientInfo, 1);
	GlobalMenuNotify notify;
	GtkAllocation * allocation;
	Atom atom;

	g_print("Applet: Client New:%p\n", (void*)client->client_xid);
	info->menu_client = client;
	info->float_window = gdk_window_foreign_new(client->float_xid);
	info->x = 0;
	info->y = 0;

/*since we don't want it be shown in the screen, perhaps its better to hide it in the menubar patch**/
	gdk_window_hide(info->float_window);
	allocation = &GTK_WIDGET(App->Holder)->allocation;
	notify.type = GM_NOTIFY_SIZE_ALLOCATE;
	notify.SizeAllocate.width = allocation->width;
	notify.SizeAllocate.height = allocation->height;
	g_message("client_new_cb: allocate->w, h = %d, %d", allocation->width,
			allocation->height);
	menu_server_send_to(App->Server, info->menu_client, &notify);
	App->Clients = g_list_append(App->Clients, info);
}
static void client_destroy_cb(MenuServer * server, MenuClient * client, Application * App){
	GList * node = NULL;
	ClientInfo * info = NULL;
	g_print("Applet: Client Destroy\n");
	for(node = g_list_first(App->Clients); node; node = g_list_next(node)){
		if(((ClientInfo*) node->data)->menu_client == client){
			info = (ClientInfo*) (node->data);
			break;
		}
	}	
	g_return_if_fail( node );
	g_return_if_fail( info );
/*****special deal if it is the current application********/
	if(info == App->ActiveClient) App->ActiveClient = NULL;
/* However, We need to tell our gdk this window no longer exists. I hope gdk_iwndow_unref will deal with it if the window still exists.*/
	gdk_window_unref(info->float_window);
	App->Clients = g_list_remove_all(App->Clients, info);
	g_free(info);
}
static void application_free(Application * App);
static gboolean main_window_destroy_cb(GtkWindow * MainWindow, Application * App){
	g_print("Main window destroyed.\n");
	application_free(App);
	return TRUE;
}
static gboolean main_window_delete_cb(GtkWindow * MainWindow, GdkEvent * event, Application * App){
	g_print("Main window delete-event.\n");
	return FALSE;
}

static void main_window_change_background_cb(PanelApplet * applet, PanelAppletBackgroundType type, GdkColor * color, GdkPixmap * pixmap, Application * App){
	GtkStyle * style;
	Window bg_xid;
	Atom color_atom;
	GlobalMenuNotify notify;
	gchar * color_name = NULL;
	g_print("Applet Background has changed\n");
/*Should notify the client!*/
	GdkWindow * window = GTK_WIDGET(App->Holder)->window;
	g_return_if_fail(window);
	switch(type){
		case PANEL_NO_BACKGROUND:{
			style = gtk_widget_get_style(GTK_WIDGET(App->MainWindow));
			gdk_window_set_back_pixmap(window, NULL, FALSE);
			gtk_widget_modify_bg(GTK_WIDGET(App->Holder), GTK_STATE_NORMAL, &style->bg[GTK_STATE_NORMAL]);
			color_name = gdk_color_to_string(&style->bg[GTK_STATE_NORMAL]);
			break;
		}
		case PANEL_COLOR_BACKGROUND:
			gdk_window_set_back_pixmap(window, NULL, FALSE);
			gtk_widget_modify_bg(GTK_WIDGET(App->Holder), GTK_STATE_NORMAL, color);
			color_name = gdk_color_to_string(color);
			break;
		case PANEL_PIXMAP_BACKGROUND:
			{
			GdkPixmap * holder_bg;
			GdkGC * gc;
			gint x, y;
			gint w, h;
			gdk_window_get_position(window, &x, &y);
			gdk_window_get_size(window, &w, &h);
			g_print("%d, %d, %d, %d\n", x, y, w, h);
			holder_bg = gdk_pixmap_new(pixmap, w, h, -1);
			gc = gdk_gc_new(pixmap);
			gdk_draw_drawable(holder_bg, gc, pixmap, x, y, 0, 0, w, h);
			gdk_window_set_back_pixmap(window, holder_bg, FALSE);
			g_object_unref(gc);
			bg_xid = GDK_PIXMAP_XID(holder_bg);
			}
			break;
	}
	gtk_widget_queue_draw(GTK_WIDGET(App->Holder));
	if(color_name){
		color_atom = gdk_x11_get_xatom_by_name(color_name);
		g_free(color_name);
	} else 
		color_atom = 0;
	notify.SetBackground.color_atom = color_atom;
	notify.SetBackground.pixmap_xid = bg_xid;
	notify.type = GM_NOTIFY_SET_BACKGROUND;
	menu_server_broadcast(App->Server, &notify);
}
static void main_window_change_orient_cb(PanelApplet * applet, PanelAppletOrient orient, Application * App){
	g_print("Applet Orientation has changed\n");
}

static void holder_resize_cb(GtkWidget * widget, GtkAllocation * allocation, Application * App){
	g_print("Holder resizesd.\n");
	GlobalMenuNotify notify;
	g_print("holder_resize_cb:Broadcast message to all clients: %d, %d, %d, %d\n", *allocation);
	if (App->ActiveClient) {
		notify.type = GM_NOTIFY_SIZE_ALLOCATE;
		notify.SizeAllocate.width = allocation->width;
		notify.SizeAllocate.height = allocation->height;
		menu_server_broadcast(App->Server, &notify);
	}

}
static gboolean label_area_action_cb(GtkWidget * widget, GdkEventButton* button, Application * App){
	g_print("client icon action.\n");
	if(button->button == 1){
		gboolean rt;
		g_signal_emit_by_name(G_OBJECT(App->MainWindow), "popup_menu", &rt);
	}
	return TRUE;
}
static gboolean backward_action_cb(GtkWidget * widget, GdkEventButton * button, Application * App){
	GtkAllocation *allocation;
	GlobalMenuNotify notify;
	g_print("backward action.\n");
	if(App->ActiveClient){
		App->ActiveClient->x -= 10;
 
		memset(&notify, 0 ,sizeof(notify));
		allocation = &(GTK_WIDGET(App->Holder))->allocation;
		notify.type = GM_NOTIFY_SIZE_ALLOCATE;
		notify.SizeAllocate.width = allocation->width - App->ActiveClient->x;
		notify.SizeAllocate.height = allocation->height;
		menu_server_send_to(App->Server, App->ActiveClient->menu_client, &notify);
 
		gdk_window_move(App->ActiveClient->float_window, 
				App->ActiveClient->x,
				App->ActiveClient->y);
	}
	ui_repaint_all(App);
	return TRUE;
}
static gboolean forward_action_cb(GtkWidget * widget, GdkEventButton * button, Application * App){
	GtkAllocation *allocation;
	GlobalMenuNotify notify;
	g_print("forward action.\n");
	if(App->ActiveClient){
		App->ActiveClient->x += 10;
		allocation = &(GTK_WIDGET(App->Holder))->allocation;
		if ( allocation->width - App->ActiveClient->x > 0)
		{
			memset(&notify, 0 ,sizeof(notify));
			notify.type = GM_NOTIFY_SIZE_ALLOCATE;
			notify.SizeAllocate.width = allocation->width - App->ActiveClient->x;
			notify.SizeAllocate.height = allocation->height;
			menu_server_send_to(App->Server, App->ActiveClient->menu_client, &notify);
		}
		gdk_window_move(App->ActiveClient->float_window, 
				App->ActiveClient->x,
				App->ActiveClient->y);
	}
	ui_repaint_all(App);
	return TRUE;
}

static void popup_menu_cb(BonoboUIComponent * uic, Application * App, gchar * cname){
	g_message("%s: cname = %s", __func__, cname);
	if(g_str_equal(cname, "About")) ui_show_about(NULL, App);
	if(g_str_equal(cname, "Preference")) preference_show_dialog(NULL, App);
}

static Application * application_new(GtkContainer * mainwindow){
	Application * App = g_new0(Application, 1);
	GdkScreen * gdkscreen = NULL;
	UICallbacks callback_table;
	

	App->Server = menu_server_new();
	App->Clients = NULL;
	App->ActiveClient = NULL;
	App->ActiveTitle = NULL;
	App->ActiveIcon = NULL;

	menu_server_set_user_data(App->Server, App);
	menu_server_set_callback(App->Server, 
		MS_CB_CLIENT_NEW, 
		(MenuServerCallback) client_new_cb);
	menu_server_set_callback(App->Server, 
		MS_CB_CLIENT_DESTROY, 
		(MenuServerCallback) client_destroy_cb);
	App->MainWindow = mainwindow;
/*Only when MainWindow is known we can load conf*/
	preference_load_conf(App);

	g_signal_connect(G_OBJECT(App->MainWindow), "destroy",
		G_CALLBACK(main_window_destroy_cb), App);
/******The delete-event not useful any more, since panel-applet dont receive it. *******/
	g_signal_connect(G_OBJECT(App->MainWindow), "delete-event",
		G_CALLBACK(main_window_delete_cb), App);
  if (is_xfce) {
		xfce_panel_plugin_menu_show_about(XFCE_PANEL_PLUGIN(App->MainWindow));
		xfce_panel_plugin_menu_show_configure(XFCE_PANEL_PLUGIN(App->MainWindow));
		g_signal_connect(G_OBJECT(App->MainWindow), "about",
				G_CALLBACK(ui_show_about), App);
		g_signal_connect(G_OBJECT(App->MainWindow), "configure-plugin",
				G_CALLBACK(preference_show_dialog), App);
	}
	else {
		g_signal_connect(G_OBJECT(App->MainWindow), "change-background",
				G_CALLBACK(main_window_change_background_cb), App);
		g_signal_connect(G_OBJECT(App->MainWindow), "change-orient",
				G_CALLBACK(main_window_change_orient_cb), App);
		panel_applet_set_background_widget(PANEL_APPLET(App->MainWindow), App->MainWindow);
	}
/**The Screen***/
	gdkscreen = gtk_widget_get_screen(GTK_WIDGET(App->MainWindow));
	g_print("GDK SCREEN number is %d\n", gdk_screen_get_number(gdkscreen));
	App->Screen = wnck_screen_get(gdk_screen_get_number(gdkscreen));
	g_print("App->Screen is %p\n",App->Screen);

	g_signal_connect(G_OBJECT(App->Screen), "active-window-changed", 
		G_CALLBACK(active_window_changed_cb), App);
	g_signal_connect(G_OBJECT(App->Screen), "window-opened", 
		G_CALLBACK(window_opened_cb), App);
	g_signal_connect(G_OBJECT(App->Screen), "window-closed", 
		G_CALLBACK(window_closed_cb), App);

/******Create the UI *****/
	callback_table.label_area_action_cb = G_CALLBACK(label_area_action_cb);
	callback_table.forward_action_cb = G_CALLBACK(forward_action_cb);
	callback_table.backward_action_cb = G_CALLBACK(backward_action_cb);
	callback_table.holder_resize_cb = G_CALLBACK(holder_resize_cb);
  if (!is_xfce) {
		callback_table.popup_menu_cb = (BonoboUIVerbFn)popup_menu_cb;
	}

	ui_create_all(App, &callback_table);

/**********All done**********/

	gtk_widget_show_all(GTK_WIDGET(mainwindow));

/*****Hide them since they don't do nothing**********/
	gtk_widget_hide(GTK_WIDGET(App->Forward));
	gtk_widget_hide(GTK_WIDGET(App->Backward));
	/*I think it is not nessary since we have an app icon already. Maybe can let user choose whether use TitleLabel or Icon*/
	gtk_widget_hide(GTK_WIDGET(App->TitleLabel)); 
	/*if we have registered the signals of App->Screen, all clients can be discovered in this function. if we haven't, here we can not find any windows.*/
	
	menu_server_start(App->Server);
	return App;
}
static void application_free(Application * App){
	g_signal_handlers_disconnect_by_func(App->Screen, active_window_changed_cb, App);
	g_signal_handlers_disconnect_by_func(App->Screen, window_opened_cb, App);
	g_signal_handlers_disconnect_by_func(App->Screen, window_closed_cb, App);
	if(App->ActiveClient){ /*Perhaps not useful, since when we reach here,
							 the a delete-event has been sent to
                             activeclient->float_window*/
		/*So don't return active client, just forget it and let the application itself handle it*/
		//return_client_float_window(App->ActiveClient, App);
		App->ActiveClient = NULL;
	}
	menu_server_shutdown(App->Server);
	menu_server_free(App->Server);
	g_free(App);
}


#define FACTORY_IID "OAFIID:GNOME_GlobalMenuApplet2_Factory"
#define APPLET_IID "OAFIID:GNOME_GlobalMenuApplet2"

#define APP_NAME "globalmenu-applet"
#define APP_VERSION "2"

static gboolean globalmenu_applet_factory (PanelApplet *applet,
                                        const gchar *iid,
                                        gpointer data){
	Application * App;
  if (g_str_equal(iid, APPLET_IID)){
	panel_applet_set_flags(applet, 
		PANEL_APPLET_EXPAND_MAJOR | PANEL_APPLET_EXPAND_MINOR | PANEL_APPLET_HAS_HANDLE);
	gtk_widget_set_name(GTK_WIDGET(applet), "globalmenu-applet-eventbox");
    App = application_new(GTK_CONTAINER(applet));
    return TRUE;
  } else {
	return FALSE;
	}
}

static gboolean size_changed(GtkWidget* plugin, gint size) {
	return TRUE;
}

static void
xfce_applet_construct(XfcePanelPlugin *plugin)
{
	g_print("constructing plugin\n");
	g_signal_connect(G_OBJECT(plugin), "size-changed", 
		G_CALLBACK(size_changed), NULL);
	application_new(GTK_CONTAINER(plugin));
}

int 
main (int argc, char **argv) 
{ 
  if (strstr(argv[0], "xfce")) {
		is_xfce = TRUE;
		g_print("xfce mode\n");
		// The following code was stolen from xfce4/libxfce4panel/xfce-panel-plugin.h
		// REGISTER_EXTERNAL_PLUGIN macro
		GtkWidget *plugin; 
		gtk_init (&argc, &argv); 
		plugin = xfce_external_panel_plugin_new (argc, argv, 
								 (XfcePanelPluginFunc)xfce_applet_construct); 
		if (!plugin) return 1; 
		g_signal_connect_after (plugin, "destroy", 
														G_CALLBACK (exit), NULL); 
		gtk_widget_show (plugin); 
		gtk_main (); 
		return 0; 
	}
	else {
		GnomeProgram *program;
		GOptionContext *context;
		int           retval;
	#ifdef ENABLE_NLS
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
		textdomain (GETTEXT_PACKAGE);
	#endif
		
		context = g_option_context_new("");
		program = gnome_program_init (APP_NAME, APP_VERSION,
				LIBGNOMEUI_MODULE,
				argc, argv,
				GNOME_PARAM_GOPTION_CONTEXT, context,	
				GNOME_CLIENT_PARAM_SM_CONNECT, FALSE,	
				GNOME_PARAM_NONE);
		gtk_rc_parse_string("\n"
				"style \"gmb_event_box_style\" \n"
				"{\n"
				" 	GtkWidget::focus-line-width=0\n"
				" 	GtkWidget::focus-padding=0\n"
				"}\n"
				"\n"
	//			"widget \"*.globalmenu-applet\" style \"gmb_event_box_style\"\n"
				"widget \"*.globalmenu-applet-eventbox\" style \"gmb_event_box_style\"\n"
				"\n");

		retval = panel_applet_factory_main (FACTORY_IID, PANEL_TYPE_APPLET, globalmenu_applet_factory, NULL);
		g_object_unref (program);
		return retval;
	}
}

