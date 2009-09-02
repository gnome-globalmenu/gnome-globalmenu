[CompatType]
public class Gtk.WindowClass: GLib.TypeClass {
	public KeysChangedDelegate keys_changed;
	public static delegate void KeysChangedDelegate(Gtk.Window window);
}
