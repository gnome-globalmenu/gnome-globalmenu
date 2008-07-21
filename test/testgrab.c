#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <libgnomenu/tools.h>
void _key_press(GtkWidget * widget, GdkEventKey * event, gpointer userdata){
	g_message("key pressed: %s", gdk_keyval_name(event->keyval));
}
int main(int argc, char* argv[]){
	gtk_init(&argc, &argv);
	g_message("%d", gdk_keyval_from_name("A"));
	GtkWindow * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "key-press-event", _key_press, 
			NULL);
	gdkx_tools_grab_key(
			XKeysymToKeycode(GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
			  XStringToKeysym("A")), Mod1Mask
			);
	gtk_widget_show_all(window);
	gtk_main();
	return 1;
}
