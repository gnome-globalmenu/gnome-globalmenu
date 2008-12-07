using Gtk;

namespace Gnomenu {
	public extern bool grab_key(Gdk.Window * grab_window, uint keyval, Gdk.ModifierType state);
	public extern bool ungrab_key(Gdk.Window * grab_window, uint keyval, Gdk.ModifierType state);
}
