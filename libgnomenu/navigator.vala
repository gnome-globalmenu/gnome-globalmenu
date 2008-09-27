using GLib;
using Gtk;
using Gnomenu;
using XML;
using DBus;

namespace Gnomenu {
	public class Navigator: Gtk.Window {
		private Document document;
		private Document serverdocument;
		private Parser parser;
		private Parser serverparser;
		private Viewer viewer;
		private Viewer serverviewer;
		private string ui;
		private Gtk.Builder builder;
		private Gtk.ComboBox selector;
		private dynamic DBus.Object server;
		private dynamic DBus.Connection conn;

		public Navigator() {
			type = Gtk.WindowType.TOPLEVEL;
		}
		construct {
			conn = Bus.get(DBus.BusType.SESSION);
			server = conn.get_object("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server", "org.gnome.GlobalMenu.Server");

			document = new Gnomenu.Document();
			parser = new XML.Parser(document);
			serverdocument = new Gnomenu.Document();
			serverparser = new XML.Parser(serverdocument);
			viewer = new Gnomenu.Viewer(document);
			serverviewer = new Viewer(serverdocument);
			builder = new Gtk.Builder();
			ui = """
			<interface>
			  <object class="GtkVBox" id="MainBox"/>
			</interface>
			""";
			builder.add_from_string(ui, -1);
			Gtk.Box box = builder.get_object("MainBox") as Gtk.Box;
			this.add(box);
			box.pack_start_defaults(serverviewer);
			box.pack_start_defaults(viewer);

			string clients = server.QueryWindows();
			print("%s\n", clients);
			serverparser.parse(clients);
			print("%s\n", serverdocument.root.to_string());

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
