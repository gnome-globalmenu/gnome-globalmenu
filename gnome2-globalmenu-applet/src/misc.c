#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

#include "typedefs.h"
#include "misc.h"

gboolean wnck_window_is_menubar(WnckWindow * window){
	if(g_str_equal(wnck_window_get_name(window), "GTK MENUBAR")){
		g_print("menu bar discovered\n");
		return TRUE;
	}
		g_print("not a menu bar\n");
	return FALSE;
}
static XWindowID get_transient_for(Window window);
XWindowID menubar_window_get_master(WnckWindow * wnck_menubarwindow){
	return(get_transient_for(wnck_window_get_xid(wnck_menubarwindow)));
}
// don't use wnck's, it would fail if main window isn't shown yet.
// rainwoodman: this code is copied from original applet. but Why?
static XWindowID get_transient_for(Window window) {
  Window parent = 0;
  Window* w = NULL;
  Atom type;
  int format;
  gulong nitems;
  gulong bytes_after;
  if (XGetWindowProperty (gdk_display, window,
                          XInternAtom(gdk_display, "WM_TRANSIENT_FOR", FALSE),
                          0, G_MAXLONG, FALSE, XA_WINDOW,
                          &type, &format, &nitems, &bytes_after,
                          (guchar **) &w) == Success
      && w != NULL)
  {
    parent = *w;
    XFree (w);
  }
  return parent;
}
