[CompatType]
public class Gtk.WindowClass: GLib.TypeClass {
	public KeysChangedDelegate keys_changed;
	public static delegate void KeysChangedDelegate(Gtk.Window window);
}
[CCode (sentinel = "G_MARKUP_COLLECT_INVALID")]
[CCode (cname= "g_markup_collect_attributes")]
public static bool collect_attributes (string element_name, [CCode (array_length = false) ] string[] attribute_names, [CCode (array_length = false)] string[] attribute_values, ...) throws GLib.MarkupError;
