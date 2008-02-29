#include <gtk/gtk.h>
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
static void _set_widget_background(GtkWidget * widget, GdkColor * color, GdkPixmap * pixmap);
static void _s_menu_bar_area_size_allocate(Application * app, GtkAllocation * allocation, GtkWidget * widget);
static void _update_background(Application * app);

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

	Application *self = APPLICATION(_self);
	switch (property_id){
		case PROP_WINDOW:
			if(GTK_IS_WIDGET(self->window)) g_object_unref(self->window);
			self->window = g_value_get_object(value);
			g_assert(!GTK_WIDGET_NO_WINDOW(self->window));
			g_object_ref(self->window);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}
}


static void 
_get_property( GObject * _self, guint property_id, GValue * value, GParamSpec * pspec){

	Application *self = APPLICATION(_self);
	switch (property_id){
		case PROP_WINDOW:
			g_value_set_object(value, self->window);
			break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}

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
						G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
}
Application * application_new(GtkContainer * widget){
	return g_object_new(TYPE_APPLICATION, "window", widget, NULL);
}
static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	Application *app;
	ApplicationClass *app_class;

	GObject * _self = ( *G_OBJECT_CLASS(application_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);

	app = APPLICATION(_self);
	app_class = G_OBJECT_GET_CLASS(app);
	GtkWidget* box = gtk_hbox_new(FALSE, 0); 
	/*This thing is ugly, (consider a vertical menu layout), we need a new alignment widget
 * 	which is similiar to GtkMenuBar(respecting directions) */

	app->menu_bar_area = GTK_FIXED(gtk_fixed_new());
	gtk_fixed_set_has_window(app->menu_bar_area, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(app->menu_bar_area), 0);
	app->title = GTK_LABEL(gtk_label_new(""));
	app->icon = GTK_IMAGE(gtk_image_new());
	app->bgpixmap = NULL;
	app->bgcolor = NULL;

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->icon), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->title), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->menu_bar_area), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(app->window), GTK_WIDGET(box));
	app->server = menu_server_new(GTK_WIDGET(app->menu_bar_area));

	(*app_class->load_conf)(app);
	(*app_class->update_ui)(app);

	g_signal_connect_swapped(G_OBJECT(app->window), 
		"destroy",
        G_CALLBACK(_s_window_destroy), app);
	g_signal_connect_swapped(G_OBJECT(app->server),
		"active-client-changed",
		G_CALLBACK(_s_active_client_changed), app);
	g_signal_connect_swapped(G_OBJECT(app->menu_bar_area),
		"size-allocate",
		G_CALLBACK(_s_menu_bar_area_size_allocate), app);

	return _self;
}

void application_set_background(Application * app, GdkColor * color, GdkPixmap * pixmap){

	if(app->bgpixmap) g_object_unref(app->bgpixmap);
	app->bgpixmap = g_object_ref(pixmap);
	if(app->bgcolor) g_object_unref(app->bgcolor);
	app->bgcolor = g_object_ref(color);
	_update_background(app);
}

static void _update_background(Application * app){
	GdkPixmap * cropped;
	GdkGC * gc;
	GtkAllocation * a;
	GdkPixmap * pixmap = app->bgpixmap;
	GdkColor * color = app->bgcolor;

	a = &GTK_WIDGET(app->menu_bar_area)->allocation;
	cropped = gdk_pixmap_new(pixmap, a->width, a->height, -1);
	gc = gdk_gc_new(pixmap);
	gdk_draw_drawable(cropped, gc, pixmap, a->x, a->y, 0, 0, a->width, a->height);

	g_object_set(app->server, "bg-color", color, "bg-pixmap", cropped, NULL);
	_set_widget_background(app->menu_bar_area, color, cropped);	

	g_object_unref(gc);
	g_object_unref(cropped);
}


static void _s_active_client_changed(Application * app, MenuServer * server){
	ApplicationClass *app_class = G_OBJECT_GET_CLASS(app);
	GdkPixbuf *icon_buf;
	WnckWindow * window = menu_server_get_client_parent(server, server->active);
	if(!window) window = wnck_screen_get_active_window(wnck_screen_get_default());
	WnckApplication * application = wnck_window_get_application(window);

	const gchar *name = wnck_application_get_name(application);
	gtk_label_set_text(app->title, name);

	icon_buf = wnck_application_get_icon(application);
	gtk_image_set_from_pixbuf(app->icon, icon_buf);
	(*app_class->update_ui)(app);
}

static void _s_window_destroy(Application * app, GtkWidget * widget){
	LOG();
	g_object_unref(G_OBJECT(app));
}
static void _set_widget_background(GtkWidget * widget, GdkColor * color, GdkPixmap * pixmap){
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
static void _s_menu_bar_area_size_allocate(Application * app, GtkAllocation * allocation, GtkWidget * widget){
	static GtkAllocation old_allo = {0};
	LOG("+%d,%d,%d,%d", *allocation);
	LOG("-%d,%d,%d,%d", old_allo);
	if(memcmp(&old_allo, allocation, sizeof(GtkAllocation)))
		_update_background(app);	
	old_allo = *allocation;
}


/*
vim:ts=4:sw=4
*/
