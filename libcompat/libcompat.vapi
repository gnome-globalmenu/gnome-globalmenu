[CCode (cheader_filename="libcompat/libcompat.h")]
[NoArrayLength]
public static bool g_markup_collect_attributes(string element_name, [CCode (array_length = false)] string[] attribute_names, [CCode (array_length = false)] string[] attribute_values, out GLib.Error? error, GMarkupCollectType type, string first_name, ...);

[CCode (cprefix = "G_MARKUP_COLLECT_", has_type_id = false)]
[CCode (cheader_filename="libcompat/libcompat.h")]
public enum GMarkupCollectType {
	INVALID,
	STRING,
	STRDUP,
	BOOLEAN,
	TRISTATE,
	OPTIONAL
}
