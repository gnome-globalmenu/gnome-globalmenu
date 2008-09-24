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

	protected class Navigator:Gtk.Window {
		public Gtk.TreeView treeview {get; construct;}
		public Navigator(Gtk.TreeModel tree) {
			type = Gtk.WindowType.TOPLEVEL;
			treeview = new Gtk.TreeView.with_model(tree);
		}
		construct {
			gtk_tree_view_insert_column_with_data_func (treeview, 0, "Title", new Gtk.CellRendererText(), 
				(tree_column, c, model, iter) => {
					Gtk.CellRendererText cell = c as Gtk.CellRendererText;
					weak XML.Document.Tag node;
					model.get(iter, 0, out node, -1);
					weak string text = null;
					text = node.get("label");
					if(text == null) text = node.get("name");
					if(text == null) text = node.tag;
					cell.text = text;
					weak string visible = node.get("visible");
					if(visible == "false") 
						cell.foreground = "gray";
					else
						cell.foreground = "black";
					weak string enabled = node.get("enabled");
					if(enabled == "false")
						cell.background = "red";
					else
						cell.background = "white";
				}, null);
			gtk_tree_view_insert_column_with_data_func (treeview, 1, "XML", new Gtk.CellRendererText(), 
				(tree_column, cell, model, iter) => {
					weak XML.Node node;
					model.get(iter, 0, out node, -1);
					(cell as Gtk.CellRendererText).text = node.summary();
				}, null);
			Gtk.ScrolledWindow scrw = new Gtk.ScrolledWindow(null, null);
			scrw.add(treeview);
			this.add(scrw);
			treeview.row_activated +=(treeview, path, column) => {
				Gtk.TreeModel model = treeview.model;
				weak XML.Node node;
				Gtk.TreeIter iter;
				model.get_iter(out iter, path);
				model.get(iter, 0, out node, -1);
				if(node is Gnomenu.Document.Widget) {
					(node as Gnomenu.Document.Widget).activate();
				}
			};
		}
	}
}
