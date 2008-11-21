using Wnck;
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
