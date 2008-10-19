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
								<object class = "GtkButton" id = "destroy">
									<property name ="label">Destroy</property>
								</object>
							</child>
						</object>
					</child>
				</object>
			</interface>""";

	public interface View: GLib.Object {
		public abstract weak DocumentModel? document {get; set;}
		public signal void activated(Node node, Quark quark);
	}
	public class ListView : GtkCompat.Container, View {
		private Gtk.TreeView treeview;
		private DocumentModel _document;
		private DocumentParser parser;
		private DocumentTreeAdapter adapter;
		private Gtk.Widget box;
		private Gtk.Builder builder;
		public weak DocumentModel? document {
			get {
				return _document;
			} set {
				if(document != null) {
					document.destroyed -= document_destroyed;
				}
				_document = value;
				if(document != null) {
					adapter = new DocumentTreeAdapter(document);
					parser = new DocumentParser(document);
					document.destroyed += document_destroyed;
				} else {
					adapter = null;
					parser = null;
				}
				treeview.set_model(adapter);
				
			}
		}
		private void document_destroyed(DocumentModel document) {
			this.document = null;
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
			builder.add_from_string(INTERFACE, -1);
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
			(get_widget("remove") as Gtk.Button).clicked += (widget) => {
				weak Node node = get_selected();
				if(node != null && node.parent != null) {
					node.parent.remove(node);
				}
			};
			(get_widget("add") as Gtk.Button).clicked += (widget) => {
				weak Node node = get_selected();
				if(node != null) node.append(document.CreateTag("tag"));
			};
			(get_widget("destroy") as Gtk.Button).clicked += (widget) => {
				weak Node node = get_selected();
				if(node != null) document.DestroyNode(node);
			};
		}
		private weak Node? get_selected() {
			Gtk.TreeSelection sel = treeview.get_selection();
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
