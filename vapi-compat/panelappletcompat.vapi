using Panel;
[CCode (cheader_filename = "panel-applet.h", cprefix="Panel", lower_case_cprefix = "panel_" )]
namespace PanelCompat {
	public class Applet : Gtk.EventBox {
		public string get_preferences_key ();
		public void set_flags (AppletFlags flags);
		public static int factory_main (string iid, GLib.Type applet_type, AppletFactoryCallback callback);
		public void set_background_widget (Gtk.Widget widget);
		[NoArrayLength]
		public void setup_menu (string xml, BonoboUI.Verb[] verb_list, void* data);
		public signal void change_background (AppletBackgroundType type, Gdk.Color? color, Gdk.Pixmap? pixmap);
	}
}
