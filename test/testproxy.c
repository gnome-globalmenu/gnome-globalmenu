#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libgnomenu/proxy.h>
GnomenuProxy * proxy;
GtkButton * bind, * invoke;
static void button_clicked_cb(GtkButton * button, gpointer user_data){
	if(button == bind){
		if(!gnomenu_proxy_bind(proxy)){
			g_print("fail to bind");
		}
	}
	if(button == invoke){
		gnomenu_proxy_invoke(proxy, "query", "%s", "all");
	}
}
int main(int argc, char * argv){
	GtkWindow * window;
	GtkBox * vbox;

	gtk_init(&argc, &argv);
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));
	proxy = gnomenu_proxy_new("org/gnome/globalmenu/foo");
#define ADD_BUTTON(buttonname) \
	buttonname = GTK_BUTTON(gtk_button_new_with_label(#buttonname)); \
	g_signal_connect(G_OBJECT(buttonname), "clicked",  \
			G_CALLBACK(button_clicked_cb), NULL); \
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(buttonname));
	ADD_BUTTON(bind);
	ADD_BUTTON(invoke);
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();

	return 0;
}
