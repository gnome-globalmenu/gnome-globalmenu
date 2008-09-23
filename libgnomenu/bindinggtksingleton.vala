using GLib;
using Gtk;
using Gnomenu;
using XML;
namespace GnomenuGtk {
	[CCode (cname="gtk_tree_view_insert_column_with_data_func")]
	public extern int gtk_tree_view_insert_column_with_data_func(
		Gtk.TreeView tw, int pos, 
		string title, Gtk.CellRenderer cell, 
		Gtk.TreeCellDataFunc func, 
		GLib.DestroyNotify? dnotify);


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
			window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			Gtk.TreeView tv = new Gtk.TreeView.with_model(factory.tree);
			gtk_tree_view_insert_column_with_data_func (tv, 0, "node", new Gtk.CellRendererText(), 
				(tree_column, cell, model, iter) => {
					weak XML.Node node;
					model.get(iter, 0, out node, -1);
					(cell as Gtk.CellRendererText).text = node.summary();
				}, null);
							
			window.add(tv);
			window.show_all();
		}
	}
}
