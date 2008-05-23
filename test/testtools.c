#include <gtk/gtk.h>
#include <libgnomenu/tools.h>

enum {
	SET_PROP,
	BN_MAX,
};
GtkButton * buttons[BN_MAX];
static void button_clicked(GtkWidget * button, GtkWidget * window){
	char prop_value[] = "this is a test";
	char *prop_value_got;
	int i;
	for(i=0; i< 100; i++){
		_gdkx_tools_set_window_prop_blocked(window->window, gdk_atom_intern("TEST_PROP", FALSE), prop_value, sizeof(prop_value));
		prop_value_got = _gdkx_tools_get_window_prop(window->window, gdk_atom_intern("TEST_PROP", FALSE), NULL);
		g_message("value got: %s", prop_value_got);
		g_assert_cmpstr(prop_value_got, ==, prop_value);
		g_free(prop_value_got);
	}
}
int main(int argc, char* argv[]){
	gtk_init(&argc, &argv);
	GtkWindow * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkBox * box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(box));

#define ADD_BUTTON(bn) \
	buttons[bn] = GTK_BUTTON(gtk_button_new_with_label(#bn));\
	g_signal_connect(G_OBJECT(buttons[bn]), "clicked", \
			G_CALLBACK(button_clicked), window);\
	gtk_box_pack_start_defaults(box, GTK_WIDGET(buttons[bn]));
	ADD_BUTTON(SET_PROP);
	gtk_widget_show_all(window);

	gtk_main();
	return 1;
}
