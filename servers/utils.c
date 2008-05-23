#include <gtk/gtk.h>
#include "log.h"

void utils_set_widget_background(GtkWidget * widget, GdkColor * color, GdkPixmap * pixmap){
	GtkRcStyle * rc_style;
	GtkStyle * style;
	gtk_widget_set_style (widget, NULL);
	rc_style = gtk_rc_style_new ();
	gtk_widget_modify_style (widget, rc_style);
	gtk_rc_style_unref (rc_style);
	if(color){
		LOG("new bg color %d, %d, %d", color->red, color->green, color->blue);
		gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, color);
	}
	if(pixmap){
		gint w, h;
		gdk_drawable_get_size(pixmap, &w, &h);
		LOG("not implemented for pixmap bg yet");
		LOG("size of pixmap, %d, %d", w, h);

		style = gtk_style_copy (widget->style);
		if (style->bg_pixmap[GTK_STATE_NORMAL])
			g_object_unref (style->bg_pixmap[GTK_STATE_NORMAL]);
		style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref (pixmap);
		gtk_widget_set_style (widget, style);
		g_object_unref (style);

	}
}
