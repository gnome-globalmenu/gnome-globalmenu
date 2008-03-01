#include <config.h>
#include <gtk/gtk.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "application.h"
//workaround a weird bug in xfce4 includes
#undef _
#undef Q_

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

XFCE_PANEL_PLUGIN_REGISTER_EXTERNAL(xfce_applet_construct)

/*
vim:ts=4:sw=4
*/
