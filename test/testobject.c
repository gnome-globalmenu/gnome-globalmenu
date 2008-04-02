#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/object.h>
GnomenuObject * object;
GtkButton * expose, * signal;
static void button_clicked_cb(GtkButton * button, gpointer user_data){
	if(button == expose){
		gnomenu_object_expose(object);
	}
	if(button == signal){

	}
}
int main(int argc, char * argv){
	GtkWindow * window;
	GtkBox * vbox;

	gtk_init(&argc, &argv);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));
	object = gnomenu_object_new("org/gnome/globalmenu/foo");
#define ADD_BUTTON(buttonname) \
	buttonname = GTK_BUTTON(gtk_button_new_with_label(#buttonname)); \
	g_signal_connect(G_OBJECT(buttonname), "clicked",  \
			G_CALLBACK(button_clicked_cb), NULL); \
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(buttonname));
	ADD_BUTTON(expose);
	ADD_BUTTON(signal);
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();

	return 0;
}
