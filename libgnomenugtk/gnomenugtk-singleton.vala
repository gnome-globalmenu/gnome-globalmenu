using GLib;
using Gtk;
using Gnomenu;
using GMarkup;
namespace GnomenuGtk {

	protected class Singleton {
		static Singleton _instance;
		public Client client;
		public weak Document document;
		private int unique_id;
		public int unique {
			get {
				unique_id++;
				return unique_id;
			}
		}
		private Gtk.Window window;
		public static Singleton instance() {
			if(_instance == null)
				_instance = new Singleton();
			return _instance;
		}
		private static void activate_node(GMarkup.Node node) {
			message("Activate node %s", node.name);
				weak Gtk.Widget widget = (node as Document.Widget).gtk;
				if(widget is Gtk.TearoffMenuItem) return;
				if(widget is Gtk.MenuItem) 
					(widget as Gtk.MenuItem).activate();
				if(widget is GtkAQD.MenuBar) {
					bool local = (widget as GtkAQD.MenuBar).local;
					(widget as GtkAQD.MenuBar).local = !local;
				}

		}
		private Singleton() {
			Document document = new Document();
			this.document = document;
			client = new Client(document);
			client.activated += (client, window, node) => {
					activate_node(node);
			};

			unique_id = 99;
			if(Environment.get_variable("GNOMENU_FUN") != null) {
				window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
				window.accept_focus = false;
				ListView viewer = new ListView(document);
				viewer.activated += (viewer, node) => {
					activate_node(node);
				};
				window.add(viewer);
				window.show_all();
			}
		}
	}
}
