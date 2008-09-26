using GLib;
using Gtk;
using Gnomenu;
using XML;

namespace Gnomenu {
	public class Navigator: Gtk.Window {
		private Document document;
		private Parser parser;
		private Viewer viewer;
		public Navigator() {
			type = Gtk.WindowType.TOPLEVEL;
		}
		construct {
			document = new Gnomenu.Document();
			parser = new XML.Parser(document);
			viewer = new Gnomenu.Viewer(document);
			Gtk.Box box = new Gtk.VBox(false, 0);
			this.add(box);
			box.pack_start_defaults(viewer);
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
