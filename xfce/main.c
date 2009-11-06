#include <gtk/gtk.h>
#include <globalmenu-server.h>

#include <libxfce4panel/xfce-panel-plugin.h>

static void xfce_panel_plugin_init (XfcePanelPlugin * plugin);

XFCE_PANEL_PLUGIN_REGISTER_EXTERNAL ( xfce_panel_plugin_init);

static void null_log_handler(const gchar * domain, GLogLevelFlags level, const gchar* message, gpointer userdata) {
	return;
}
static void xfce_panel_plugin_init (XfcePanelPlugin * plugin) {
	g_log_set_handler("libgnomenu", G_LOG_LEVEL_DEBUG, null_log_handler, NULL);

	gtk_rc_parse_string(
		"style \"globalmenu_event_box_style\"\n"
		"{\n"
		"	GtkWidget::focus-line-width=0\n"
		"	GtkWidget::focus-padding=0\n"
		"}\n"
		"style \"globalmenu_menu_bar_style\"\n"
		"{\n"
		"	ythickness = 0\n"
		"	GtkMenuBar::shadow-type = none\n"
		"	GtkMenuBar::internal-padding = 0\n"
		"}\n"
		"class \"GtkPlug\" style \"globalmenu_event_box_style\"\n"
		"class \"GnomenuMenuBar\" style:highest \"globalmenu_menu_bar_style\"\n"
	);
	gtk_widget_reset_rc_styles(plugin);
	GnomenuGlobalMenuBar * menubar = gnomenu_global_menu_bar_new();
	g_object_ref_sink(menubar);

	xfce_panel_plugin_add_action_widget(plugin, GTK_WIDGET(plugin));
	xfce_panel_plugin_set_expand(plugin, TRUE);

	xfce_panel_plugin_add_action_widget(plugin, GTK_WIDGET(menubar));

	gtk_container_add(GTK_CONTAINER(plugin), GTK_WIDGET(menubar));
	gtk_widget_show(menubar);

}

