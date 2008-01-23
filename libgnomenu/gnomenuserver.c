#include <gtk/gtk.h>

#include "gnomenuserver.h"

#define LOG_FUNC_NAME g_message(__func__)

typedef struct _GnomeMenuServerPrivate GnomeMenuServerPrivate;

struct _GnomeMenuServerPrivate {
	int foo;
};

#define GNOME_MENU_SERVER_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE(obj, GNOME_TYPE_MENU_SERVER, GnomeMenuServerPrivate))

static void gnome_menu_server_dispose(GObject * self);
static void gnome_menu_server_finalize(GObject * self);
static void gnome_menu_server_init(GnomeMenuServer * self);
static void gnome_menu_server_client_new(GnomeMenuServer * self, GdkNativeWindow client);
static void gnome_menu_server_client_destroy(GnomeMenuServer * self, GdkNativeWindow client);
static void gnome_menu_server_client_size_request(GnomeMenuServer * self, GdkNativeWindow client, GtkRequisition * requisition);

G_DEFINE_TYPE (GnomeMenuServer, gnome_menu_server, G_TYPE_OBJECT)

static void
gnome_menu_server_class_init(GnomeMenuServerClass *klass){
	GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
	LOG_FUNC_NAME;

	g_type_class_add_private(gobject_class, sizeof (GnomeMenuServerPrivate));
	gobject_class->dispose = gnome_menu_server_dispose;
	gobject_class->finalize = gnome_menu_server_finalize;

	klass->signal_client_new = gnome_menu_server_client_new;
	klass->signal_client_destroy = gnome_menu_server_client_destroy;
	klass->signal_client_size_request = gnome_menu_server_client_size_request;
}

static void
gnome_menu_server_init(GnomeMenuServer * self){
	LOG_FUNC_NAME;
}

/**
 * gnome_menu_server_new:
 * 
 * create a new menu server object
 */
GnomeMenuServer * 
gnome_menu_server_new(){
	GnomeMenuServer * self;
	LOG_FUNC_NAME;
	self = g_object_new(GNOME_TYPE_MENU_SERVER, NULL);
	self->socket = gdk_socket_new(GNOME_MENU_SERVER_NAME);
	self->clients = NULL;
}

