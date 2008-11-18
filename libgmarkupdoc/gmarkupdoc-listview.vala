using GLib;
using Gtk;
using GtkCompat;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	private const string INTERFACE = """
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
						<object class = "GtkHBox" id = "vbox">
							<child>
								<object class = "GtkButton" id = "add">
									<property name ="label">Add</property>
								</object>
							</child>
							<child>
								<object class = "GtkButton" id = "remove">
									<property name ="label">Remove</property>
								</object>
							</child>
							<child>
								<object class = "GtkButton" id = "check">
									<property name ="label">Check</property>
								</object>
							</child>
						</object>
					</child>
				</object>
			</interface>""";

	public class ListView : GtkCompat.Container, View {
		private Gtk.TreeView _treeview;
		private Document _document;
		private DocumentParser _parser;
		private NodeTreeAdapter _adapter;
		private Gtk.Widget _box;
		private Gtk.Builder _builder;
		private Node _root;
		public weak Node? root {
			get {
				return _root;
			} 
			set {
				_root = value;
				if(root != null) {
					_adapter = new NodeTreeAdapter(root);
					_document = root.document;
					_parser = new DocumentParser(_document);
				} else {
					_adapter = null;
					_parser = null;
					_document = null;
				}
				_treeview.set_model(_adapter);
			}
		}
		public ListView() {
			this.root = null;
		}
		public ListView.with_root(Node root) {
			this.root = root;
		}
		private override void forall(bool include_internal, GtkCompat.Callback cb, void* data){
			if(include_internal) cb(_box, data);
		}
		private Gtk.Widget get_widget(string name) {
			return _builder.get_object(name) as Gtk.Widget;
		}
		public signal void activated(Node node);
		construct {
			this.set_flags(Gtk.WidgetFlags.NO_WINDOW);
			_builder = new Gtk.Builder();
			try {
				_builder.add_from_string(INTERFACE, -1);
			} catch (GLib.Error e) {
				critical("%s", e.message);
			}
			_treeview = new Gtk.TreeView();
			(get_widget("scroll") as Gtk.ScrolledWindow).add(_treeview);
			_box = get_widget("box");
			_box.show_all();
			_box.set_parent(this);
			this.size_allocate += (widget, alloc) => {
				_box.size_allocate(alloc);
			};
			this.size_request += (widget, req) => {
				_box.size_request(req);
			};
			(_treeview as GtkCompat.TreeView).insert_column_with_data_func (0, "ID", new Gtk.CellRendererText(), 
				(tree_column, c, model, iter) => {
					Gtk.CellRendererText cell = c as Gtk.CellRendererText;
					weak Node node;
					model.get(iter, 0, out node, -1);
					cell.text = node.id.to_string();
				}, null);
			(_treeview as GtkCompat.TreeView).insert_column_with_data_func (1, "Title", new Gtk.CellRendererText(), 
				(tree_column, c, model, iter) => {
					Gtk.CellRendererText cell = c as Gtk.CellRendererText;
					weak Node node;
					model.get(iter, 0, out node, -1);
					cell.text = node.name;
				}, null);
			Gtk.CellRendererText render = new Gtk.CellRendererText();
			render.edited += (render, spath, new_text) => {
				Gtk.TreePath path = new Gtk.TreePath.from_string(spath);
				Gtk.TreeModel model = _treeview.model;
				Gtk.TreeIter iter;
				weak Node node;
				model.get_iter(out iter, path);
				model.get(iter, 0, out node, -1);
				string meta_str = "<gmarkup:meta id=\"%d\" name=\"%s\">%s</gmarkup:meta>".printf(
						node.id, node.name, new_text);
				message("%s", meta_str);
				Node meta = _parser.parse(meta_str);
				message("%s", meta.to_string());
				_document.mergeMeta(meta, _root, null);
			};
			(_treeview as GtkCompat.TreeView).insert_column_with_data_func (2, "GMarkup", render,
				(tree_column, cell, model, iter) => {
					weak Node node;
					model.get(iter, 0, out node, -1);
					(cell as Gtk.CellRendererText).text = node.to_string(false);
					(cell as Gtk.CellRendererText).editable = true;
					
				}, null);

			_treeview.row_activated +=(treeview, path, column) => {
				Gtk.TreeModel model = treeview.model;
				weak Node node;
				Gtk.TreeIter iter;
				model.get_iter(out iter, path);
				model.get(iter, 0, out node, -1);
				debug("treeview activated");
				this.activated(node);
			};
			(get_widget("remove") as Gtk.Button).clicked += (widget) => {
				weak Node node = get_selected();
				if(node != null && node.parent != null) {
					node.parent.remove(node);
					_document.destroyNode(node);
				}
			};
			(get_widget("add") as Gtk.Button).clicked += (widget) => {
				weak Node node = get_selected();
				if(node != null) node.append(_document.createElement("tag"));
			};
			(get_widget("check") as Gtk.Button).clicked += (widget) => {
				_document.memcheck();
			};
		}
		private weak Node? get_selected() {
			Gtk.TreeSelection sel = _treeview.get_selection();
			weak Gtk.TreeModel model;
			weak Gtk.TreeIter iter;
			if(sel.get_selected(out model, out iter)) {
				weak Node node;
				model.get(iter, 0, out node, -1);
				return node;
			}
			return null;
		}
		public static int test(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Document document = new Document();
			GMarkup.DocumentParser parser = new GMarkup.DocumentParser(document);
			ListView c = new ListView.with_root(document);
			Node fragment = parser.parse(
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
			document.append(fragment);
			ListView s = new ListView.with_root(document.firstChild);
			Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
			Gtk.Box box = new Gtk.VBox(false, 0);
			box.pack_start_defaults(c);
			box.pack_start_defaults(s);
			window.add(box);
			window.show_all();
			loop.run();
			return 0;
		}
		
	}
}
