using GLib;
using Gtk;
using Gnomenu;
using GtkAQD;
using XML;
using DBus;
namespace Gnomenu {
	public class MenuBar : MenuView {
		private string _xid;
		private RemoteDocument serverdoc;
		private RemoteDocument clientdoc;
		private DBus.Connection conn;
		public weak string xid {
			set {
				message("showing doc: %s", value);
				this.document = null;
				this.clientdoc = null;
				_xid = value;
				if(value == null) return;
				weak XML.Document.Tag node = serverdoc.lookup(xid) as XML.Document.Tag;
				if(node == null) return;
				weak string bus = node.get("bus");
				message("at bus %s", bus);
				clientdoc = new Gnomenu.RemoteDocument(bus, "/org/gnome/GlobalMenu/Application");
				dynamic DBus.Object client = conn.get_object(bus, "/org/gnome/GlobalMenu/Application", "org.gnome.GlobalMenu.Client");
				string widget_name = client.QueryXID(xid);
				message("widget_name %s", widget_name);
				node = clientdoc.lookup(widget_name) as XML.Document.Tag;
				if(node == null) return;
				foreach(weak XML.Node c in node.children) {
					if(!(c is XML.Document.Tag)) continue;
					if((c as XML.Document.Tag).tag == "menubar") {
						message("menubar found");
						XML.Section section = new XML.Section(clientdoc, c);
						this.document = section;
					}
				}
			}
			get {
				return _xid;
			}
		}
		public MenuBar(string? xid) {
			this.xid = xid;
			this.local = true;
		}
		construct {
			serverdoc = new Gnomenu.RemoteDocument("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server");
			conn = Bus.get(DBus.BusType.SESSION);

		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Gtk.Window window = new Gtk.Window(WindowType.TOPLEVEL);
			MenuBar menubar = new MenuBar(args[1]);
			Gtk.Box box = new Gtk.HBox(false, 0);
			window.add(box);
			box.pack_start_defaults(new Gtk.Label("suck"));
			box.pack_start_defaults(menubar);
			window.show_all();
			loop.run();
			return 0;
		}
	}
}
