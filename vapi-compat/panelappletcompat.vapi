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

		public void add_preferences(string schema_dir) throws GLib.Error;
		public bool gconf_get_bool(string key) throws GLib.Error;
		public void gconf_set_bool(string key) throws GLib.Error;
		public int gconf_get_int(string key) throws GLib.Error;
		public void gconf_set_int(string key) throws GLib.Error;
	}
}
