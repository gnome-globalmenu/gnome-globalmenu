/***
 * Fixed in 0.5.2
 * didn't work in 0.5.1
 */
public static GLib.List<weak Gtk.Widget> gtk_container_get_children(Gtk.Container container);

/***
 * Not working since 0.5.1
 */
public Gdk.Window? gdk_window_foreign_new(ulong native); 
public weak Gdk.Window? gdk_window_lookup(ulong native);
Pango.FontDescription pango_font_description_from_string (string str);
public bool gdk_property_get (Gdk.Window window, 
		Gdk.Atom property, 
		Gdk.Atom type, 
		ulong offset, ulong length, 
		bool pdelete, 
		out Gdk.Atom actual_property_type, 
		out int actual_format, 
		out int actual_length, 
		out string data);

public bool gdk_property_change (Gdk.Window window, 
		Gdk.Atom property, 
		Gdk.Atom type, 
		int format,
		Gdk.PropMode mode,
		string data,
		int bytes_size);

/***
 * Missing 
 */
[CCode ( cname = "GDK_WINDOW_XID", cheader_filename="gdk/gdkx.h")]
public ulong gdk_window_xid(Gdk.Window window);


public static void g_markup_parse_context_push(GLib.MarkupParseContext context, GLib.MarkupParser parser, void* user_data);
public static void* g_markup_parse_context_pop(GLib.MarkupParseContext context);
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

