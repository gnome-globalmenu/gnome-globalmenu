using GLib;
using Gtk;
using GtkCompat;

namespace GMarkupDoc {

	public class ListView : GtkCompat.Container {
		private Gtk.TreeView treeview;
		private DocumentModel _document;
		private Parser parser;
		private DocumentTreeAdapter adapter;
		private Gtk.Widget box;
		private Gtk.Builder builder;
		public weak DocumentModel? document {
			get {
				return _document;
			} set {
				_document = value;
				if(document != null) {
					adapter = new DocumentTreeAdapter(document);
					parser = new Parser(document);
				} else {
					adapter = null;
					parser = null;
				}
				treeview.set_model(adapter);
				
			}
		}
		public ListView(DocumentModel? document) {
			this.document = document;
		}
		private override void forall(bool include_internal, GtkCompat.Callback cb, void* data){
			if(include_internal) cb(box, data);
		}
		private Gtk.Widget get_widget(string name) {
			return builder.get_object(name) as Gtk.Widget;
		}
		construct {
			this.set_flags(Gtk.WidgetFlags.NO_WINDOW);
			builder = new Gtk.Builder();
			builder.add_from_string("""
			<interface>
				<object class = "GtkVPaned" id = "box">
					<child>
						<object class = "GtkScrolledWindow" id = "scroll">
						</object>
						<packing>
							<property name = "resize">1</property>
							<property name = "shrink">1</property>
						</packing>
					</child>
					<child>
						<object class = "GtkVBox" id = "vbox">
							<child><object class = "GtkButton" id = "Add"/></child>
							<child><object class = "GtkButton" id = "Remove"/></child>
						</object>
					</child>
				</object>
			</interface>
			""", -1);		
			treeview = new Gtk.TreeView();
			(get_widget("scroll") as Gtk.ScrolledWindow).add(treeview);
			box = get_widget("box");
			box.show_all();
			box.set_parent(this);
			this.size_allocate += (widget, alloc) => {
				box.size_allocate(alloc);
			};
			this.size_request += (widget, req) => {
				box.size_request(req);
			};
			(treeview as GtkCompat.TreeView).insert_column_with_data_func (0, "Title", new Gtk.CellRendererText(), 
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
			Gtk.CellRendererText render = new Gtk.CellRendererText();
			render.edited += (render, spath, new_text) => {
				Gtk.TreePath path = new Gtk.TreePath.from_string(spath);
				Gtk.TreeModel model = treeview.model;
				Gtk.TreeIter iter;
				weak Node node;
				model.get_iter(out iter, path);
				model.get(iter, 0, out node, -1);
				message("%s", new_text);
				parser.update_tag(node as Tag, null, new_text);
			};
			(treeview as GtkCompat.TreeView).insert_column_with_data_func (1, "GMarkup", render,
				(tree_column, cell, model, iter) => {
					weak Node node;
					model.get(iter, 0, out node, -1);
					(cell as Gtk.CellRendererText).text = node.summary();
					(cell as Gtk.CellRendererText).editable = true;
					
				}, null);
			treeview.row_activated +=(treeview, path, column) => {
				Gtk.TreeModel model = treeview.model;
				weak Node node;
				Gtk.TreeIter iter;
				model.get_iter(out iter, path);
				model.get(iter, 0, out node, -1);
				debug("treeview activated");
				this.activated(node, 0);
			//	adapter.activate(node, 0);
			};
		}
		public signal void activated(Node node, Quark detail);
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
