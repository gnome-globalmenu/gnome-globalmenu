using GLib;
using Gtk;
using Gnomenu;
using XML;
namespace GnomenuGtk {

	protected class Singleton {
		static Singleton _instance;
		public Client client;
		public weak NodeFactory factory;
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
		private Singleton() {
			NodeFactory factory = new NodeFactory();
			this.factory = factory;
			client = new Client(factory);
			unique_id = 99;
			window = new Navigator(factory.tree);
			window.show_all();
		}
	}
}
