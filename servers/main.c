#include <config.h>
#include <gtk/gtk.h>

#include "libgnomenu/serverhelper.h"
#include "application.h"
#include "log.h"
#include "intl.h"
#include "appmenu.h"

static void window_destroy(GtkWidget * widget, gpointer useless){
	gtk_main_quit();
}
GtkWidget * show_about_dialog, * show_conf_dialog;
GtkWidget * set_bg;
GtkWidget * switch_orientation;
GtkWidget * app_menu;
static GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL;
static void button_clicked(Application * app, GtkWidget * button){
	if(button == show_about_dialog){
		application_show_about_dialog(app);
	}
	if(button == show_conf_dialog){
		application_show_conf_dialog(app);
	}
	if(button == switch_orientation){
		g_object_set(app, "orientation", orientation, NULL);
		orientation = (orientation + 1) % 2;
	}
	if(button == set_bg){
		GtkWidget * file_chooser 
			= gtk_file_chooser_dialog_new(NULL, NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN,
				GTK_RESPONSE_OK,
				GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL,
					NULL);
		GtkResponseType res = gtk_dialog_run(file_chooser);
		if(res == GTK_RESPONSE_OK){
			gchar * fn = gtk_file_chooser_get_filename(file_chooser);
			GError * error = NULL;
			GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(fn, &error);
			if(pixbuf){
			GdkPixmap * pixmap = gdk_pixmap_new(gtk_widget_get_root_window(button), 100, 100, -1);
			GdkGC * gc = gdk_gc_new(pixmap);
			gdk_draw_pixbuf(pixmap, gc, pixmap, 0, 0, 0, 0, 100, 100, GDK_RGB_DITHER_NONE, 0, 0);
			application_set_background(app, NULL, pixmap);
			g_object_unref(pixmap);
			g_object_unref(gc);
			g_object_unref(pixbuf);
			}else {
				LOG(" load bg failed: %s", error->message);
				g_error_free(error);
			}
			g_free(fn);
		}
		gtk_widget_destroy(file_chooser);
	}
}
static title_clicked(Application * app, gpointer data){
	gtk_menu_popup(app_menu, NULL, NULL, NULL, NULL, NULL, NULL);
}
static gboolean fancy = FALSE;
static gboolean stick = TRUE;
static gboolean topmost = TRUE;

int main (int argc, char * argv []){
GOptionEntry entries [] = {
	{ "fancy", 'f', 0, G_OPTION_ARG_NONE, &fancy, _("Show fancy control buttons for menu bars"), NULL },
	{ "no-stick", 's', G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE, &stick, _("No Stick to the workspace"), NULL},
	{ "no-topmost", 't', G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE, &topmost, _("No Always on top"), NULL },
	{ NULL }
};
	GtkWindow * window;
	GtkBox  * vbox, /*for buttons*/
			* hbox;
	GtkContainer * container;
	GError * error = NULL;
	GOptionContext * context;

	int i;
	Application * app;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_init(&argc, &argv);
	
	context = g_option_context_new("Application parameters");	
	g_option_context_add_main_entries ( context, entries, GETTEXT_PACKAGE);
	g_option_context_add_group ( context, gtk_get_option_group (TRUE));
	g_option_context_parse(context, &argc, &argv, &error);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(window, 400,20);
	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);

	container = gtk_event_box_new();
	app_menu = app_menu_new();
	gtk_menu_attach_to_widget(app_menu, container, NULL);
	gtk_container_set_border_width(container, 0);

	gtk_box_pack_start(hbox, container, TRUE, TRUE, 0);
	gtk_box_pack_start(hbox, vbox, FALSE, FALSE, 0);

	gtk_container_add(window, hbox);
	g_signal_connect(window, "destroy", 
			window_destroy, NULL);
	app = g_object_new(TYPE_APPLICATION, 
				"window", container, 
				NULL);

#define NEW_BUTTON(b) \
	b = gtk_button_new_with_label(#b); \
	gtk_box_pack_start_defaults(vbox, b); \
	g_signal_connect_swapped(b, "clicked", button_clicked, app);

	if(fancy){
		NEW_BUTTON(show_about_dialog);
		NEW_BUTTON(show_conf_dialog);
		NEW_BUTTON(switch_orientation);
		NEW_BUTTON(set_bg);
	}

	gtk_window_set_keep_above(window, topmost);
	if(stick) gtk_window_stick(window);
	g_signal_connect(app, "title-clicked", title_clicked, NULL);
	gtk_widget_show_all(window);
	application_start(app);
	gtk_main();

	return 0;
}
/*
vim:ts=4:sw=4
*/
