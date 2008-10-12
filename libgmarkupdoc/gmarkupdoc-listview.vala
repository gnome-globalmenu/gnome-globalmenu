using GLib;
using Gtk;
using GtkCompat;

namespace GMarkupDoc {
	[CCode (cname="gtk_tree_view_insert_column_with_data_func")]
	public extern int gtk_tree_view_insert_column_with_data_func(
		Gtk.TreeView tw, int pos, 
		string title, Gtk.CellRenderer cell, 
		Gtk.TreeCellDataFunc func, 
		GLib.DestroyNotify? dnotify);

	public class ListView : GtkCompat.Container {
		private Gtk.ScrolledWindow scroll;
		private Gtk.TreeView treeview;
		private DocumentModel _document;
		private DocumentTreeAdapter adapter;
		public weak DocumentModel? document {
			get {
				return _document;
			} set {
				_document = value;
				if(document != null) {
					adapter = new DocumentTreeAdapter(document);
				} else
					adapter = null;
				treeview.set_model(adapter);
				
			}
		}
		public ListView(DocumentModel? document) {
			this.document = document;
		}
		private override void forall(bool include_internal, GtkCompat.Callback cb, void* data){
			if(include_internal) cb(scroll, data);
		}
		construct {
			message("constructing the viewer");
			this.set_flags(Gtk.WidgetFlags.NO_WINDOW);
			treeview = new Gtk.TreeView();
			scroll = new Gtk.ScrolledWindow(null, null);
			scroll.visible = true;
			treeview.visible = true;
			scroll.add(treeview);
			scroll.set_parent(this);
			this.size_allocate += (widget, alloc) => {
				scroll.size_allocate(alloc);
			};
			this.size_request += (widget, req) => {
				scroll.size_request(req);
			};
			gtk_tree_view_insert_column_with_data_func (treeview, 0, "Title", new Gtk.CellRendererText(), 
				(tree_column, c, model, iter) => {
					Gtk.CellRendererText cell = c as Gtk.CellRendererText;
					weak Node node;
					model.get(iter, 0, out node, -1);
					weak string text = null;
					text = node.name;
					if(text == null) {
						if(node is Tag)
							text = (node as Tag).tag;
						if(node is Text)
							text = "TEXT";
						if(node is Special)
							text = "SPECAL";
						if(node is Root)
							text = "ROOT";
					}
					cell.text = text;
					/*
					weak string visible = node.get("visible");
					if(visible == "false") 
						cell.foreground = "gray";
					else
						cell.foreground = "black";
					weak string enabled = node.get("sensitive");
					if(enabled == "false")
						cell.background = "red";
					else
						cell.background = "white";
					*/
				}, null);
			gtk_tree_view_insert_column_with_data_func (treeview, 1, "GMarkup", new Gtk.CellRendererText(), 
				(tree_column, cell, model, iter) => {
					weak Node node;
					model.get(iter, 0, out node, -1);
					(cell as Gtk.CellRendererText).text = "'" + node.summary() + "'";
				}, null);
			treeview.row_activated +=(treeview, path, column) => {
				Gtk.TreeModel model = treeview.model;
				weak Node node;
				Gtk.TreeIter iter;
				model.get_iter(out iter, path);
				model.get(iter, 0, out node, -1);
				adapter.activate(node, 0);
			};
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Document document = new Document();
			GMarkupDoc.Parser parser = new GMarkupDoc.Parser(document);
			ListView c = new ListView(document);
			parser.parse(
"""
<html><title>title</title>
<body name="body">
<div name="header">
	<h1> This is a header</h1>
</div>
<div name="content"></div>
<div name="tail"></div>
</body>
""");
			Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			window.add(c);
			window.show_all();
			loop.run();
			return 0;
		}
		
	}
}
