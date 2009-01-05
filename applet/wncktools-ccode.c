#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <libwnck/libwnck.h>

WnckWindow * gdk_window_to_wnck_window (GdkWindow * window) {
	return wnck_window_get(GDK_WINDOW_XID(window));
}

WnckScreen * gdk_screen_to_wnck_screen (GdkScreen * screen) {
	gint number = gdk_screen_get_number(screen);
	return wnck_screen_get(number);
}
