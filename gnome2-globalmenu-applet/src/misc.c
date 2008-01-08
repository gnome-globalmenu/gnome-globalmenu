#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

#include "typedefs.h"
#include "misc.h"

gboolean wnck_window_is_stealable_menubar(WnckWindow * window){
	if(g_str_equal(wnck_window_get_name(window), "GTK MENUBAR2")){
		g_print("Menu bar discovered, assume stealable\n");
		return TRUE;
	}
		g_print("not a menu bar\n");
	return FALSE;
}
static Window get_transient_for(Window window);
Window menubar_window_get_master(WnckWindow * wnck_menubarwindow){
	return(get_transient_for(wnck_window_get_xid(wnck_menubarwindow)));
}
// don't use wnck's, it would fail if main window isn't shown yet.
// rainwoodman: this code is copied from original applet. but Why?
static Window get_transient_for(Window window) {
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
void
gdk_foreign_window_send_resize_event (GdkWindow * window, gint w, gint h)
{
  XConfigureEvent xconfigure;
  gint x, y;

  g_return_if_fail (window != NULL);
  g_printf("sending resize info to Xwindow %p, w=%d, h=%d\n", GDK_WINDOW_XID(window), w, h);
  memset (&xconfigure, 0, sizeof (xconfigure));
  xconfigure.type = ConfigureNotify;

  xconfigure.event = GDK_WINDOW_XID (window);
  xconfigure.window = GDK_WINDOW_XID (window);

  /* The ICCCM says that synthetic events should have root relative
 *    * coordinates. We still aren't really ICCCM compliant, since
 *       * we don't send events when the real toplevel is moved.
 *          */
  gdk_error_trap_push ();
  gdk_window_get_origin (window, &x, &y);
  gdk_error_trap_pop ();
             
  xconfigure.x = x;
  xconfigure.y = y;
  xconfigure.width = w;
  xconfigure.height = h;

  xconfigure.border_width = 0;
  xconfigure.above = None;
  xconfigure.override_redirect = False;

  gdk_error_trap_push ();
  XSendEvent (GDK_WINDOW_XDISPLAY (window),
          GDK_WINDOW_XID (window),
          False, NoEventMask, (XEvent *)&xconfigure);
  gdk_display_sync (gdk_display_get_default());
  gdk_error_trap_pop ();
}
