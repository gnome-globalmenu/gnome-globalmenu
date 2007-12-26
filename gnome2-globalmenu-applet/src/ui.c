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
