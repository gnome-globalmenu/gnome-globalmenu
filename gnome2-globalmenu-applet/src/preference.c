#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <panel-applet.h>

#include "typedefs.h"
#include "application.h"
#include "ui.h"

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif
typedef struct {
	Application * App;
	GtkDialog * window;
	GtkCheckButton * show_title;
	GtkCheckButton * show_icon;
	GtkCheckButton * show_arrows;
	GtkSpinButton * title_max_width_chars;
} PrefDialog;
static void preference_dlg_cb(GtkDialog * nouse, gint arg, PrefDialog * dlg){
	switch(arg){
		case GTK_RESPONSE_ACCEPT: 
			g_message("Preference Accepted");
#define MY_GET_PROP(x, func) dlg->App->AppletProperty.x = func (dlg->x)
			MY_GET_PROP(show_title, gtk_toggle_button_get_active);
			MY_GET_PROP(show_icon, gtk_toggle_button_get_active);
			MY_GET_PROP(show_arrows, gtk_toggle_button_get_active);
			MY_GET_PROP(title_max_width_chars, gtk_spin_button_get_value);
#undef MY_GET_PROP
#define MY_STORE_PROP(type ,x) panel_applet_gconf_set_##type(dlg->App->MainWindow, #x, dlg->App->AppletProperty.x, NULL)
			MY_STORE_PROP(bool, show_title);
			MY_STORE_PROP(bool, show_icon);
			MY_STORE_PROP(bool, show_arrows);
			MY_STORE_PROP(int, title_max_width_chars);
#undef MY_STORE_PROP
			ui_repaint_all(dlg->App);
			break;
		default:
			g_message("What Response is it?");
	}
	gtk_widget_destroy(GTK_WIDGET(dlg->window));
}
void preference_load_conf(Application * App){
	panel_applet_add_preferences(App->MainWindow, "/app/gnome2-globalmenu-applet", NULL);
#define MY_GET_PROP(type, x) App->AppletProperty.x = panel_applet_gconf_get_##type(App->MainWindow, #x, NULL) 
	MY_GET_PROP(bool, show_title);
	MY_GET_PROP(bool, show_icon);
	MY_GET_PROP(bool, show_arrows);
	MY_GET_PROP(int, title_max_width_chars);
#undef MY_GET_PROP
}
void preference_show_dialog(Application * App){
	PrefDialog * dlg = g_new0(PrefDialog, 1);
	GtkBox * vbox = gtk_vbox_new(TRUE, 0);
	GtkWidget * show = gtk_label_new(_("Display following elements"));
	GtkWidget * title_label = gtk_label_new(_("Maximium Title Label Width(in chars)"));
	GtkBox * title_box = gtk_hbox_new(TRUE, 0);
	dlg->window = GTK_DIALOG(gtk_dialog_new());
	dlg->App = App;
	dlg->show_title = GTK_CHECK_BUTTON(gtk_check_button_new_with_label (_("Active Window Title")));
	dlg->show_icon = GTK_CHECK_BUTTON(gtk_check_button_new_with_label (_("Active Window Icon")));
	dlg->show_arrows = GTK_CHECK_BUTTON(gtk_check_button_new_with_label (_("Scrolling Arrows")));
	dlg->title_max_width_chars = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(-1, 1024, 1));

#define MY_SET_PROP(func, x) func(GTK_TOGGLE_BUTTON(dlg->x), App->AppletProperty.x)
	MY_SET_PROP(gtk_toggle_button_set_active, show_title);
	MY_SET_PROP(gtk_toggle_button_set_active, show_icon);
	MY_SET_PROP(gtk_toggle_button_set_active, show_arrows);
	MY_SET_PROP(gtk_spin_button_set_value, title_max_width_chars);
#undef MY_SET_PROP
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(show));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(dlg->show_title));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(dlg->show_icon));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(dlg->show_arrows));
	gtk_box_pack_start_defaults(title_box, GTK_WIDGET(title_label));
	gtk_box_pack_start_defaults(title_box, GTK_WIDGET(dlg->title_max_width_chars));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(title_box));
	gtk_container_add(GTK_CONTAINER(dlg->window->vbox), GTK_WIDGET(vbox));

	gtk_dialog_add_button(dlg->window, GTK_STOCK_APPLY, GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(dlg->window, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);
	
	g_signal_connect(G_OBJECT(dlg->window), "response", G_CALLBACK(preference_dlg_cb), dlg);
	gtk_widget_show_all(GTK_WIDGET(dlg->window));
}
