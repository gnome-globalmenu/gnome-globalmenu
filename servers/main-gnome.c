#include <config.h>

#include <gtk/gtk.h>


#include <panel-applet.h>
#include "libgnomenu/serverhelper.h"

/*
 * Standard gettext macros.
 */

#include "application-gnome.h"

#include "log.h"

#define FACTORY_IID "OAFIID:GNOME_GlobalMenuApplet_Factory"
#define APPLET_IID "OAFIID:GNOME_GlobalMenuApplet"

#define APP_NAME "gnome-globalmenu-applet"
#define APP_VERSION "4"


static _change_background ( PanelApplet * applet, 
							PanelAppletBackgroundType bgtp,
							GdkColor * color,
							GdkPixmap * pixmap,
							Application * app){
	GtkStyle * style = gtk_widget_get_style(applet);
	switch(bgtp){
		case PANEL_NO_BACKGROUND:
			application_set_background(app, &style->bg[GTK_STATE_NORMAL], NULL);
		break;
		case PANEL_COLOR_BACKGROUND:
			application_set_background(app, color, NULL);
		break;
		case PANEL_PIXMAP_BACKGROUND:
			LOG("background");
			application_set_background(app, NULL, pixmap);
		break;
	}
}
static gboolean globalmenu_applet_factory (PanelApplet *applet,
                                        const gchar *iid,
                                        gpointer data){
	Application * App;
  if (g_str_equal(iid, APPLET_IID)){
	panel_applet_set_flags(applet, 
		PANEL_APPLET_EXPAND_MAJOR | PANEL_APPLET_EXPAND_MINOR | PANEL_APPLET_HAS_HANDLE);
	gtk_widget_set_name(GTK_WIDGET(applet), "globalmenu-applet-eventbox");
	panel_applet_set_background_widget(applet, applet);
	App = application_gnome_new(applet);
	g_signal_connect(G_OBJECT(applet), "change-background", 
				_change_background, App);
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
			"widget \"*.globalmenu-applet-eventbox\" style \"gmb_event_box_style\"\n"
			"\n");

	retval = panel_applet_factory_main (FACTORY_IID, PANEL_TYPE_APPLET, globalmenu_applet_factory, NULL);
	g_object_unref (program);
	return retval;
}
/*
vim:ts=4:sw=4
*/
