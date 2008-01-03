#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

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
	gtk_label_set_max_width_chars(App->TitleLabel, 10);

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

/********Button: focus hack*********/
	button = GTK_BUTTON(gtk_button_new());
	gtk_button_set_relief(button, GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
	gtk_box_pack_start(basebox, GTK_WIDGET(button),
				FALSE, FALSE, 0);

/*******Applet tweaks*************/
	if(App->Mode == APP_APPLET){ /*setup a compact visual if in a panel*/
		gtk_container_set_border_width(GTK_CONTAINER(basebox), 0);
	}
/*****Hide them since they don't do nothing**********/
	gtk_widget_hide(GTK_WIDGET(App->Forward));
	gtk_widget_hide(GTK_WIDGET(App->Backward));


}

void ui_repaint_all(Application * App){
	gboolean show_backward;
	gboolean show_forward;


	show_backward = TRUE;
	show_forward = TRUE;
	

	if(show_forward) 
		gtk_widget_show(GTK_WIDGET(App->Forward));
	else
		gtk_widget_hide(GTK_WIDGET(App->Forward));
	if(show_backward) 
		gtk_widget_show(GTK_WIDGET(App->Backward));
	else
		gtk_widget_hide(GTK_WIDGET(App->Backward));

	gtk_image_set_from_stock(App->ClientIcon, GTK_STOCK_YES, GTK_ICON_SIZE_MENU);

}
