#include <string.h>
#include <X11/Xatom.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include <libgnomenu/gnomenu.h>

void gdk_window_set_menu_context (GdkWindow * window, char* context ) {
	GdkAtom atom = gdk_atom_intern(GNOMENU_NET_GLOBALMENU_MENU_CONTEXT, FALSE);
	GdkAtom type = gdk_atom_intern("STRING", FALSE);
	if(context != NULL) 
		gdk_property_change(window, atom, type, 8, GDK_PROP_MODE_REPLACE, context, strlen(context)+1);
	else 
		gdk_property_delete(window, atom);
}
char * gdk_window_get_menu_event (GdkWindow * window) {
	GdkAtom atom = gdk_atom_intern(GNOMENU_NET_GLOBALMENU_MENU_EVENT, FALSE);
	GdkAtom type = gdk_atom_intern("STRING", FALSE);
	GdkAtom actual_type = NULL;
	gint actual_format;
	gint actual_length;
	char * rt = NULL;
	gdk_property_get(window, atom, type, 0, G_MAXINT, FALSE, &actual_type, &actual_format, &actual_length, &rt);
	return rt;
}
gboolean gdk_window_get_desktop_hint (GdkWindow * window) {
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

GtkWindow * gtk_widget_get_toplevel_window(GtkWidget * widget) {
	if(!GTK_IS_WIDGET(widget)) return NULL;
	return gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW);
}

GtkMenuBar * gtk_window_find_menubar(GtkWidget * widget) {
	GList * list = gtk_container_get_children(widget);
	GList * node;
	for(node = list; node; node = node->next) {
		GtkWidget * child = node->data;
		if(GTK_IS_MENU_BAR(child)) return child;
		if(GTK_IS_CONTAINER(child)) {
			GtkMenuBar * menubar = gtk_window_find_menubar(child);
			if(menubar) return menubar;
		}
	}	
	g_list_free(list);
	return NULL;
}

static void gcgac_cb(GtkWidget * widget, GList ** list) {
	*list = g_list_append(*list, widget);
}
GList * gtk_container_get_all_children(GtkContainer * container) {
	GList * list = NULL;
	gtk_container_forall(container, gcgac_cb, &list);
	return list;
}
