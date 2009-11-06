namespace Gnomenu {
	/**
	 * grab a key to a Gdk.Window.
	 *
	 * This is an external CCode function.
	 */
	internal extern bool grab_key(Gdk.Window * grab_window, uint keyval, Gdk.ModifierType state);
	/**
	 * release the grab on a key from a Gdk.Window.
	 *
	 * This is an external CCode function.
	 */
	internal extern bool ungrab_key(Gdk.Window * grab_window, uint keyval, Gdk.ModifierType state);
}
