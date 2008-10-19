using GLib;
using Gtk;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public class DocumentTreeAdapter: GLib.Object, DocumentModel, Gtk.TreeModel {
		private Gtk.TreeStore tree;
		private DocumentModel _document;
		public weak DocumentModel document {get{return _document;} construct{_document = value;}}
		private HashTable<weak Node, weak TreeIterBox> iterbox_hash;
		public DocumentTreeAdapter(DocumentModel document) {
			this.document = document;
		}	
		public weak Node root {get {return document.root;}}
		public weak Node orphan {get {return document.orphan;}}
		public weak HashTable<weak string, weak Node> dict { get{return document.dict;}}
		public weak string name_attr {get {return document.name_attr;}}
		private void add_to_tree(Node node) {
			this.document.transverse(node, (node) => {
					TreeIterBox box = new TreeIterBox();
					if(node == this.document.root ||
					  node == this.document.orphan
					) {
						this.tree.insert(out box.iter, null, -1);
					} else {
						this.tree.insert(out box.iter, get_iterbox(node.parent).iter, node.parent.index(node));
					}
					set_iterbox(node, box);
					this.tree.set(box.iter, 0, node, -1);
				});
		}
		construct {
			this.tree = new Gtk.TreeStore(1, typeof(constpointer));
			this.iterbox_hash = new HashTable<weak Node, weak TreeIterBox>.full(direct_hash, direct_equal, null, g_object_unref);
			this.add_to_tree(root);
			this.add_to_tree(orphan);
			this.document.inserted += (document, parent, child, pos) => {
				this.add_to_tree(child);
				this.inserted(parent, child, pos);
			};
			this.document.removed += (document, parent, child) => {
				this.tree.remove(get_iterbox(child).iter);
				iterbox_hash.remove(child);
				this.removed(parent, child);
			};
			this.document.updated += (document, node, prop) => {
				Gtk.TreeIter iter = get_iterbox(node).iter;
				this.tree.row_changed(tree.get_path(iter), iter);
				this.updated(node, prop);
			};
			this.document.renamed += (document, node, oldname, newname) => {
				Gtk.TreeIter iter = get_iterbox(node).iter;
				this.tree.row_changed(tree.get_path(iter), iter);
				this.renamed(node, oldname, newname);
			};
			tree.row_changed += (o, p, i) => { row_changed(p, i);};
			tree.row_deleted += (o, p) => { row_deleted(p);};
			tree.row_has_child_toggled += (o, p, i) => { row_has_child_toggled(p, i);};
			tree.row_inserted += (o, p, i) => { row_inserted(p, i);};
			tree.rows_reordered += (o, p, i, n) => {rows_reordered(p, i, n);};
		}
		public void activate(Node node, Quark detail) {
			this.document.activate(node, detail);
		}
		public GLib.Type get_column_type (int index_) {
			return this.tree.get_column_type(index_);
		}
		public Gtk.TreeModelFlags get_flags () {
			return this.tree.get_flags();
		}
		public bool get_iter (out Gtk.TreeIter iter, Gtk.TreePath path){
			return this.tree.get_iter(out iter, path);
		}
		public int get_n_columns () {
			return this.tree.get_n_columns();
		}
		public Gtk.TreePath get_path (Gtk.TreeIter iter) {
			return this.tree.get_path(iter);
		}
		public void get_value (Gtk.TreeIter iter, int column, ref GLib.Value value) {
			this.tree.get_value(iter, column, ref value);
		}
		public bool iter_children (out Gtk.TreeIter iter, Gtk.TreeIter? parent) {
			return this.tree.iter_children(out iter, parent);
		}
		public bool iter_has_child (Gtk.TreeIter iter) {
			return this.tree.iter_has_child(iter);
		}
		public int iter_n_children (Gtk.TreeIter? iter) {
			return this.tree.iter_n_children(iter);
		}
		public bool iter_next (ref Gtk.TreeIter iter) {
			return this.tree.iter_next(ref iter);
		}
		public bool iter_nth_child (out Gtk.TreeIter iter, Gtk.TreeIter? parent, int n) {
			return this.tree.iter_nth_child(out iter, parent, n);
		}
		public bool iter_parent (out Gtk.TreeIter iter, Gtk.TreeIter child) {
			return this.tree.iter_parent(out iter, child);
		}
		public void ref_node (Gtk.TreeIter iter) {
			this.tree.ref_node(iter);	
		}
		public void unref_node (Gtk.TreeIter iter) {
			this.tree.unref_node(iter);	
		}

		private class TreeIterBox:GLib.Object {
			public Gtk.TreeIter iter;
		}
		private weak TreeIterBox get_iterbox(Node node) {
			return this.iterbox_hash.lookup(node);
		}
		private void set_iterbox(Node node, TreeIterBox iterbox) {
			this.iterbox_hash.insert(node, (TreeIterBox) iterbox.ref());
		}
	}
}
