#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libbonoboui.h>

#include "typedefs.h"
#include "application.h"
#include "ui.h"

GtkEventBox * ui_create_label_area(Application * App){
	GtkEventBox * label_area;	
	GtkBox * label_area_box = NULL;
	label_area = GTK_EVENT_BOX(gtk_event_box_new());
	label_area_box = GTK_BOX(gtk_hbox_new(FALSE, 0));
	gtk_event_box_set_visible_window(label_area, FALSE);
	gtk_box_pack_start(label_area_box, GTK_WIDGET(App->ClientIcon), FALSE, FALSE, 0);
	gtk_box_pack_start(label_area_box, GTK_WIDGET(App->TitleLabel), FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(label_area), GTK_WIDGET(label_area_box));
	return label_area;
}

GtkEventBox * ui_create_event_box_with_icon(const gchar * stock_id){
	GtkEventBox * rt;
	GtkImage * icon;
	rt = GTK_EVENT_BOX(gtk_event_box_new());
	gtk_event_box_set_visible_window(rt, FALSE);
	icon = GTK_IMAGE(gtk_image_new_from_stock(
			stock_id, GTK_ICON_SIZE_MENU));
	gtk_container_add(GTK_CONTAINER(rt), GTK_WIDGET(icon));
	return rt;
}
void ui_create_popup_menu(Application * App, UICallbacks * callbacks){
	static const char toggle_menu_xml [] =
	"<popup name=\"button3\">\n"
		"<menuitem name=\"About\" "
		"          verb=\"About\" "
		"          _label=\"_About\" "
		"          pixtype=\"stock\" "
		"          pixname=\"gtk-about\"/>\n"
		"<menuitem name=\"Preference\" "
		"          verb=\"Preference\" "
		"          _label=\"_Preference\" "
		"          pixtype=\"stock\" "
		"          pixname=\"gtk-preferences\"/>\n"
   "</popup>\n";
	BonoboUIVerb toggle_menu_verbs[] = {
		BONOBO_UI_VERB ("About", callbacks->popup_menu_cb),
		BONOBO_UI_VERB ("Preference", callbacks->popup_menu_cb),
		BONOBO_UI_VERB_END
	};
	BonoboUIComponent* popup_component = 
		panel_applet_get_popup_component(App->MainWindow);
	panel_applet_setup_menu(App->MainWindow, 
			toggle_menu_xml, 
			toggle_menu_verbs, 
			App);
}
void ui_create_all(Application * App, UICallbacks * callbacks){
	GtkBox * basebox = NULL;
	GtkEventBox * label_area = NULL;

	GtkButton * button = NULL; /*So the dashed box will cover the button instead of the Applet, and the menu will response when clicked at the very top.*/

/****** Applet's Base Horizontal Box*******/
	basebox = GTK_BOX(gtk_hbox_new(FALSE, 0));
	gtk_container_add(GTK_CONTAINER(App->MainWindow), GTK_WIDGET(basebox));

/********Label Area********/
	App->ClientIcon = GTK_IMAGE(gtk_image_new());
	App->TitleLabel = GTK_LABEL(gtk_label_new(""));
//	gtk_label_set_max_width_chars(App->TitleLabel, 10);

	label_area = ui_create_label_area(App);
	gtk_box_pack_start(basebox, GTK_WIDGET(label_area), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(label_area), "button-press-event",
			G_CALLBACK(callbacks->label_area_action_cb), App);

/****** Move Backward ***********/
	App->Backward = ui_create_event_box_with_icon(GTK_STOCK_GO_BACK);
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Backward), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(App->Backward), "button-press-event",
			G_CALLBACK(callbacks->backward_action_cb), App);

/**********Holder: Menubars Shows here**********/
	App->Holder = GTK_FIXED(gtk_fixed_new());
	gtk_fixed_set_has_window(App->Holder, TRUE);
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Holder), TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(App->Holder), "size-allocate", 
			G_CALLBACK(callbacks->holder_resize_cb), App);
/****** Move Forward ***********/
	App->Forward = ui_create_event_box_with_icon(GTK_STOCK_GO_FORWARD);
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Forward), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(App->Forward), "button-press-event",
			G_CALLBACK(callbacks->forward_action_cb), App);

/*******Applet tweaks*************/
	gtk_container_set_border_width(GTK_CONTAINER(basebox), 0);
	ui_create_popup_menu(App, callbacks);

}

void ui_repaint_all(Application * App){
	gboolean show_arrows = App->AppletProperty.show_arrows;
	gboolean show_backward;
	gboolean show_forward;
	gboolean show_icon = App->AppletProperty.show_icon;
	gboolean show_title = App->AppletProperty.show_title;
	gint title_max_width_chars = App->AppletProperty.title_max_width_chars;

	GdkPixbuf * icon;
	gint icon_width;
	gint icon_height;
	show_backward = TRUE & show_arrows;
	show_forward = TRUE & show_arrows;

	if(show_forward) 
		gtk_widget_show(GTK_WIDGET(App->Forward));
	else
		gtk_widget_hide(GTK_WIDGET(App->Forward));
	if(show_backward) 
		gtk_widget_show(GTK_WIDGET(App->Backward));
	else
		gtk_widget_hide(GTK_WIDGET(App->Backward));

	if(show_icon && App->ActiveIcon){
		icon_width = GTK_WIDGET(App->MainWindow)->allocation.width;
		icon_height = GTK_WIDGET(App->MainWindow)->allocation.height;
		if(icon_height < icon_width) 
			icon_width = icon_height;
		else 
			icon_height = icon_width;
		icon = gdk_pixbuf_scale_simple(App->ActiveIcon, icon_width, icon_height, GDK_INTERP_BILINEAR);
		gtk_image_set_from_pixbuf(App->ClientIcon, icon);
		g_object_unref(icon);
		gtk_widget_show(GTK_WIDGET(App->ClientIcon));
	}else
		gtk_widget_hide(GTK_WIDGET(App->ClientIcon));
	if(show_title && App->ActiveTitle){
		gchar * markup = g_markup_printf_escaped ("<b>%s</b>", App->ActiveTitle);
		gtk_label_set_max_width_chars(GTK_LABEL(App->TitleLabel), title_max_width_chars);
		gtk_label_set_markup(GTK_LABEL(App->TitleLabel), markup);
		gtk_label_set_ellipsize(GTK_LABEL(App->TitleLabel), PANGO_ELLIPSIZE_END);
		g_free(markup);
		gtk_widget_show(GTK_WIDGET(App->TitleLabel));
	}else
		gtk_widget_hide(GTK_WIDGET(App->TitleLabel));

}

void ui_show_about(Application * App){
	gchar * authors[] = {
		"Yu Feng <rainwoodman@gmail.com>",
		"Mingxi Wu <fengshenx.@gmail.com>",
		"And thanks to others for the discussion",
		NULL
		}	;
	gtk_show_about_dialog(NULL, 
				"authors", authors, NULL);
}
