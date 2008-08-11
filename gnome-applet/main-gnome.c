#include <config.h>

#include <gtk/gtk.h>

#include <glade/glade.h>
#include <panel-applet.h>
//#include <libgnomenu/globalmenu.h>

#include "log.h"

#include "intl.h"
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

#define FACTORY_IID "OAFIID:GNOME_GlobalMenuApplet_Factory"
#define APPLET_IID "OAFIID:GNOME_GlobalMenuApplet"

#define APP_NAME "gnome-globalmenu-applet"
#define APP_VERSION "5"


static _change_background ( PanelApplet * applet, 
							PanelAppletBackgroundType bgtp,
							GdkColor * color,
							GdkPixmap * pixmap,
							gpointer app){
	GtkStyle * style = gtk_widget_get_style(applet);
	switch(bgtp){
		case PANEL_NO_BACKGROUND:
		break;
		case PANEL_COLOR_BACKGROUND:
		break;
		case PANEL_PIXMAP_BACKGROUND:
		break;
	}
}
static void _change_orient(PanelApplet * applet,
						PanelAppletOrient ori,
						gpointer app){
	switch(ori){
		case PANEL_APPLET_ORIENT_UP:
		break;
		case PANEL_APPLET_ORIENT_DOWN:
		break;
		case PANEL_APPLET_ORIENT_LEFT:
		break;
		case PANEL_APPLET_ORIENT_RIGHT:
		break;
	}
}
static void 
	_s_screen_active_window_changed	(WnckScreen * screen, WnckWindow * previous, gpointer global_menu){
	WnckWindow * active = wnck_screen_get_active_window(screen);
	if (!active) return;
	if( wnck_window_get_pid(active) == getpid()){
		return;
	}
	gnomenu_global_menu_switch(global_menu, wnck_window_get_xid(active));
}
static gboolean globalmenu_applet_factory (PanelApplet *applet,
                                        const gchar *iid,
                                        gpointer data){
  if (g_str_equal(iid, APPLET_IID)){
	panel_applet_set_flags(applet, 
		PANEL_APPLET_EXPAND_MAJOR | PANEL_APPLET_EXPAND_MINOR | PANEL_APPLET_HAS_HANDLE);
	gtk_widget_set_name(GTK_WIDGET(applet), "globalmenu-applet-eventbox");
//	gnomenu_global_menu_get_type();
	panel_applet_set_background_widget(applet, applet);
	GladeXML * xml;
	GtkContainer * box;
	xml = glade_xml_new("GnomenuServerApplet.glade", NULL, NULL);
	if(!xml)
		xml = glade_xml_new(GLADEDIR"/GnomenuServerApplet.glade", NULL, NULL);
	g_assert(xml);
	/*
	GnomenuGlobalMenu * globalmenu;
	globalmenu = glade_xml_get_widget(xml, "globalmenu");
	box = glade_xml_get_widget(xml, "GnomenuServerApplet");
	glade_xml_signal_autoconnect(xml);
	g_signal_connect(wnck_screen_get_default(),
			"active-window-changed", G_CALLBACK(_s_screen_active_window_changed), globalmenu);
			*/
	gtk_container_add(applet, box);
	gtk_widget_show_all(applet);

    return TRUE;
  } else {
	return FALSE;
	}
}

int main (int argc, char *argv [])
{
	GnomeProgram *program;
	GOptionContext *context;
	int           retval;
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
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
			"widget \"*.globalmenu-applet-eventbox\" style \"gmb_event_box_style\"\n"
			"\n");

	retval = panel_applet_factory_main (FACTORY_IID, PANEL_TYPE_APPLET, globalmenu_applet_factory, NULL);
	g_object_unref (program);
	return retval;
}
/*
vim:ts=4:sw=4
*/
