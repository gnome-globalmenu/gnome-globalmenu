/***
 * Fixed in 0.5.2
 * didn't work in 0.5.1
 */
public static GLib.List<weak Gtk.Widget> gtk_container_get_children(Gtk.Container container);
public static GLib.List<weak Gtk.Window> gtk_window_list_toplevels();

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

Gdk.Pixbuf gdk_pixbuf_from_pixdata(Gdk.Pixdata pixdata, bool copy_pixels) throws GLib.Error;
