using GLib;
using Gtk;
using Gnomenu;
using XML;
using DBus;

namespace Gnomenu {
	public class Navigator: Gtk.Window {
		private Document document;
		private Parser parser;
		private ListView viewer;
		private string ui;
		private Gtk.Builder builder;
		private Gtk.ComboBox selector;
		private dynamic DBus.Object remote;
		private dynamic DBus.Connection conn;

		public Navigator() {
			type = Gtk.WindowType.TOPLEVEL;
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			remote = conn.get_object("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server", "org.gnome.GlobalMenu.Document");
			remote.Inserted += remote_inserted;
			remote.Removed += remote_removed;
			remote.Updated += remote_updated;
			document = new Gnomenu.Document();
			parser = new XML.Parser(document);
			viewer = new ListView(document);

			builder = new Gtk.Builder();
			ui = """
			<interface>
			  <object class="GtkVBox" id="MainBox"/>
			</interface>
			""";
			builder.add_from_string(ui, -1);
			Gtk.Box box = builder.get_object("MainBox") as Gtk.Box;
			this.add(box);
			box.pack_start_defaults(viewer);

			string clients = remote.QueryRoot(0);
			print("%s\n", clients);
			parser.parse(clients);
			print("%s\n", document.root.to_string());

		}
		private void remote_inserted(dynamic DBus.Object remote, string parentname, string nodename, int pos) {
			weak XML.Node parent = document.lookup(parentname);
			XML.Node node = parser.parse_tag(remote.QueryNode(nodename, -1));
			parent.insert(node, pos);
		}
		private void remote_removed(dynamic DBus.Object remote, string parentname, string nodename) {
			weak XML.Node parent = document.lookup(parentname);
			weak XML.Node node = document.lookup(nodename);
			parent.remove(node);
		}
		private void remote_updated(dynamic DBus.Object remote, string nodename, string propname) {
			weak XML.Node node = document.lookup(nodename);
			XML.Document.Tag updated_node = parser.parse_tag(remote.QueryNode(nodename, 0));
			node.set(propname, updated_node.get(propname));
		}

		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Navigator navigator = new Navigator();
			navigator.show_all();
			loop.run();
			return 0;
		}
	}
}
