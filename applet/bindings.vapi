public GConf.ValueType gconf_schema_get_type(GConf.Schema schema);
public GConf.Schema gconf_client_get_schema(GConf.Client client, string key) throws GLib.Error;

[CCode (cprefix = "Wnck", lower_case_cprefix = "wnck_")]
namespace WnckCompat {
	public class Screen : GLib.Object {
	[CCode (cheader_filename = "libwnck/libwnck.h")]
		public virtual signal void active_window_changed (Wnck.Window? previous_window);
	}

	public class Window : GLib.Object {
	[CCode (cheader_filename = "libwnck/libwnck.h")]
		public void get_geometry (out int x, out int y, out int width, out int height);
	}
}
public Gdk.Pixbuf gdk_pixbuf_scale_simple(Gdk.Pixbuf src, int width, int height, Gdk.InterpType interp_type);

namespace Bonobo {
	[CCode (cheader_filename = "bonobo/bonobo-main.h")]
	public static bool init ([CCode (array_length_pos = 0.9)] ref unowned string[] argv);
}
