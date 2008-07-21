#include <config.h>
#include <gtk/gtk.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "application-xfce.h"
//workaround a weird bug in xfce4 includes
#undef _
#undef Q_

/*
 * Standard gettext macros.
 */
#include "intl.h"


static void
xfce_applet_construct(XfcePanelPlugin *plugin)
{
	g_print("constructing plugin\n");
//	app = application_xfce_new(GTK_WIDGET(plugin));
	gtk_widget_show_all(GTK_WIDGET(plugin));
	//application_start(app);
}

XFCE_PANEL_PLUGIN_REGISTER_EXTERNAL(xfce_applet_construct)

/*
vim:ts=4:sw=4
*/
