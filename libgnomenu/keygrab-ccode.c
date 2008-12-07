#include <gdk/gdk.h>
#include <gdk/gdkx.h>
static gboolean maybe_grab_key(GdkWindow * grab_window, guint keyval, GdkModifierType state, gboolean maybe) {
	GdkDisplay * display = gdk_drawable_get_display(grab_window);
	g_assert(display);
	g_assert(grab_window);
	GdkKeymapKey * keys = NULL;
	guint modifiers; 
	gint n_keys = 0;
	int i;
	int min, max;
	XDisplayKeycodes(GDK_DISPLAY_XDISPLAY(display), &min, &max);
	gboolean rt = gdk_keymap_get_entries_for_keyval(NULL, keyval, &keys, &n_keys);
	if(rt == FALSE) return FALSE;
	state &= ~(1 << 13 | 1 << 14);
	for(i = 0; i < n_keys; i++) {
		modifiers = state | (keys[i].group << 13);
		gdk_error_trap_push();
		g_message("keycode = %x, group = %d, modifies=%x", keys[i].keycode, keys[i].group, modifiers);
		if(maybe) {
			XGrabKey(GDK_DISPLAY_XDISPLAY(display), 
					keys[i].keycode, modifiers,
					GDK_WINDOW_XWINDOW(grab_window),
					True,
					GrabModeAsync,
					GrabModeAsync);
		} else {
			XUngrabKey(GDK_DISPLAY_XDISPLAY(display), 
					keys[i].keycode, modifiers, 
					GDK_WINDOW_XWINDOW(grab_window));
		}
		gdk_flush ();
		int errcode = gdk_error_trap_pop();
		if(errcode) {
			g_message("errorcode = %d", errcode);
			rt = FALSE;
			break;
		} else {
		}
	}
	g_free(keys);
	return rt;
}
gboolean gnomenu_grab_key(GdkWindow * grab_window, guint keyval, GdkModifierType state) {
	return maybe_grab_key(grab_window, keyval, state, TRUE);
}
gboolean gnomenu_ungrab_key(GdkWindow * grab_window, guint keyval, GdkModifierType state) {
	return maybe_grab_key(grab_window, keyval, state, FALSE);
}
