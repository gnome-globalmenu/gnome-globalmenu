#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

gboolean gdk_window_get_is_desktop (GdkWindow * window) {
	Display * display = GDK_DISPLAY_XDISPLAY (gdk_display_get_default());
	Atom atom;
	atom = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", FALSE);
	Atom ret_prop_type;
	gint ret_format;
	gint ret_nitems;
	gint ret_bytes_after;
	Atom * ret_data;	

	gulong offset = 0;
	gulong get_length = G_MAXLONG;
	gboolean pdelete = False;
	Atom propname = XInternAtom(display, "_NET_WM_WINDOW_TYPE", FALSE);
	
	if(Success == XGetWindowProperty (display,
			    GDK_WINDOW_XWINDOW (window), 
				propname,
			    offset, get_length, pdelete,
			    XA_ATOM, 
				&ret_prop_type, 
				&ret_format,
			    &ret_nitems, 
				&ret_bytes_after,
			    &ret_data)) {
		if(*ret_data == atom) {
			XFree(ret_data);
			return TRUE;
		}
		XFree(ret_data);
		return FALSE;
	} else {
		return FALSE;
	}
}
static gboolean menu_item_parent_set_hook(GSignalInvocationHint * hint,
			int value_count, GValue values[], gpointer data) {
	GObject * self = g_value_get_object(&values[0]);
	if(!GTK_IS_WIDGET(self)) return TRUE;
	GtkWidget * label = NULL;
	GQueue queue = G_QUEUE_INIT;
	g_queue_push_tail(&queue, self);
	while(!g_queue_is_empty(&queue)){
		GtkWidget* head = g_queue_pop_head(&queue);
		if(GTK_IS_CONTAINER(head)) {
			GList * list = gtk_container_get_children(head);
			GList * node;
			for(node = list; node; node = node->next) {
				g_queue_push_tail(&queue, node->data);
			}
		}
		if(GTK_IS_LABEL(head) || GTK_IS_SEPARATOR(head)|| GTK_IS_SEPARATOR_MENU_ITEM(head)) {
			g_queue_clear(&queue);
			label = head;
			break;
		}
	}
	GObject * parent = gtk_widget_get_parent(self);
	for(; parent; parent = gtk_widget_get_parent(parent)) {
		if(GTK_IS_MENU_ITEM(parent)) {
			if(label != g_object_get_data(parent, "label")) {
				g_object_set_data(parent, "label", label);
				//FOUND
			}

			break;
		}
	}
	return TRUE;
}
