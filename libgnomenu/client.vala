using GLib;
using Gtk;
using DBus;
namespace Gnomenu {
	public static void bind_menu(Gtk.Window window, Gtk.Widget widget) {

	}
	public static void unbind_menu(Gtk.Window window, Gtk.Widget widget) {

	}
	public static void wrap_widget(Gtk.Widget widget) {
		
	}
	public static void unwrap_widget(Gtk.Widget widget) {

	}
	[DBus (name = "org.gnome.GlobalMenu.Client")]
	public class Client:GLib.Object {
		Connection conn;
		public Client() {
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dynamic DBus.Object bus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			
			string str = bus.GetId();
			message("here");
			message("%s", str);
			uint r = bus.RequestName ("org.gnome.GlobalMenu.Applications." + str, (uint) 0);
			assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
			conn.register_object("/org/gnome/GlobalMenu/Application", this);
		}
		public string QueryMenu(string menu){
			return "";
		}
		public void ActivateItem(string item){
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Client c = new Client();
			loop.run();
			return 0;
		}
	}
}
