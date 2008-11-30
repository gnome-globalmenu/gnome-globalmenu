#include <string.h>
#include <X11/Xatom.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

void gdk_window_set_menu_context (GdkWindow * window, char* context ) {
	GdkAtom atom = gdk_atom_intern("_NET_GLOBALMENU_MENU_CONTEXT", FALSE);
	GdkAtom type = gdk_atom_intern("STRING", FALSE);
	gdk_property_change(window, atom, type, 8, GDK_PROP_MODE_REPLACE, context, strlen(context));
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
