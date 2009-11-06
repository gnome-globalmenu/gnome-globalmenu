#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

/**
 * maybe_grab_key:
 *
 * @grab_window: window to grab the key
 * @keyval: the keyval (used extensively by Gtk). 0 for AnyKey.
 * @state: modifier state (used extensively by Gtk)
 * @maybe: TRUE to grab, FALSE to ungrab.
 *
 * This function grabs/ungrabs the key by calling XGrabKey.
 * Before calling XGrabKey, the hardware keycodes are found
 * by @gdk_keymap_get_entries_from_keyval.
 *
 * returns FALSE if there is an error. TRUE if OK.
 */
static gboolean maybe_grab_key (GdkWindow * grab_window, 
				guint keyval, 
				GdkModifierType state, 
				gboolean maybe) {

	GdkDisplay * display = gdk_drawable_get_display(grab_window);
	g_assert(display);
	g_assert(grab_window);
	GdkKeymapKey * keys = NULL;
	guint modifiers; 
	gint n_keys = 0;
	int i;
	int min, max;
	gboolean rt;
	if(keyval != 0) {
		rt = gdk_keymap_get_entries_for_keyval(NULL, keyval, &keys, &n_keys);
		if(rt == FALSE) return FALSE;
	} else {
		keys = g_new0(GdkKeymapKey, 1);
		n_keys = 1;
		keys[0].keycode = AnyKey;
		keys[0].group = 0;
	}
	state &= ~(1 << 13 | 1 << 14);
	for(i = 0; i < n_keys; i++) {
		modifiers = state | (keys[i].group << 13);
		gdk_error_trap_push();
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
			rt = FALSE;
			break;
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

