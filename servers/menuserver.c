#include <gtk/gtk.h>
#include "menuserver.h"

#include "log.h"
#include "intl.h"

#define MENU_SERVER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, TYPE_MENU_SERVER, MenuServerPrivate))
#define GET_OBJECT(_s, s, p) \
	MenuServer * s = MENU_SERVER(_s); \
	MenuServerPrivate * p = MENU_SERVER_GET_PRIVATE(_s);
	
enum {
	PROP_0,
	PROP_BGPIXMAP,
	PROP_BGCOLOR
};

typedef struct {
	gboolean disposed;
} MenuServerPrivate;

struct _MenuClient {
	enum {
		MENU_CLIENT_GTK,
		MENU_CLIENT_KDE,
	} type;
	gpointer handle; /*handle for helper to locate this client, is also the key*/
	union{
		GdkNativeWindow transient;
		GdkNativeWindow parent;  /* for GTK I use parent, for KDE I use transient*/
	};
	GdkWindow * window; /*the window for the menu bar*/
};

static MenuClient * _add_client		( MenuServer * _self);
/* GObject interface */
static void _free_client			( MenuClient * client);
static void _dispose				( GObject * object);
static void _finalize				( GObject * object);
static GObject * _constructor		( GType type, guint n_construct_properties,
									  GObjectConstructParam * constrcut_params);
static void _set_property 		( GObject * _self, 
								  guint property_id, const GValue * value, GParamSpec * pspec );
static void _get_property 		( GObject * _self, 
								  guint property_id, GValue * value, GParamSpec * pspec );
/* GtkWidget interface */
static void _realize			( GtkWidget * widget);
static void _size_allocate		( GtkWidget * widget, GtkAllocation * allocation);

static void _size_request		( GtkWidget * widget, GtkRequisition * requisition);

static void 
	_s_client_new					( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_client_destroy				( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_client_realize				( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_client_reparent				( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_client_unrealize				( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
static void 
	_s_screen_active_window_changed	( MenuServer * _self, 
									  WnckWindow * previous, 
									  WnckScreen * screen);
static void 
	_s_gtk_helper_size_request		( MenuServer * _self, 
									  GnomenuClientInfo * ci, 
									  GnomenuServerHelper * helper);
			
static void _c_active_client_changed( MenuServer * _self);

static void 
	_update_active_menu_bar 		( MenuServer * _self);
static MenuClient * 
	_find_client_by_parent			( MenuServer * _self, GdkNativeWindow parent);
static gboolean _is_client			( MenuServer * server, MenuClient * client);

G_DEFINE_TYPE		(MenuServer, menu_server, GTK_TYPE_WIDGET);

enum {
	ACTIVE_CLIENT_CHANGED,
	SIGNAL_MAX,
};
gulong class_signals[SIGNAL_MAX] = {0};

static void
menu_server_class_init(MenuServerClass * klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass * widget_class = GTK_WIDGET_CLASS(klass);

	GParamSpec * psec;
	LOG("class_init");
	
	g_type_class_add_private(gobject_class, sizeof(MenuServerPrivate));
	gobject_class->dispose = _dispose;
	gobject_class->finalize = _finalize;
	gobject_class->constructor = _constructor;
	gobject_class->set_property = _set_property;
	gobject_class->get_property = _get_property;

	widget_class->realize = _realize;
	widget_class->size_allocate = _size_allocate;
	widget_class->size_request = _size_request;

	klass->active_client_changed = _c_active_client_changed;

	class_signals[ACTIVE_CLIENT_CHANGED] =
		g_signal_new("active-client-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
			G_STRUCT_OFFSET (MenuServerClass, active_client_changed),
			NULL,
			NULL,
			g_cclosure_marshal_VOID__VOID,
			G_TYPE_NONE,
			0);

/*
	g_object_class_install_property (gobject_class,
		PROP_WINDOW,
		g_param_spec_object ("window",
						"window",
						"Widget who has a window to swallow menu bars",
						GTK_TYPE_WIDGET,
						G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
*/
	g_object_class_install_property (gobject_class,
		PROP_BGPIXMAP,
		g_param_spec_object ("bg-pixmap",
						"bg-pixmap",
						"",
						GDK_TYPE_PIXMAP,
						G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class,
		PROP_BGCOLOR,
		g_param_spec_boxed ("bg-color",
						"bg-color",
						"",
						GDK_TYPE_COLOR,
						G_PARAM_READWRITE));
}
static void
menu_server_init(MenuServer * _self){
	GTK_WIDGET_UNSET_FLAGS(_self, GTK_NO_WINDOW);
}
static GObject * 
_constructor	( GType type, guint n_construct_properties,
				  GObjectConstructParam * construct_params) {
	LOG("constructor");
	GObject * _self = ( *G_OBJECT_CLASS(menu_server_parent_class)->constructor)(type,
			n_construct_properties,
			construct_params);
	GET_OBJECT(_self, server, priv);
	server->gtk_helper = gnomenu_server_helper_new();
	server->kde_helper = NULL;
	server->clients = g_hash_table_new_full(NULL, NULL, NULL, _free_client);
	server->screen = wnck_screen_get_default();
	
	g_signal_connect_swapped(server->gtk_helper,
			"client-new", _s_client_new, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-destroy", _s_client_destroy, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-realize", _s_client_realize, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-reparent", _s_client_reparent, server);
	g_signal_connect_swapped(server->gtk_helper,
			"client-unrealize", _s_client_unrealize, server);

	g_signal_connect_swapped(server->screen,
			"active-window-changed", _s_screen_active_window_changed, server);
	g_signal_connect_swapped(server->gtk_helper,
			"size-request", _s_gtk_helper_size_request, server);
	return _self;
}
static void 
_set_property( GObject * _self, guint property_id, const GValue * value, GParamSpec * pspec){
	LOG("set property");
	GET_OBJECT(_self, self, priv);
	gboolean dirty = FALSE;
	switch (property_id){
/*
		case PROP_WINDOW:
			if(GTK_IS_WIDGET(self->window)) g_object_unref(self->window);
			self->window = g_value_get_object(value);
			g_assert(!GTK_WIDGET_NO_WINDOW(self->window));
			g_object_ref(self->window);
		break;
*/
		case PROP_BGCOLOR:
			{
			GdkColor * newcolor = g_value_get_boxed(value);
			if(self->bgcolor && !gdk_color_equal(newcolor, self->bgcolor)){
				LOG("BGCOLOR dirty");
				dirty = TRUE;
			}
			if(self->bgcolor) 
				g_boxed_free(GDK_TYPE_COLOR, self->bgcolor);
			self->bgcolor = g_value_dup_boxed(value);
			}
		break;
		case PROP_BGPIXMAP:
			if(self->bgpixmap != NULL && g_value_get_object(value)!= NULL){
				LOG("BGPIXMAP dirty");
				dirty = TRUE;
			}
			if(GDK_IS_PIXMAP(self->bgpixmap)) g_object_unref(self->bgpixmap);
			self->bgpixmap = g_value_get_object(value);
			if(GDK_IS_PIXMAP(self->bgpixmap)) g_object_ref(self->bgpixmap);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}
	if(dirty) {
		GList * node;
		GnomenuClientInfo * ci;
		for(node = g_list_first(self->gtk_helper->clients);
			node;
			node = g_list_next (node)){
			ci = node->data;
			gnomenu_server_helper_set_background(self->gtk_helper, ci, self->bgcolor, self->bgpixmap);
		}
	}
}
static void 
_get_property( GObject * _self, guint property_id, GValue * value, GParamSpec * pspec){
	GET_OBJECT(_self, self, priv);
	switch (property_id){
/*
		case PROP_WINDOW:
			g_value_set_object(value, self->window);
		break;
*/
		case PROP_BGCOLOR:
			g_value_set_static_boxed(value, self->bgcolor);
		break;
		case PROP_BGPIXMAP:
			g_value_set_object(value, self->bgpixmap);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
	}
}
static void _dispose ( GObject * _self){
	LOG();
	GET_OBJECT(_self, self, priv);
	if(!priv->disposed) {
		priv->disposed = TRUE;
		g_object_unref(self->gtk_helper);
	}
	G_OBJECT_CLASS(menu_server_parent_class)->dispose(_self);
}
static void _finalize ( GObject * _self){
	LOG();
	GET_OBJECT(_self, self, priv);
	if(self->bgcolor)
	g_boxed_free(GDK_TYPE_COLOR, self->bgcolor);
	if(self->bgpixmap)
	g_object_unref(self->bgpixmap);
	g_hash_table_destroy(self->clients);
	G_OBJECT_CLASS(menu_server_parent_class)->finalize(_self);

}
MenuServer * menu_server_new(){
	LOG("menu_server_new");
	return g_object_new(TYPE_MENU_SERVER, NULL);
}
static void _free_client(MenuClient * client){

	if(client->window)
		gdk_window_destroy(client->window);
	g_free(client);
}
static void _s_client_new(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_new0(MenuClient, 1);
	LOG();
	c->type = MENU_CLIENT_GTK;
	c->handle = ci;
	c->window = NULL;
	c->parent = NULL;
	g_hash_table_insert(_self->clients, ci, c);
	gnomenu_server_helper_queue_resize(_self->gtk_helper, c->handle);
}
static void _s_client_realize(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_hash_table_lookup(_self->clients, ci);
	GtkWidget * widget = GTK_WIDGET(_self);
	LOG();
	g_assert(c);
	c->window = gdk_window_foreign_new(ci->ui_window);
	LOG("c->window: %p", ci->ui_window);
	g_assert(c->window);
	gdk_window_reparent(c->window, widget->window, 0, 0);
	gnomenu_server_helper_set_background(_self->gtk_helper, c->handle, _self->bgcolor, _self->bgpixmap);
}
static void _s_client_reparent(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_hash_table_lookup(_self->clients, ci);
	LOG();
	g_return_if_fail(c); /*work around. wondering why*/
	c->parent = ci->parent_window;
	_update_active_menu_bar(_self);
}
static void _s_client_unrealize(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_hash_table_lookup(_self->clients, ci);
	LOG();
	g_assert(c);
	gdk_window_destroy(c->window);
	c->window = NULL;
}
static void _s_client_destroy(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	MenuClient * c = g_hash_table_lookup(_self->clients, ci);
	LOG();
	g_assert(c);
	if( _self->active == c ){
		_self->active = NULL;
	}
	g_hash_table_remove(_self->clients, ci);
}
static MenuClient * _find_client_by_parent(MenuServer * _self, GdkNativeWindow parent){
	GList * node = NULL;
	GList * list = g_hash_table_get_values(_self->clients);
	MenuClient * rt = NULL;
	for(node = g_list_first(list); node; node = g_list_next(node)){
		if(((MenuClient *)node->data)->parent == parent) rt = node->data;
	}
	g_list_free(list);
	return rt;
}
static void _update_active_menu_bar (MenuServer * _self){
	GdkNativeWindow parent_transient ;
	GdkNativeWindow parent;
	GtkWidget * widget = GTK_WIDGET(_self);
	WnckWindow * active = wnck_screen_get_active_window(_self->screen);
	MenuClient * c = NULL;
	if(active){
		WnckWindow * active_transient = wnck_window_get_transient(active);
 		parent = wnck_window_get_xid(active);
		if(active_transient)
			parent_transient = wnck_window_get_xid(active_transient);

		c =_find_client_by_parent(_self, parent);
		if(!c) 
			c = _find_client_by_parent(_self, parent_transient);
		LOG("find client at %p", c);
	} else {
		LOG("active is nil");
	}
		if(_self->active){
			switch(_self->active->type){
				case MENU_CLIENT_GTK:
					gnomenu_server_helper_set_visibility(_self->gtk_helper, _self->active->handle, FALSE);
				break;
				case MENU_CLIENT_KDE:
					/*try to hide it*/
				break;
			}
		}
		if(c){
			switch(c->type){
				case MENU_CLIENT_GTK:
					if(c->window)
						gdk_window_reparent(c->window, widget->window, 0, 0);
					gnomenu_server_helper_set_visibility(_self->gtk_helper, c->handle, TRUE);
				break;
				case MENU_CLIENT_KDE:
					if(c->window)
						gdk_window_reparent(c->window, widget->window, 0, 0);
				break;
			}
		}
		_self->active = c;
		g_signal_emit(_self, class_signals[ACTIVE_CLIENT_CHANGED],
			0);
		gtk_widget_queue_resize(_self);
}
static void 
	_s_screen_active_window_changed	(MenuServer * _self, WnckWindow * previous, WnckScreen * screen){
	WnckWindow * active = wnck_screen_get_active_window(_self->screen);
	if (!active) return;
	if( wnck_window_get_pid(active) == getpid()){
		return;
	}
	_update_active_menu_bar(_self);
}
static void _s_gtk_helper_size_request(MenuServer * _self, GnomenuClientInfo * ci, GnomenuServerHelper * helper){
	GtkWidget * widget = GTK_WIDGET(_self);
	ci->allocation.width = MAX(ci->requisition.width,
							widget->allocation.width);
	ci->allocation.height = MAX(ci->requisition.height,
							widget->allocation.height);
}
static void _size_request(GtkWidget * widget, GtkRequisition * requisition){
	GList * node;
	GET_OBJECT(widget, server, priv);
	GnomenuClientInfo * ci;
	gint w = -1;
	gint h = -1;
/*
	if(server->active){
		ci = server->active->handle;
		w = MAX(ci->requisition.width , w);
		h = MAX(ci->requisition.height, h);
		LOG("%d, %d", w, h);
	} else
	for(node = g_list_first( server->gtk_helper->clients);
		node;
		node = g_list_next (node)){
		ci = node->data;
		w = MAX(ci->requisition.width , w);
		h = MAX(ci->requisition.height, h);
	}
*/
	requisition->width = w;
	requisition->height = h;
	widget->requisition = * requisition;
}
static void _size_allocate(GtkWidget * widget, GtkAllocation * allocation){
	GtkAllocation a = * allocation;
	GET_OBJECT(widget, self, priv);
	MenuClient * c = self->active;
	a.x = 0;
	a.y = 0;
	LOG("w = %d, h = %d", allocation->width, allocation->height);
	if(c)
	switch(c->type){
		case MENU_CLIENT_GTK:
				gnomenu_server_helper_allocate_size(self->gtk_helper, c->handle, allocation);
		break;
		case MENU_CLIENT_KDE:
		LOG("KDE unhandled");
		break;
	}
	widget->allocation = * allocation;
	if(GTK_WIDGET_REALIZED(widget)){
		gdk_window_move_resize(widget->window,
			allocation->x,
			allocation->y,
			allocation->width,
			allocation->height);
	}
}
static void _c_active_client_changed( MenuServer * _self){

}
static gboolean _is_client(MenuServer * server, MenuClient * client){
	GList * node = NULL;
	GList * list = g_hash_table_get_values(server->clients);
	gboolean rt = FALSE;
	for(node = g_list_first(list); node; node = g_list_next(node)){
		if(node->data == client) {
			rt = TRUE;
			break;
		}
	}
	g_list_free(list);
	return rt;
}
WnckWindow * menu_server_get_client_parent(MenuServer  * server, MenuClient * client){
	if(_is_client(server, client))
		return wnck_window_get(client->parent);
	else
		return NULL;
}
static void _realize			( GtkWidget * widget){
  GdkWindowAttr attributes;
  gint attributes_mask;

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK );

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

}
/*
 vim:ts=4:sw=4
*/
