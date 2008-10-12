using GLib;
using Gtk;
namespace GMarkupDoc {
	public class Document: GLib.Object, DocumentModel {
		private Root _root;
		private HashTable <weak string, weak Node> _dict;
		public weak Node root {get {return _root;}}
		public weak HashTable<weak string, weak Node> dict { get{return _dict;} }
		public weak string name_attr {get {return S("name");}}
		construct {
			_dict = new HashTable<weak string, weak Node>(str_hash, str_equal);
			_root = new Root(this);
			this.activated += (document, node, detail) => {
				message("a node %s is activated", node.name);
			};
		}
	}
	public class DocumentTreeAdapter: GLib.Object, DocumentModel, Gtk.TreeModel {
		private Gtk.TreeStore tree;
		private DocumentModel _document;
		public weak DocumentModel document {get{return _document;} construct{_document = value;}}
		private HashTable<weak Node, weak TreeIterBox> iterbox_hash;
		public DocumentTreeAdapter(DocumentModel document) {
			this.document = document;
		}	
		public weak Node root {get {return document.root;}}
		public weak HashTable<weak string, weak Node> dict { get{return document.dict;}}
		public weak string name_attr {get {return document.name_attr;}}
		construct {
			this.tree = new Gtk.TreeStore(1, typeof(constpointer));
			this.iterbox_hash = new HashTable<weak Node, weak TreeIterBox>.full(direct_hash, direct_equal, null, g_object_unref);
			this.document.transverse(root, (node) => {
					TreeIterBox box = new TreeIterBox();
					if(node == this.document.root) {
						this.tree.insert(out box.iter, null, -1);
					} else {
						this.tree.insert(out box.iter, get_iterbox(node.parent).iter, node.parent.index(node));
					}
					set_iterbox(node, box);
					this.tree.set(box.iter, 0, node, -1);
				});
			this.document.inserted += (document, parent, child, pos) => {
				TreeIterBox box = new TreeIterBox();
				weak TreeIterBox parent_iterbox = get_iterbox(parent);
				this.tree.insert(out box.iter, parent_iterbox.iter, pos);
				set_iterbox(child, box);
				this.tree.set(box.iter, 0, child, -1);
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
/*
	public class Document: GLib.Object, DocumentModel, Gtk.TreeModel {
		public class NamedTag: Tag {
			public Gtk.TreeIter iter;
			private NamedTag(Document document, string tag) {
				this.document = document;
				this.tag = document.S(tag);
			}
			construct {
				this.parent_set += (o, old) => {
					if(this.parent == null) return;
					if(this.parent is NamedTag) {
						(document as Document).treestore.insert(out this.iter, (this.parent as NamedTag).iter, this.parent.index(this));
					} else {
						(document as Document).treestore.insert(out this.iter, null, 0);
					}
					(document as Document).treestore.set(this.iter, 0, this, -1);
				};
			}
			public override void dispose() {
				base.dispose();
				(this.document as Document).treestore.remove(this.iter);
			}
			~NamedTag(){
				//debug("NamedTag %s is removed", name);
			}
			public virtual void activate() {
				(document as Document).activated(this);
				//debug("NamedTag %s is activated", name);
			}
		}
		public Gtk.TreeStore treestore;
		private Root _root;
		private HashTable <weak string, weak NamedTag> _dict;
		public weak Node root {get {return _root;}}
		public weak HashTable<weak string, weak Node> dict { get{return _dict;} }
		public weak string name_attr {get {return S("name");}}
		construct {
			_dict = new HashTable<weak string, weak Tag>(str_hash, str_equal);
			_root = new Root(this);
			treestore = new Gtk.TreeStore(1, typeof(constpointer));
			this.updated += (o, node) => {
				if(node is NamedTag) {
					weak NamedTag w = node as NamedTag;
					row_changed(this.treestore.get_path(w.iter), w.iter);
				}
			};
			treestore.row_changed += (o, p, i) => { row_changed(p, i);};
			treestore.row_deleted += (o, p) => { row_deleted(p);};
			treestore.row_has_child_toggled += (o, p, i) => { row_has_child_toggled(p, i);};
			treestore.row_inserted += (o, p, i) => { row_inserted(p, i);};
			treestore.rows_reordered += (o, p, i, n) => {rows_reordered(p, i, n);};
		}
		public virtual Tag CreateTag(string tag) {
			Tag t = new NamedTag(this, tag);
			return t;
		}
		public virtual weak Node? lookup(string? name) { 
			if(name == null) return null;
			return dict.lookup(name);
		}
		public signal void activated(NamedTag node);
		public GLib.Type get_column_type (int index_) {
			return treestore.get_column_type(index_);
		}
		public Gtk.TreeModelFlags get_flags () {
			return treestore.get_flags();
		}
		public bool get_iter (out Gtk.TreeIter iter, Gtk.TreePath path){
			return treestore.get_iter(out iter, path);
		}
		public int get_n_columns () {
			return treestore.get_n_columns();
		}
		public Gtk.TreePath get_path (Gtk.TreeIter iter) {
			return treestore.get_path(iter);
		}
		public void get_value (Gtk.TreeIter iter, int column, ref GLib.Value value) {
			treestore.get_value(iter, column, ref value);
		}
		public bool iter_children (out Gtk.TreeIter iter, Gtk.TreeIter? parent) {
			return treestore.iter_children(out iter, parent);
		}
		public bool iter_has_child (Gtk.TreeIter iter) {
			return treestore.iter_has_child(iter);
		}
		public int iter_n_children (Gtk.TreeIter? iter) {
			return treestore.iter_n_children(iter);
		}
		public bool iter_next (ref Gtk.TreeIter iter) {
			return treestore.iter_next(ref iter);
		}
		public bool iter_nth_child (out Gtk.TreeIter iter, Gtk.TreeIter? parent, int n) {
			return treestore.iter_nth_child(out iter, parent, n);
		}
		public bool iter_parent (out Gtk.TreeIter iter, Gtk.TreeIter child) {
			return treestore.iter_parent(out iter, child);
		}
		public void ref_node (Gtk.TreeIter iter) {
			treestore.ref_node(iter);	
		}
		public void unref_node (Gtk.TreeIter iter) {
			treestore.unref_node(iter);	
		}
	}
*/
}
