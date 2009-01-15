[CCode (cprefix = "GConf", lower_case_cprefix = "gconf_")]
namespace GConfCompat {
	[Compact]
	[CCode (copy_function = "gconf_schema_copy", cheader_filename = "gconf/gconf.h")]
	public class Schema : GConf.Schema {
		public GConf.ValueType get_type ();
	}
}

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
