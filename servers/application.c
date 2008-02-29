#include <gtk/gtk.h>
#include <panel-applet.h>
#include "application.h"
#include "menuserver.h"
#include "preference-gnome.h"
#include "log.h"

enum {
	PROP_0,
	PROP_WINDOW
};

static void _s_window_destroy(Application * app, GtkWidget * widget);
static void _s_active_client_changed(Application * app, MenuServer * server);
static void _set_background(GtkWidget * widget, GdkColor * color, GdkPixmap * pixmap);

static GObject *_constructor( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) ;



G_DEFINE_TYPE		(Application, application, G_TYPE_OBJECT);

static void _update_ui_unimp(Application *app)
{
	g_warning("Application:: update_ui isn't implemented for %s\n", 
			g_type_name(G_TYPE_FROM_INSTANCE(app)));
}

static void _load_conf_unimp(Application *app)
{
	g_warning("Application:: _load_conf isn't implemented for %s\n", 
			g_type_name(G_TYPE_FROM_INSTANCE(app)));
}


static void 
_set_property( GObject * _self, guint property_id, const GValue * value, GParamSpec * pspec){

}


static void 
_get_property( GObject * _self, guint property_id, GValue * value, GParamSpec * pspec){

}


static void application_init(Application *app)
{
}

static void _finalize(GObject *obj)
{
	Application *app = APPLICATION(obj);
	menu_server_destroy(app->server);
}

static void application_class_init(ApplicationClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	
	obj_class->constructor = _constructor;
	obj_class->finalize = _finalize;
	obj_class->set_property = _set_property;
	obj_class->get_property = _get_property;

	klass->update_ui = _update_ui_unimp;
	klass->load_conf = _load_conf_unimp;


	g_object_class_install_property (obj_class,
		PROP_WINDOW,
		g_param_spec_object ("window",
						"window",
						"applet window",
						GTK_TYPE_WIDGET,
						G_PARAM_CONSTRUCT_ONLY));
}

static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	Application *app;

	GObject * _self = ( *G_OBJECT_CLASS(application_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);

	app = APPLICATION(_self);
	GtkWidget* box = gtk_hbox_new(FALSE, 0); 
	/*This thing is ugly, (consider a vertical menu layout), we need a new alignment widget
 * 	which is similiar to GtkMenuBar(respecting directions) */

	app->menu_bar_area = GTK_FIXED(gtk_fixed_new());
	gtk_fixed_set_has_window(app->menu_bar_area, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(app->menu_bar_area), 0);
	app->title = GTK_LABEL(gtk_label_new(""));
	app->icon = GTK_IMAGE(gtk_image_new());

	gtk_box_pack_start_defaults(GTK_BOX(box), GTK_WIDGET(app->icon));
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->title), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->menu_bar_area), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(app->window), GTK_WIDGET(box));
	app->server = menu_server_new(GTK_WIDGET(app->menu_bar_area));

	preference_load_conf(app);
	update_ui(app);

	g_signal_connect_swapped(G_OBJECT(app->window), 
		"destroy",
        G_CALLBACK(_s_window_destroy), app);
	g_signal_connect_swapped(G_OBJECT(app->server),
		"active-client-changed",
		G_CALLBACK(_s_active_client_changed), app);

	return _self;
}

void application_set_background(Application * app, GdkColor * color, GdkPixmap * pixmap){
	GdkPixmap * cropped;
	GdkGC * gc;
	GtkAllocation * a;
	a = &GTK_WIDGET(app->menu_bar_area)->allocation;
	cropped = gdk_pixmap_new(pixmap, a->width, a->height, -1);
	gc = gdk_gc_new(pixmap);
	gdk_draw_drawable(cropped, gc, pixmap, a->x, a->y, 0, 0, a->width, a->height);

	g_object_set(app->server, "bg-color", color, "bg-pixmap", cropped, NULL);
	_set_background(GTK_WIDGET(app->menu_bar_area), color, cropped);	
	g_object_unref(gc);
	g_object_unref(cropped);
}


static void _s_active_client_changed(Application * app, MenuServer * server){
	GdkPixbuf *icon_buf;
	WnckWindow * window = menu_server_get_client_parent(server, server->active);
	if(!window) window = wnck_screen_get_active_window(wnck_screen_get_default());
	WnckApplication * application = wnck_window_get_application(window);

	const gchar *name = wnck_application_get_name(application);
	gtk_label_set_text(app->title, name);

	icon_buf = wnck_application_get_icon(application);
	gtk_image_set_from_pixbuf(app->icon, icon_buf);
	update_ui(app);
}

static void _s_window_destroy(Application * app, GtkWidget * widget){
	LOG();
	
	g_object_unref(G_OBJECT(app));
}
static void _set_background(GtkWidget * widget, GdkColor * color, GdkPixmap * pixmap){
	GtkRcStyle * rc_style;
	GtkStyle * style;
	gtk_widget_set_style (widget, NULL);
	rc_style = gtk_rc_style_new ();
	gtk_widget_modify_style (widget, rc_style);
	gtk_rc_style_unref (rc_style);
	if(color){
		LOG("new bg color %d, %d, %d", color->red, color->green, color->blue);
		gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, color);
	}
	if(pixmap){
		gint w, h;
		gdk_drawable_get_size(pixmap, &w, &h);
		LOG("not implemented for pixmap bg yet");
		LOG("size of pixmap, %d, %d", w, h);

		style = gtk_style_copy (widget->style);
		if (style->bg_pixmap[GTK_STATE_NORMAL])
			g_object_unref (style->bg_pixmap[GTK_STATE_NORMAL]);
		style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref (pixmap);
		gtk_widget_set_style (widget, style);
		g_object_unref (style);

	}
}


void update_ui(Application  *app)
{
	if(app->show_title) 
		gtk_widget_show(app->title);
	else gtk_widget_hide(app->title);

	if(app->show_icon)
		gtk_widget_show(app->icon);
	else gtk_widget_hide(app->icon);
}


static void popup_menu_cb(BonoboUIComponent * uic, Application *App, gchar *cname)
{
	g_message("%s: cname = %s", __func__, cname);
	if(g_str_equal(cname, "About")) ui_show_about(NULL, App);
	if(g_str_equal(cname, "Preference")) preference_show_dialog(NULL, App);
}

/*
vim:ts=4:sw=4
*/
