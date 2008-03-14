#include <config.h>
#include <panel-applet.h>
#include <panel-applet-gconf.h>
#include "application-gnome.h"
#include "log.h"

#include "intl.h"

#define APPLICATION_GNOME_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, TYPE_APPLICATION_GNOME, ApplicationGnomePrivate))

typedef struct {
	gint foo;
}ApplicationGnomePrivate;

static void _create_popup_menu(ApplicationGnome * self);

G_DEFINE_TYPE		(ApplicationGnome, application_gnome, TYPE_APPLICATION);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	Application * app;

	GObject * obj = ( *G_OBJECT_CLASS(application_gnome_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);
	app = APPLICATION(obj);
	panel_applet_add_preferences(PANEL_APPLET(app->window), "/schemas/apps/gnome-globalmenu-applet/prefs", NULL);
	_create_popup_menu(app);
	return obj;
}

static void _update_ui(Application *app)
{
	g_return_if_fail(IS_APPLICATION_GNOME(app));
	LOG("app-gnome:_update_ui, chain to parent class\n");
	APPLICATION_CLASS(application_gnome_parent_class)->update_ui(app);
}

static void _load_conf(Application *app)
{
	g_return_if_fail(IS_APPLICATION_GNOME(app));
	PangoFontDescription * font;
	gchar * font_name;
	font_name = panel_applet_gconf_get_string(PANEL_APPLET(app->window), "title_font", NULL);
	font = pango_font_description_from_string(font_name);
	g_free(font_name);
	g_object_set(app,
			"title-visible",
			 panel_applet_gconf_get_bool(PANEL_APPLET(app->window), "show_title", NULL),
			"icon-visible",
			panel_applet_gconf_get_bool(PANEL_APPLET(app->window), "show_icon", NULL),
			"title_font",
			font,
			NULL);
	g_boxed_free(PANGO_TYPE_FONT_DESCRIPTION, font);
}
static void _save_conf(Application *app)
{
	g_return_if_fail(IS_APPLICATION_GNOME(app));
	gboolean show_title, show_icon;
	PangoFontDescription * font= NULL;
	gchar * font_name;
	g_object_get(app, 
			"title-visible", &show_title,
			"icon-visible", &show_icon, 
			"title-font", &font, 
			NULL);
	panel_applet_gconf_set_bool(PANEL_APPLET(app->window), "show_title", show_title, NULL);
	panel_applet_gconf_set_bool(PANEL_APPLET(app->window), "show_icon", show_icon, NULL);
	font_name = pango_font_description_to_string(font);
	panel_applet_gconf_set_string(PANEL_APPLET(app->window), "title_font", font_name, NULL);
	if(font)
		g_boxed_free(PANGO_TYPE_FONT_DESCRIPTION, font);
	if(font_name)
		g_free(font_name);
}


static void application_gnome_class_init(ApplicationGnomeClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	ApplicationClass *app_class = APPLICATION_CLASS(klass);

	obj_class->constructor = _constructor;
//	obj_class->finalize = _finalize;

	app_class->update_ui = _update_ui;
	app_class->load_conf = _load_conf;
	app_class->save_conf = _save_conf;	

	g_type_class_add_private(obj_class, sizeof(ApplicationGnomePrivate));
}

static void application_gnome_init(ApplicationGnome *app_gnome)
{
}


Application *application_gnome_new(GtkWidget *w)
{
	return g_object_new(TYPE_APPLICATION_GNOME, "window", w, NULL);
}

static void _popup_menu(BonoboUIComponent * uic, Application * app, const gchar * cname){
	LOG("%s: cname = %s", __func__, cname);
	if(g_str_equal(cname, "About")) application_show_about_dialog(app);
	if(g_str_equal(cname, "Preference")) application_show_conf_dialog(app);
}

static void _create_popup_menu(ApplicationGnome * self){
	Application *app = APPLICATION(self);

	LOG("panel-window:%p\n", app->window);
	static const char t_toggle_menu_xml [] =
	"<popup name=\"button3\">\n"
		"<menuitem name=\"About\" "
		"          verb=\"About\" "
		"          label=\"%s\" "
		"          pixtype=\"stock\" "
		"          pixname=\"gtk-about\"/>\n"
		"<menuitem name=\"Preference\" "
		"          verb=\"Preference\" "
		"          label=\"%s\" "
		"          pixtype=\"stock\" "
		"          pixname=\"gtk-preferences\"/>\n"
   "</popup>\n";
	gchar * toggle_menu_xml = g_strdup_printf(t_toggle_menu_xml,
						_("_About"),
						_("_Preferences"));
	LOG("%s", toggle_menu_xml);
	BonoboUIVerb toggle_menu_verbs[] = {
		BONOBO_UI_VERB ("About", _popup_menu),
		BONOBO_UI_VERB ("Preference", _popup_menu),
		BONOBO_UI_VERB_END
	};
//	BonoboUIComponent* popup_component = 
//		panel_applet_get_popup_component(PANEL_APPLET(app->window));
	panel_applet_setup_menu(PANEL_APPLET(app->window), 
			toggle_menu_xml, 
			toggle_menu_verbs, 
			app);
	g_free(toggle_menu_xml);
}
