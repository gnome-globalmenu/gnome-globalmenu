
/***
 * Missing 
 */

[NoArrayLength]
public static bool g_markup_collect_attributes(string element_name, [CCode (array_length = false)] string[] attribute_names, [CCode (array_length = false)] string[] attribute_values, out GLib.Error? error, GMarkupCollectType type, string first_name, ...);

[CCode (cprefix = "G_MARKUP_COLLECT_", has_type_id = false)]
public enum GMarkupCollectType {
	INVALID,
	STRING,
	STRDUP,
	BOOLEAN,
	TRISTATE,
	OPTIONAL
}
[CompatType]
public class Gtk.WindowClass: GLib.TypeClass {
	public KeysChangedDelegate keys_changed;
	public static delegate void KeysChangedDelegate(Gtk.Window window);
}
