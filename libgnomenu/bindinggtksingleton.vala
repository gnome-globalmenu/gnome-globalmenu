using GLib;
using Gnomenu;
using XML;
namespace GnomenuGtk {
	protected class Singleton {
		static Singleton _instance;
		public Client client;
		public weak GtkNodeFactory factory;
		private int unique_id;
		public int unique {
			get {
				unique_id++;
				return unique_id;
			}
		}
		public static Singleton instance() {
			if(_instance == null)
				_instance = new Singleton();
			return _instance;
		}
		private Singleton() {
			GtkNodeFactory factory = new GtkNodeFactory();
			this.factory = factory;
			client = new Client(factory);
			unique_id = 99;
		}
	}
}
