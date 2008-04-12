#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/service.h>
#include <libgnomenu/object.h>
GnomenuService * service;
GnomenuObject * foo;
GnomenuObject * bar;

int main(int argc, char * argv){
	GtkWindow * window;
	GtkBox * vbox;

	gtk_init(&argc, &argv);

	service = gnomenu_service_new("org/gnome/globalmenu/foo");
	foo = gnomenu_object_new("foo");
	bar = gnomenu_object_new("bar");

	gnomenu_service_expose(service);
	gnomenu_service_register(service, foo);
	gnomenu_service_register(service, bar);

	gtk_main();

	return 0;
}
