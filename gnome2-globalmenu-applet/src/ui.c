#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

#include "typedefs.h"
#include "ui.h"

GtkEventBox * ui_create_label_area(Application * App){
	GtkEventBox * label_area;	
	GtkBox * label_area_box = NULL;
	label_area = GTK_EVENT_BOX(gtk_event_box_new());
	label_area_box = GTK_BOX(gtk_hbox_new(FALSE, 0));
	gtk_box_pack_start(label_area_box, GTK_WIDGET(App->ClientIcon), FALSE, FALSE, 0);
	gtk_box_pack_start(label_area_box, GTK_WIDGET(App->TitleLabel), FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(label_area), GTK_WIDGET(label_area_box));
	return label_area;
}

GtkEventBox * ui_create_event_box_with_icon(const gchar * stock_id){
	GtkEventBox * rt;
	GtkImage * icon;
	rt = GTK_EVENT_BOX(gtk_event_box_new());
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

/**********Layout and Notebook: Menubars Shows here**********/
	App->Layout = GTK_LAYOUT(gtk_layout_new(NULL, NULL));
	App->Notebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_layout_put(App->Layout, GTK_WIDGET(App->Notebook), 0, 0); /*inital position*/
	gtk_box_pack_start(basebox, GTK_WIDGET(App->Layout), TRUE, TRUE, 0);

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
		gtk_container_set_border_width(GTK_CONTAINER(App->Layout), 0);
		/*layout don't care about border width, anyway, set it*/
		gtk_container_set_border_width(GTK_CONTAINER(App->Notebook), 0);
		gtk_notebook_set_show_tabs(App->Notebook, FALSE);
		gtk_notebook_set_show_border(App->Notebook, FALSE);
	}


}

void ui_repaint_all(Application * App){
	int page_num;
	gint h;
	gint w;
	gboolean show_backward;
	gboolean show_forward;
	ClientEntry * client = App->ActiveClient;

	if(client->Type == MENUBAR_LOCAL){
			page_num = gtk_notebook_page_num(App->Notebook, client->Widget);
	}else{
			page_num = gtk_notebook_page_num(App->Notebook, GTK_WIDGET(client->Socket));
	}
	g_assert(page_num != -1);
	gtk_notebook_set_current_page(App->Notebook, page_num);
	gtk_label_set_text(App->TitleLabel, client->Title);
	gtk_widget_set_tooltip_text(GTK_WIDGET(App->ClientIcon), client->Title);
	h = GTK_WIDGET(App->Layout)->allocation.height;
	w = GTK_WIDGET(App->Layout)->allocation.width;

	g_print("Determining which scroll button to show:\n"
		"client->x: %d, w: %d, client->w: %d\n",
		client->x, w, client->w);
	show_backward = FALSE;
	show_forward = FALSE;
	
	show_backward = (client->x > w - client->w);
	show_forward = (client->x < 0);
	g_print("Backward: %d, Forward: %d",
		show_backward, show_forward);

	if(show_forward) 
		gtk_widget_show(GTK_WIDGET(App->Forward));
	else
		gtk_widget_hide(GTK_WIDGET(App->Forward));
	if(show_backward) 
		gtk_widget_show(GTK_WIDGET(App->Backward));
	else
		gtk_widget_hide(GTK_WIDGET(App->Backward));

	gtk_widget_set_size_request(GTK_WIDGET(App->Notebook), client->w, h);

	gtk_layout_move(App->Layout, App->Notebook, client->x, client->y);

	if(client->Type == MENUBAR_LOCAL){ /*Should load a pixmap for dummy*/
/*		gtk_image_set_from_pixbuf(App->ClientIcon, NULL);
		gtk_image_clear(App->ClientIcon);*/
		gtk_image_set_from_stock(App->ClientIcon, GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
	}else{
		GdkPixbuf * resized_icon = NULL;
		resized_icon = gdk_pixbuf_scale_simple(client->Icon, h, h, GDK_INTERP_BILINEAR);
		gtk_image_set_from_pixbuf(App->ClientIcon, resized_icon);
		g_object_unref(G_OBJECT(resized_icon));
	}

}
