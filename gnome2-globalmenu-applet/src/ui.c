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
void ui_repaint_all(Application * App){
	int page_num;
	guint h;
	ClientEntry * client = App->ActiveClient;

	if(client->Type == MENUBAR_LOCAL){
			page_num = gtk_notebook_page_num(App->Notebook, client->Widget);
	}else{
			page_num = gtk_notebook_page_num(App->Notebook, GTK_WIDGET(client->Socket));
	}
	g_assert(page_num != -1);
	gtk_notebook_set_current_page(App->Notebook, page_num);
	//gtk_label_set_text(App->TitleLabel, client->Title);
	gtk_widget_set_tooltip_text(GTK_WIDGET(App->ClientIcon), client->Title);
	h = GTK_WIDGET(App->Layout)->allocation.height;
	gtk_widget_set_size_request(GTK_WIDGET(App->Notebook), client->w, h);

	gtk_layout_move(App->Layout, App->Notebook, client->x, client->y);

	if(client->Type == MENUBAR_LOCAL){ /*Should load a pixmap for dummy*/
		gtk_image_set_from_pixbuf(App->ClientIcon, NULL);
		gtk_image_clear(App->ClientIcon);
	}else{
		GdkPixbuf * resized_icon = NULL;
		resized_icon = gdk_pixbuf_scale_simple(client->Icon, h, h, GDK_INTERP_BILINEAR);
		gtk_image_set_from_pixbuf(App->ClientIcon, resized_icon);
		g_object_unref(G_OBJECT(resized_icon));
	}

}
