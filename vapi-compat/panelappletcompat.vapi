using Panel;
[CCode (cheader_filename = "panel-applet.h", cprefix="Panel", lower_case_cprefix = "panel_" )]
namespace PanelCompat {
	public class Applet : Gtk.EventBox {
		public signal void change_background (AppletBackgroundType type, Gdk.Color? color, Gdk.Pixmap? pixmap);
	}
}
