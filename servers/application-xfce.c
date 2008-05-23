#include <config.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include "application-xfce.h"
#include "log.h"

#include "intl.h"

#define APPLICATION_XFCE_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, TYPE_APPLICATION_XFCE, ApplicationXfcePrivate))

typedef struct {
	gint foo;
}ApplicationXfcePrivate;

G_DEFINE_TYPE		(ApplicationXfce, application_xfce, TYPE_APPLICATION);

static void _init_plugin(Application *app);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	Application * app;

	GObject * obj = ( *G_OBJECT_CLASS(application_xfce_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);
	app = APPLICATION(obj);
	_init_plugin(app);

	return obj;
}

static void _plugin_show_about(void *plugin, Application *app)
{
	application_show_about_dialog(app);
}
static void _plugin_show_configuration(void *plugin, Application *app)
{
	application_show_conf_dialog(app);
}

static gboolean _plugin_size_changed(GtkWidget* plugin, gint size, Application *app) {
	return TRUE;
}

static void _plugin_orientation_changed(GtkWidget *plugin, GtkOrientation *orientation, Application *app) {
	if (orientation) {
		g_object_set(app, 
			"orientation", *orientation == GTK_ORIENTATION_HORIZONTAL ? GNOMENU_ORIENT_TOP : GNOMENU_ORIENT_LEFT, 
			NULL);
	}
}

static void _init_plugin(Application *app)
{
	GtkOrientation orientation = xfce_panel_plugin_get_orientation(XFCE_PANEL_PLUGIN(app->window));
	_plugin_orientation_changed(GTK_WIDGET(app->window), &orientation, app);

	xfce_panel_plugin_menu_show_about(XFCE_PANEL_PLUGIN(app->window));
	xfce_panel_plugin_menu_show_configure(XFCE_PANEL_PLUGIN(app->window));
	xfce_panel_plugin_add_action_widget(XFCE_PANEL_PLUGIN(app->window), APPLICATION_XFCE(app)->event_box);

	g_signal_connect(G_OBJECT(app->window), "size-changed", 
		G_CALLBACK(_plugin_size_changed), app);
	g_signal_connect(G_OBJECT(app->window), "about",
			G_CALLBACK(_plugin_show_about), app);
	g_signal_connect(G_OBJECT(app->window), "configure-plugin",
			G_CALLBACK(_plugin_show_configuration), app);
	g_signal_connect(G_OBJECT(app->window), "orientation-changed",
			G_CALLBACK(_plugin_orientation_changed), app);
	/* defaults for settings */
	app->title_font = pango_font_description_from_string("Verdana Bold 9");
	app->title_max_width = 12;
}

static void _update_ui(Application *app)
{
  ApplicationXfce* xapp = APPLICATION_XFCE(app);
	if (gtk_widget_get_parent(GTK_WIDGET(app->box)) == GTK_WIDGET(app->window)) {
		gtk_container_remove(GTK_CONTAINER(app->window), GTK_WIDGET(app->box));
		if (!gtk_widget_get_parent(xapp->event_box)) {
			gtk_container_add(GTK_CONTAINER(app->window), xapp->event_box);
			gtk_widget_show(xapp->event_box);
		}
		gtk_container_add(GTK_CONTAINER(xapp->event_box), GTK_WIDGET(app->box));
	}
	g_return_if_fail(IS_APPLICATION_XFCE(app));
	gtk_widget_set_size_request(GTK_WIDGET(app->window), 512, 16);
	LOG("app-xfce:_update_ui, chain to parent class\n");
	APPLICATION_CLASS(application_xfce_parent_class)->update_ui(app);
}

static void _load_conf(Application *app)
{
	g_return_if_fail(IS_APPLICATION_XFCE(app));

	gchar * config_name = xfce_panel_plugin_lookup_rc_file(XFCE_PANEL_PLUGIN(app->window));
	if (config_name) {
		XfceRc * rc = xfce_rc_simple_open(config_name, TRUE);
		if (rc) {
			g_object_set(app,
					"title-visible",
					xfce_rc_read_bool_entry(rc, "show_title", TRUE),
					"icon-visible",
					xfce_rc_read_bool_entry(rc, "show_icon", TRUE),
					NULL);
			xfce_rc_close(rc);
		}
		g_free(config_name);
	}
}
static void _save_conf(Application *app)
{
	g_return_if_fail(IS_APPLICATION_XFCE(app));
	gboolean show_title, show_icon;
	g_object_get(app, 
			"title-visible", &show_title,
			"icon-visible", &show_icon, NULL);

	gchar *config_name = xfce_panel_plugin_save_location(XFCE_PANEL_PLUGIN(app->window), TRUE);
	if (config_name) {
		XfceRc* rc = xfce_rc_simple_open(config_name, FALSE);
		if (rc) {
			xfce_rc_write_bool_entry(rc, "show_title", show_title);
			xfce_rc_write_bool_entry(rc, "show_icon", show_icon);
			xfce_rc_close(rc);
		}
		g_free(config_name);
	}
}


static void application_xfce_class_init(ApplicationXfceClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	ApplicationClass *app_class = APPLICATION_CLASS(klass);

	obj_class->constructor = _constructor;
//	obj_class->finalize = _finalize;

	app_class->update_ui = _update_ui;
	app_class->load_conf = _load_conf;
	app_class->save_conf = _save_conf;	

	g_type_class_add_private(obj_class, sizeof(ApplicationXfcePrivate));
}

static void application_xfce_init(ApplicationXfce *app)
{
	app->event_box = gtk_event_box_new();
}

Application *application_xfce_new(GtkWidget *w)
{
	return g_object_new(TYPE_APPLICATION_XFCE, "window", w, NULL);
}

