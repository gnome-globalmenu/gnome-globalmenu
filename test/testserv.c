#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/service.h>
GnomenuService * service;

int main(int argc, char * argv){
	GtkWindow * window;
	GtkBox * vbox;

	gtk_init(&argc, &argv);

	service = gnomenu_service_new("org/gnome/globalmenu/foo");

	gnomenu_service_expose(service);

	gtk_main();

	return 0;
}
