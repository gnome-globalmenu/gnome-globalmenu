using GLib;
using Gdk;
using Gtk;
using DBus;
using Markup;
namespace Gnomenu {
	[DBus (name = "org.gnome.GlobalMenu.Client")]
	public abstract class Client:GLib.Object {
		Connection conn;
		XML xml;
		XMLNode windows;
		string bus;
		public Client() {
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			dynamic DBus.Object dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
			
			string str = dbus.GetId();
			bus = "org.gnome.GlobalMenu.Applications." + str;
			uint r = dbus.RequestName (bus, (uint) 0);
			assert(r == DBus.RequestNameReply.PRIMARY_OWNER);
			conn.register_object("/org/gnome/GlobalMenu/Application", this);
			xml = new XML();
			windows = new XMLTagNode(xml.S("windows"));
			xml.root.children.append(windows);
		}
		public string QueryMenu(string menu){
			return query_menu(menu);
		}
		public string QueryWindow(string window) {
			return query_window(window);
		}
		public void ActivateItem(string item){
			activate_item(item);
		}
		protected dynamic DBus.Object get_server(){
			return conn.get_object("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server", "org.gnome.GlobalMenu.Server");
		}
		protected abstract virtual void activate_item(string item);
		protected abstract virtual string query_menu(string menu);
		protected abstract virtual string query_window(string window);
			
		protected bool register_window(string window) {
			try {
				get_server().RegisterWindow(this.bus, window);
				return true;
			} catch(GLib.Error e) {
				warning("%s", e.message);
				return false;
			}
		}
	}
}
