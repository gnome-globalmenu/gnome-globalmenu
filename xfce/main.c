#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>

static void xfce_panel_plugin_init (XfcePanelPlugin * plugin);

XFCE_PANEL_PLUGIN_REGISTER_EXTERNAL ( xfce_panel_plugin_init);

static void xfce_panel_plugin_init (XfcePanelPlugin * plugin) {
	xfce_panel_plugin_add_action_widget(plugin, GTK_WIDGET(plugin));
	gtk_widget_show(GTK_WIDGET(plugin));
}
