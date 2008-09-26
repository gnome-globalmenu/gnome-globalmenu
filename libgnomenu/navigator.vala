using GLib;
using Gtk;
using Gnomenu;
using XML;
using DBus;

namespace Gnomenu {
	public class Navigator: Gtk.Window {
		private Document document;
		private Parser parser;
		private Viewer viewer;
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
			viewer = new Gnomenu.Viewer(document);
			builder = new Gtk.Builder();
			ui = """
			<interface>
			  <object class="GtkVBox" id="MainBox">
				<child>
				  <object class="GtkComboBox" id="WindowSelector" />
				</child>
			  </object>
			</interface>
			""";
			builder.add_from_string(ui, -1);
			Gtk.Box box = builder.get_object("MainBox") as Gtk.Box;
			this.add(box);
			box.pack_start_defaults(viewer);
			selector = builder.get_object("WindowSelector") as Gtk.ComboBox;
			
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
