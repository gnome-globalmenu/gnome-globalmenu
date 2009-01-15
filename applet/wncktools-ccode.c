#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <libwnck/libwnck.h>
#include <libwnck/window-action-menu.h>
#ifndef wnck_action_window_new
GtkMenu * wnck_action_window_new(WnckWindow * window) {
	return wnck_create_window_action_menu(window);
}
#endif
WnckWindow * gdk_window_to_wnck_window (GdkWindow * window) {
	return wnck_window_get(GDK_WINDOW_XID(window));
}

WnckScreen * gdk_screen_to_wnck_screen (GdkScreen * screen) {
	gint number = gdk_screen_get_number(screen);
	return wnck_screen_get(number);
}


