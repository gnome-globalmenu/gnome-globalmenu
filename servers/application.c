#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "application.h"
#include "menuserver.h"
#include "utils.h"
#include "log.h"

#include "intl.h"

enum {
	PROP_0,
	PROP_WINDOW,
	PROP_TITLE_VISIBLE,
	PROP_ICON_VISIBLE,
	PROP_ORIENTATION,
	PROP_TITLE_FONT,
};

typedef struct _ApplicationPrivate {
	gint foo;
	gboolean disposed;
} ApplicationPrivate;

#define APPLICATION_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, TYPE_APPLICATION, ApplicationPrivate))

#define GET_OBJECT(src, obj, priv) \
	Application * obj = APPLICATION(src); \
	ApplicationPrivate * priv = APPLICATION_GET_PRIVATE(src);

static void _s_window_destroy(Application * app, GtkWidget * widget);
static void _s_notify_active_client(Application * app, GParamSpec * pspec, MenuServer * server);
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
static void _dispose					( GObject *obj);
static GObject *_constructor			( GType type, guint n_construct_properties,
										  GObjectConstructParam * construct_params) ;

/* tool function*/
static void _update_background			(Application * app);


G_DEFINE_TYPE		(Application, application, G_TYPE_OBJECT);

static void _create_conf_dialog(Application * app){
	ApplicationConfDlg * cfd = &app->conf_dialog;
	GladeXML * xml = app->glade_factory;
	cfd->dlg = glade_xml_get_widget(xml, "ConfDialog");

	cfd->tgbtn_title_visible = glade_xml_get_widget(xml, "chkbtn_title_visible");
	cfd->ftbtn_title_font = glade_xml_get_widget(xml, "fntbtn_title_font");
	cfd->tgbtn_icon_visible = glade_xml_get_widget(xml, "chkbtn_icon_visible");
	cfd->spnbtn_title_max_width = glade_xml_get_widget(xml, "spnbtn_title_max_width");
}
static void application_init(Application *app)
{
	GET_OBJECT(app, self, priv);
	self->orientation = -1;
	app->hbox = GTK_BOX(gtk_hbox_new(FALSE, 0)); 
	app->vbox = GTK_BOX(gtk_vbox_new(FALSE, 0)); 
	app->title = gtk_label_new("");
	app->icon = gtk_image_new();
	app->server = menu_server_new();
	app->bgpixmap = NULL;
	app->bgcolor = NULL;

	app->title_font = NULL;
	app->title_visible = FALSE;
	app->icon_visible = FALSE;
	app->glade_factory = glade_xml_new("application.glade",
						NULL, NULL);
	if(!app->glade_factory){
		app->glade_factory = glade_xml_new(GLADEDIR"/""application.glade", NULL, NULL);
	}
	g_assert(app->glade_factory);
	_create_conf_dialog(app);
}
static void application_class_init(ApplicationClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	
	obj_class->constructor = _constructor;
	obj_class->finalize = _finalize;
	obj_class->dispose = _dispose;
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

	g_object_class_install_property (obj_class,
		PROP_ORIENTATION,
		g_param_spec_enum ("orientation",
						"orientation",
						"",
						GTK_TYPE_ORIENTATION,
						GTK_ORIENTATION_HORIZONTAL,
						G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

	g_object_class_install_property (obj_class,
		PROP_TITLE_FONT,
		g_param_spec_boxed ("title-font",
						"title-font",
						"",
						PANGO_TYPE_FONT_DESCRIPTION,
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
	gchar * title_font_name;
	ApplicationConfDlg * cfd = &app->conf_dialog;
	PangoLayout * layout = gtk_label_get_layout(GTK_LABEL(app->title));
	if(app->title_visible) 
		gtk_widget_show(app->title);
	else 
		gtk_widget_hide(app->title);

	if(app->icon_visible)
		gtk_widget_show(app->icon);
	else 
		gtk_widget_hide(app->icon);

	pango_layout_set_font_description(layout,
			app->title_font);
	gtk_widget_queue_draw(GTK_WIDGET(app->title));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cfd->tgbtn_title_visible), 
				app->title_visible);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cfd->tgbtn_icon_visible), 
				app->icon_visible);
	title_font_name = pango_font_description_to_string(app->title_font);
	gtk_font_button_set_font_name(GTK_FONT_BUTTON(cfd->ftbtn_title_font), 
			title_font_name);
	g_free(title_font_name);
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
_set_property( GObject * object, guint property_id, const GValue * value, GParamSpec * pspec){

	GET_OBJECT(object, self, priv);
	switch (property_id){
		case PROP_WINDOW:
			if(GTK_IS_WIDGET(self->window)) g_object_unref(self->window);
			self->window = g_value_get_object(value);
			g_assert(!GTK_WIDGET_NO_WINDOW(self->window));
			g_object_ref(self->window);
			break;
		case PROP_TITLE_VISIBLE:
			self->title_visible = g_value_get_boolean(value);
			break;
		case PROP_ICON_VISIBLE:
			self->icon_visible = g_value_get_boolean(value);
			break;
		case PROP_ORIENTATION:
			{
			GtkOrientation o = g_value_get_enum(value);
		/*FIXME: tune widget layout to fit the new orientation*/
			if(self->orientation !=o){
				GtkBox * oldbox = self->box;
				self->orientation = o;
				switch (o){
				case GTK_ORIENTATION_HORIZONTAL:
					self->box = self->hbox;
				break;
				case GTK_ORIENTATION_VERTICAL:
					self->box = self->vbox;
				break;
				}
				if(oldbox){
					gtk_container_remove(GTK_CONTAINER(oldbox), 
									GTK_WIDGET(self->icon));
					gtk_container_remove(GTK_CONTAINER(oldbox), 
									GTK_WIDGET(self->title));
					gtk_container_remove(GTK_CONTAINER(oldbox), 
									GTK_WIDGET(self->server));
					gtk_container_remove(GTK_CONTAINER(self->window), 
									GTK_WIDGET(oldbox));
				}
				g_object_set(self->server, "orientation", self->orientation, NULL);
				gtk_box_pack_start(GTK_BOX(self->box), GTK_WIDGET(self->icon), FALSE, FALSE, 0);
				gtk_box_pack_start(GTK_BOX(self->box), GTK_WIDGET(self->title), FALSE, FALSE, 0);
				gtk_box_pack_start(GTK_BOX(self->box), GTK_WIDGET(self->server), TRUE, TRUE, 0);

				gtk_widget_show(GTK_WIDGET(self->box));
				gtk_container_add(GTK_CONTAINER(self->window), GTK_WIDGET(self->box));
			}
		}
		break;
		case PROP_TITLE_FONT:
		{
			PangoFontDescription * new_title_font
				= g_value_get_boxed(value);
			if(!pango_font_description_equal(new_title_font, self->title_font)){
				if(self->title_font)
					g_boxed_free(PANGO_TYPE_FONT_DESCRIPTION, self->title_font);
				self->title_font = g_value_dup_boxed(value);
			}
		}
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
		case PROP_ORIENTATION:
			g_value_set_enum(value, self->orientation);
		break;
		case PROP_TITLE_FONT:
			g_value_set_boxed(value, self->title_font);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}

}


static void _dispose					( GObject *obj){
	GET_OBJECT(obj, app, priv);
	if(!priv->disposed){
		priv->disposed = TRUE;
		g_signal_handlers_disconnect_by_func(G_OBJECT(app->conf_dialog.dlg),
			G_CALLBACK(_s_conf_dialog_response), app);
		g_signal_handlers_disconnect_by_func(G_OBJECT(app->window), 
			G_CALLBACK(_s_window_destroy), app);
		g_signal_handlers_disconnect_by_func(G_OBJECT(app->server),
			G_CALLBACK(_s_notify_active_client), app);
		g_signal_handlers_disconnect_by_func(G_OBJECT(app->server),
			G_CALLBACK(_s_menu_bar_area_size_allocate), app);
		g_object_unref(app->hbox);
		g_object_unref(app->vbox);
		g_object_unref(app->icon);
		g_object_unref(app->title);
		g_object_unref(app->server);
	}
	G_OBJECT_CLASS(application_parent_class)->dispose(obj);
}
static void _finalize(GObject *obj)
{
	Application *app = APPLICATION(obj);
	if (app->window)
		g_object_unref(app->window);
	g_object_unref(app->glade_factory);
	G_OBJECT_CLASS(application_parent_class)->finalize(obj);
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
	/*This thing is ugly, (consider a vertical menu layout), we need a new alignment widget
 * 	which is similiar to GtkMenuBar(respecting directions) */
/*so that reparenting won't destroy it*/
	g_object_ref(app->hbox);
	g_object_ref(app->vbox);
	g_object_ref(app->icon);
	g_object_ref(app->title);
	g_object_ref(app->server);

	g_signal_connect_swapped(G_OBJECT(app->conf_dialog.dlg), 
		"response", 
		G_CALLBACK(_s_conf_dialog_response), app);
	g_signal_connect_swapped(G_OBJECT(app->window), 
		"destroy",
        G_CALLBACK(_s_window_destroy), app);
	g_signal_connect_swapped(G_OBJECT(app->server),
		"notify::active-client",
		G_CALLBACK(_s_notify_active_client), app);
	g_signal_connect_swapped(G_OBJECT(app->server),
		"size-allocate",
		G_CALLBACK(_s_menu_bar_area_size_allocate), app);

	return _self;
}
/* ENDS: GObject Interface */

/* Public methods */
void application_start(Application * app){
	application_load_conf(app);
	application_update_ui(app);

	_update_background(app);
/* start server */
	menu_server_start(app->server);
}
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
		"Mingxi Wu <fengshenx@gmail.com>",
		"And many help from others",
		NULL
		};
	gchar * translator_credits = _("translator_credits");
	
	if(g_str_equal(translator_credits, "translator_credits"))
		translator_credits = NULL;
	gtk_show_about_dialog(NULL, 
				"authors", authors, 
				"translator_credits", translator_credits, 
				"comments", _("GNOME panel applet for Global Menu"), 
				"version", "0.4",
				"website", "http://gnome2-globalmenu.googlecode.com/",
				NULL);
}
/* END: Public Methods */

/* BEGIN: Signal handlers */
static void _s_notify_active_client(Application * app, GParamSpec * pspec, MenuServer * server){
	ApplicationClass *app_class = G_OBJECT_GET_CLASS(app);
	GdkPixbuf *icon_buf, *resized_icon;
	gint w, h;
	WnckWindow * window = menu_server_get_client_parent(server, server->active_client);

	if(!window) window = wnck_screen_get_active_window(wnck_screen_get_default());
	if(!window) return;
	WnckApplication * application = wnck_window_get_application(window);

	const gchar *name = wnck_application_get_name(application);
	gtk_label_set_text(GTK_LABEL(app->title), name);

	icon_buf =  wnck_application_get_icon(application);
	if (icon_buf) {
		/* FIXME : check the direction fisrt? */
		w = h = MIN(
			GTK_WIDGET(app->window)->allocation.height,
			GTK_WIDGET(app->window)->allocation.width) ;
		resized_icon = gdk_pixbuf_scale_simple(icon_buf , w, h, GDK_INTERP_BILINEAR);
		gtk_image_set_from_pixbuf(GTK_IMAGE(app->icon), resized_icon);
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
	ApplicationConfDlg * cfd = &app->conf_dialog;
	PangoFontDescription * font;
	const gchar * font_name;
	switch(arg){
		case GTK_RESPONSE_CANCEL:
			application_update_ui(self);
		break;
		case GTK_RESPONSE_OK: 
			gtk_widget_hide(dialog);
		case GTK_RESPONSE_APPLY: 
			LOG("Preference Accepted");
			font_name = gtk_font_button_get_font_name(cfd->ftbtn_title_font);
			font = pango_font_description_from_string(font_name);
			LOG("font name = %s, font = %p", font_name, font);
			g_object_set(app,
				"title-visible",
				gtk_toggle_button_get_active(cfd->tgbtn_title_visible),
				"icon-visible",
				gtk_toggle_button_get_active(cfd->tgbtn_icon_visible),
				"title-font",
				font,
				NULL);
			application_save_conf(self);
			application_load_conf(self);
			application_update_ui(self);
			break;
		break;
		default:
			LOG("What Response is it?");
	}
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
	utils_set_widget_background(GTK_WIDGET(app->server), color, cropped);	
}
/* END: tool functions*/
/*
vim:ts=4:sw=4
*/
