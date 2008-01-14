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
static void preference_dlg_cb(GtkDialog * dlg, gint arg, Application * App){
	switch(arg){
		case GTK_RESPONSE_ACCEPT: 
			g_message("Preference Accepted");
			ui_repaint_all(App);
			break;
		default:
			g_message("What Response is it?");
	}
	gtk_widget_destroy(dlg);
}
void preference_load_conf(Application * App){
	App->AppletProperty.show_title = FALSE;
	App->AppletProperty.show_icon = FALSE;
	App->AppletProperty.show_arrows = TRUE;
}
void preference_show_dialog(Application * App){
	GtkDialog * dlg = gtk_dialog_new();
	GtkBox * vbox = gtk_vbox_new(TRUE, 0);
	GtkLabel * show = gtk_label_new(_("Display following elements"));
	GtkCheckButton * show_title = gtk_check_button_new_with_label (_("Active Window Title"));
	GtkCheckButton * show_icon = gtk_check_button_new_with_label (_("Active Window Icon"));
	GtkCheckButton * show_arrows = gtk_check_button_new_with_label (_("Scrolling Arrows"));

	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(show));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(show_title));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(show_icon));
	gtk_box_pack_start_defaults(vbox, GTK_WIDGET(show_arrows));

	gtk_container_add(dlg->vbox, GTK_WIDGET(vbox));
	gtk_dialog_add_button(dlg, GTK_STOCK_APPLY, GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(dlg, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);
	
	g_signal_connect(dlg, "response", G_CALLBACK(preference_dlg_cb), App);
	gtk_widget_show_all(dlg);
}
