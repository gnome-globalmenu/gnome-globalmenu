#include <panel-applet.h>
#include <panel-applet-gconf.h>
#include "application-gnome.h"
#include "log.h"

#define APPLICATION_GNOME_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, TYPE_APPLICATION_GNOME, ApplicationGnomePrivate))

typedef struct {
	Application * App;
	GtkDialog * dlg;
	GtkCheckButton * show_title;
	GtkCheckButton * show_icon;
}ApplicationGnomePrivate;

static void _create_popup_menu(ApplicationGnome * self);

G_DEFINE_TYPE		(ApplicationGnome, application_gnome, TYPE_APPLICATION);

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	ApplicationGnome *self;

	GObject * _self = ( *G_OBJECT_CLASS(application_gnome_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);
	self = APPLICATION_GNOME(_self);
	_create_popup_menu(self);
	return _self;
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

	panel_applet_add_preferences(PANEL_APPLET(app->window), "/app/gnome2-globalmenu-applet", NULL);
	g_object_set(app,
			"title-visible",
			 panel_applet_gconf_get_bool(PANEL_APPLET(app->window), "show_title", NULL),
			"icon-visible",
			panel_applet_gconf_get_bool(PANEL_APPLET(app->window), "show_icon", NULL),
			NULL);
}
static void _save_conf(Application *app)
{
	g_return_if_fail(IS_APPLICATION_GNOME(app));
	gboolean show_title, show_icon;
/*	panel_applet_add_preferences(PANEL_APPLET(app->window), "/app/gnome2-globalmenu-applet", NULL);
 *	FIXME: need this?*/
	g_object_get(app, 
			"title-visible", &show_title,
			"icon-visible", &show_icon, NULL);
	panel_applet_gconf_set_bool(PANEL_APPLET(app->window), "show_title", show_title, NULL);
	panel_applet_gconf_set_bool(PANEL_APPLET(app->window), "show_icon", show_icon, NULL);
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

static void application_gnome_init(ApplicationGnome *obj)
{
}


Application *application_gnome_new(GtkWidget *w)
{
	return g_object_new(TYPE_APPLICATION_GNOME, "window", w, NULL);
}

static void _dlg_cb(GtkDialog * nouse, gint arg, ApplicationGnome* self){
	ApplicationGnomePrivate *priv = APPLICATION_GNOME_GET_PRIVATE(self);
	Application *app = APPLICATION(self);

	switch(arg){
		case GTK_RESPONSE_ACCEPT: 
			LOG("Preference Accepted");
		g_object_set(app,
				"title-visible",
				gtk_toggle_button_get_active(priv->show_title),
				"icon-visible",
				gtk_toggle_button_get_active(priv->show_icon),
				NULL);
		
			application_save_conf(self);
			application_load_conf(self);
			application_update_ui(self);
			break;
		default:
			LOG("What Response is it?");
	}
	gtk_widget_destroy(GTK_WIDGET(priv->dlg));
}


void _show_dialog(ApplicationGnome * self){
	ApplicationGnomePrivate *priv = APPLICATION_GNOME_GET_PRIVATE(self);
	Application *app = APPLICATION(self);
	gboolean show_title, show_icon;

	GtkBox * vbox = GTK_BOX(gtk_vbox_new(TRUE, 0));
	GtkWidget * show = gtk_label_new(_("Display following elements"));
//	GtkWidget * title_label = gtk_label_new(_("Maximium Title Label Width(in chars)"));
//	GtkBox * title_box = GTK_BOX(gtk_hbox_new(TRUE, 0));
	priv->dlg= GTK_DIALOG(gtk_dialog_new());
	priv->show_title = GTK_CHECK_BUTTON(gtk_check_button_new_with_label (_("Active Window Title")));
	priv->show_icon = GTK_CHECK_BUTTON(gtk_check_button_new_with_label (_("Active Window Icon")));


	g_object_get(app, 
			"title-visible", &show_title,
			"icon-visible", &show_icon, NULL);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->show_title), show_title);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->show_icon), show_icon);
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(show));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(priv->show_title));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(priv->show_icon));
//	gtk_box_pack_start_defaults(title_box, GTK_WIDGET(title_label));
//	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(title_box));
	gtk_container_add(GTK_CONTAINER(priv->dlg->vbox), GTK_WIDGET(vbox));

	gtk_dialog_add_button(priv->dlg, GTK_STOCK_APPLY, GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(priv->dlg, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);
	
	g_signal_connect(G_OBJECT(priv->dlg), "response", G_CALLBACK(_dlg_cb), self);
	gtk_widget_show_all(GTK_WIDGET(priv->dlg));
}


void _show_about(ApplicationGnome * self){
	gchar * authors[] = {
		"Yu Feng <rainwoodman@gmail.com>",
		"Mingxi Wu <fengshenx.@gmail.com>",
		"And thanks to others for the discussion",
		NULL
		};
	gtk_show_about_dialog(NULL, 
				"authors", authors, NULL);
}

static void _popup_menu(BonoboUIComponent * uic, gpointer user_data, const gchar * cname){
	ApplicationGnome* app_gnome = APPLICATION_GNOME(user_data);
	LOG("%s: cname = %s", __func__, cname);
	if(g_str_equal(cname, "About")) _show_about(app_gnome);
	if(g_str_equal(cname, "Preference")) _show_dialog(app_gnome);
}

static void _create_popup_menu(ApplicationGnome * self){
	Application *app = APPLICATION(self);

	LOG("panel-window:%p\n", app->window);
	static const char toggle_menu_xml [] =
	"<popup name=\"button3\">\n"
		"<menuitem name=\"About\" "
		"          verb=\"About\" "
		"          _label=\"_About\" "
		"          pixtype=\"stock\" "
		"          pixname=\"gtk-about\"/>\n"
		"<menuitem name=\"Preference\" "
		"          verb=\"Preference\" "
		"          _label=\"_Preference\" "
		"          pixtype=\"stock\" "
		"          pixname=\"gtk-preferences\"/>\n"
   "</popup>\n";
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
}
