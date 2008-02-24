#include <config.h>

#include <gtk/gtk.h>


#include <panel-applet.h>
#include "libgnomenu/serverhelper.h"

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

#include "application.h"

#include "log.h"

#define FACTORY_IID "OAFIID:GNOME_GlobalMenuApplet_Factory"
#define APPLET_IID "OAFIID:GNOME_GlobalMenuApplet"

#define APP_NAME "gnome-globalmenu-applet"
#define APP_VERSION "4"

static gboolean globalmenu_applet_factory (PanelApplet *applet,
                                        const gchar *iid,
                                        gpointer data){
	Application * App;
  if (g_str_equal(iid, APPLET_IID)){
	panel_applet_set_flags(applet, 
		PANEL_APPLET_EXPAND_MAJOR | PANEL_APPLET_EXPAND_MINOR | PANEL_APPLET_HAS_HANDLE);
	//gtk_widget_set_name(GTK_WIDGET(applet), "globalmenu-applet-eventbox");
	GtkFixed * fixed = gtk_fixed_new();
	gtk_container_add(applet, fixed);
	gtk_fixed_set_has_window(fixed, TRUE);
	application_new(fixed);
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
