using GLib;
public static void g_markup_parse_context_push(MarkupParseContext context, MarkupParser parser, void* user_data);
public static void* g_markup_parse_context_pop(MarkupParseContext context);
[NoArrayLength]
public static bool g_markup_collect_attributes(string element_name, string[] attribute_names, string[] attribute_values, out GLib.Error? error, GMarkupCollectType type, string first_name, ...);

[CCode (cprefix = "G_MARKUP_COLLECT_", has_type_id = false)]
public enum GMarkupCollectType {
	INVALID,
	STRING,
	STRDUP,
	BOOLEAN,
	TRISTATE,
	OPTIONAL
}
