#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "application.h"
#include "menuserver.h"
#include "utils.h"
#include "log.h"

enum {
	PROP_0,
	PROP_WINDOW,
	PROP_TITLE_VISIBLE,
	PROP_ICON_VISIBLE
};

typedef struct _ApplicationPrivate {
	gint foo;
} ApplicationPrivate;

#define APPLICATION_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, TYPE_APPLICATION, ApplicationPrivate))

#define GET_OBJECT(src, obj, priv) \
	Application * obj = APPLICATION(src); \
	ApplicationPrivate * priv = APPLICATION_GET_PRIVATE(src);

static void _s_window_destroy(Application * app, GtkWidget * widget);
static void _s_active_client_changed(Application * app, MenuServer * server);
static void _s_menu_bar_area_size_allocate(Application * app, GtkAllocation * allocation, GtkWidget * widget);
static void _s_conf_dialog_response(Application * app, gint arg, GtkWidget * dialog);

/* Application Interface */
static void _update_ui					(Application *app);
static void _save_conf_unimp			(Application *app);
static void _load_conf_unimp			(Application *app);

/* GObject Interface */
static void _set_property				( GObject * obj, guint property_id, 
										  const GValue * value, GParamSpec * pspec);
static void _get_property				( GObject * obj, guint property_id, 
										  GValue * value, GParamSpec * pspec);
static void _finalize					( GObject *obj);
static GObject *_constructor			( GType type, guint n_construct_properties,
										  GObjectConstructParam * construct_params) ;

/* tool function*/
static void _update_background			(Application * app);


G_DEFINE_TYPE		(Application, application, G_TYPE_OBJECT);

static void application_init(Application *app)
{
}
static void application_class_init(ApplicationClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	
	obj_class->constructor = _constructor;
	obj_class->finalize = _finalize;
	obj_class->set_property = _set_property;
	obj_class->get_property = _get_property;

	klass->update_ui = _update_ui;
	klass->load_conf = _load_conf_unimp;
	klass->save_conf = _save_conf_unimp;

	g_object_class_install_property (obj_class,
		PROP_WINDOW,
		g_param_spec_object ("window",
						"window",
						"applet window",
						GTK_TYPE_WIDGET,
						G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

	g_object_class_install_property (obj_class,
		PROP_TITLE_VISIBLE,
		g_param_spec_boolean ("title-visible",
						"title-visible",
						"whether or not display the title",
						FALSE,
						G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

	g_object_class_install_property (obj_class,
		PROP_ICON_VISIBLE,
		g_param_spec_boolean ("icon-visible",
						"icon-visible",
						"whether or not display the title",
						FALSE,
						G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

	g_type_class_add_private(obj_class, sizeof(ApplicationPrivate));
}

Application * application_new(GtkContainer * widget){
	return g_object_new(TYPE_APPLICATION, "window", widget, NULL);
}

/* BEGINS: Application Interface */
static void _update_ui(Application *app)
{
	LOG();
	if(app->title_visible) 
		gtk_widget_show(app->title);
	else 
		gtk_widget_hide(app->title);

	if(app->icon_visible)
		gtk_widget_show(app->icon);
	else 
		gtk_widget_hide(app->icon);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->conf_dialog.title_visible), app->title_visible);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->conf_dialog.icon_visible), app->icon_visible);
}
static void _save_conf_unimp(Application *app){
	LOG("Not implemented for %s\n", 
			g_type_name(G_TYPE_FROM_INSTANCE(app)));

}
static void _load_conf_unimp(Application *app)
{
	LOG("Not implemented for %s\n", 
			g_type_name(G_TYPE_FROM_INSTANCE(app)));
}
/* ENDS: Application Interface */

/* BEGINS: GObject Interface */
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
		case PROP_TITLE_VISIBLE:
			self->title_visible = g_value_get_boolean(value);
//			application_update_ui(self);
//			application_save_conf(self);
			break;
		case PROP_ICON_VISIBLE:
			self->icon_visible = g_value_get_boolean(value);
//			application_update_ui(self);
//			application_save_conf(self);
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
		case PROP_TITLE_VISIBLE:
			g_value_set_boolean(value, self->title_visible);
			break;
		case PROP_ICON_VISIBLE:
			g_value_set_boolean(value, self->icon_visible);
			break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}

}


static void _finalize(GObject *obj)
{
	Application *app = APPLICATION(obj);
	if (app->window)
		g_object_unref(app->window);
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

	app->title = GTK_LABEL(gtk_label_new(""));
	app->icon = GTK_IMAGE(gtk_image_new());
	app->bgpixmap = NULL;
	app->bgcolor = NULL;

	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->icon), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->title), FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(app->window), GTK_WIDGET(box));

/* conf dialog */
	app->conf_dialog.dlg = GTK_DIALOG(gtk_dialog_new());
	GtkBox * vbox = GTK_BOX(gtk_vbox_new(TRUE, 0));
//	GtkWidget * title_label = gtk_label_new(_("Maximium Title Label Width(in chars)"));
//	GtkBox * title_box = GTK_BOX(gtk_hbox_new(TRUE, 0));
	#define NEW_CHECK_BUTTON(n, t) \
		app->conf_dialog.n = gtk_check_button_new_with_label(t); \
		gtk_box_pack_start_defaults(vbox, app->conf_dialog.n);
	NEW_CHECK_BUTTON(title_visible, "Show active application title");
	NEW_CHECK_BUTTON(icon_visible, "Show active window icon");
	#undef NEW_CHECK_BUTTON 
//	gtk_box_pack_start_defaults(title_box, GTK_WIDGET(title_label));
//	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(title_box));
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(app->conf_dialog.dlg)->vbox), GTK_WIDGET(vbox));

	gtk_dialog_add_button(app->conf_dialog.dlg, GTK_STOCK_APPLY, GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(app->conf_dialog.dlg, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);
	

/* start server */
	app->server = menu_server_new();
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(app->server), TRUE, TRUE, 0);

//	application_load_conf(app);
//	application_update_ui(app);

	g_signal_connect_swapped(G_OBJECT(app->conf_dialog.dlg), 
		"response", 
		G_CALLBACK(_s_conf_dialog_response), app);
	g_signal_connect_swapped(G_OBJECT(app->window), 
		"destroy",
        G_CALLBACK(_s_window_destroy), app);
	g_signal_connect_swapped(G_OBJECT(app->server),
		"active-client-changed",
		G_CALLBACK(_s_active_client_changed), app);
	g_signal_connect_swapped(G_OBJECT(app->server),
		"size-allocate",
		G_CALLBACK(_s_menu_bar_area_size_allocate), app);

	_update_background(app);

	return _self;
}
/* ENDS: GObject Interface */

/* Public methods */
void application_set_background(Application * app, GdkColor * color, GdkPixmap * pixmap){
	gboolean dirty = FALSE;
/* This piece code is redundant, for the purpose of clearity*/
/* pixmap */	
	if(pixmap == NULL){  /* clear the pixmap */
		if(app->bgpixmap){
			g_object_unref(app->bgpixmap);
			app->bgpixmap = NULL;
			dirty = TRUE;
		}
	} else
	if(app->bgpixmap == pixmap) {
		/* Do nothing*/
	} else { /* new pixmap is not current pixmap */
		if(app->bgpixmap)
			g_object_unref(app->bgpixmap);
		app->bgpixmap = g_object_ref(pixmap);
		dirty = TRUE;
	}
/* color */
	if(color == NULL){ /* clear the color */
		if(app->bgcolor){
			g_boxed_free(GDK_TYPE_COLOR, app->bgcolor);
			app->bgcolor = NULL;
			dirty = TRUE;
		}
	} else 
	if (app->bgcolor == color) {
		/* do nothing*/
	} else { /* set the new color */
		if(app->bgcolor)
			g_boxed_free(GDK_TYPE_COLOR, app->bgcolor);
		app->bgcolor = g_boxed_copy(GDK_TYPE_COLOR, color);
		dirty = TRUE;
	}
/* update background*/
	if(dirty)
		_update_background(app);
}

void application_update_ui(Application *app){
	if(APPLICATION_GET_CLASS(app)->update_ui){
		APPLICATION_GET_CLASS(app)->update_ui(app);
	} else {
		LOG("not implemented in subclass");
	}
}
void application_load_conf(Application *app){
	if(APPLICATION_GET_CLASS(app)->load_conf){
		APPLICATION_GET_CLASS(app)->load_conf(app);
	} else {
		LOG("not implemented in subclass");
	}
}
void application_save_conf(Application *app){
	if(APPLICATION_GET_CLASS(app)->save_conf){
		APPLICATION_GET_CLASS(app)->save_conf(app);
	} else {
		LOG("not implemented in subclass");
	}
}

void application_show_conf_dialog(Application *app){
	gtk_widget_show_all(GTK_WIDGET(app->conf_dialog.dlg));
}

void application_show_about_dialog(Application * app){
	gchar * authors[] = {
		"Yu Feng <rainwoodman@gmail.com>",
		"Mingxi Wu <fengshenx.@gmail.com>",
		"And thanks to others for the discussion",
		NULL
		};
	gtk_show_about_dialog(NULL, 
				"authors", authors, NULL);
}
/* END: Public Methods */

/* BEGIN: Signal handlers */
static void _s_active_client_changed(Application * app, MenuServer * server){
	ApplicationClass *app_class = G_OBJECT_GET_CLASS(app);
	GdkPixbuf *icon_buf, *resized_icon;
	gint w, h;
	WnckWindow * window = menu_server_get_client_parent(server, server->active);

	if(!window) window = wnck_screen_get_active_window(wnck_screen_get_default());
	if(!window) return;
	WnckApplication * application = wnck_window_get_application(window);

	const gchar *name = wnck_application_get_name(application);
	gtk_label_set_text(app->title, name);

	icon_buf =  wnck_application_get_icon(application);
	if (icon_buf) {
		/* FIXME : check the direction fisrt? */
		w = h = GTK_WIDGET(app->window)->allocation.height ;
		resized_icon = gdk_pixbuf_scale_simple(icon_buf , w, h, GDK_INTERP_BILINEAR);
		gtk_image_set_from_pixbuf(app->icon, resized_icon);
		g_object_unref(resized_icon);
	}
	application_update_ui(app);
}

static void _s_window_destroy(Application * app, GtkWidget * widget){
	LOG();
	g_object_unref(G_OBJECT(app));
}
static void _s_menu_bar_area_size_allocate(Application * app, GtkAllocation * allocation, GtkWidget * widget){
	static GtkAllocation old_allo = {0};
	LOG("+%d,%d,%d,%d", *allocation);
	LOG("-%d,%d,%d,%d", old_allo);
	if(memcmp(&old_allo, allocation, sizeof(GtkAllocation)))
		_update_background(app);	
	old_allo = *allocation;
}
static void _s_conf_dialog_response(Application * self, gint arg, GtkWidget * dialog){
	Application * app = APPLICATION(self);

	switch(arg){
		case GTK_RESPONSE_ACCEPT: 
			LOG("Preference Accepted");
			g_object_set(app,
				"title-visible",
				gtk_toggle_button_get_active(app->conf_dialog.title_visible),
				"icon-visible",
				gtk_toggle_button_get_active(app->conf_dialog.icon_visible),
				NULL);
			application_save_conf(self);
			application_load_conf(self);
			application_update_ui(self);
			break;
		default:
			LOG("What Response is it?");
	}
	gtk_widget_hide(dialog);
}
/* END: Signal handlers */

/* BEGIN: tool functions*/
static void _update_background(Application * app){
	GdkPixmap * cropped = NULL;  /*clear bg by default*/
	GdkGC * gc;
	GtkAllocation * a;
	GdkPixmap * pixmap = app->bgpixmap;
	GdkColor * color = app->bgcolor;
	a = &GTK_WIDGET(app->server)->allocation;
	LOG();
	if (pixmap) { /* get the cropped pixmap for menu bar area*/
		cropped = gdk_pixmap_new(pixmap, a->width, a->height, -1);
		gc = gdk_gc_new(pixmap);
		gdk_draw_drawable(cropped, gc, pixmap, a->x, a->y, 0, 0, a->width, a->height);
		g_object_unref(gc);
	} 

	g_object_set(app->server, "bg-color", color, 
							"bg-pixmap", cropped, NULL);
	if(cropped);
		g_object_unref(cropped);
	utils_set_widget_background(app->server, color, cropped);	
}
/* END: tool functions*/
/*
vim:ts=4:sw=4
*/
