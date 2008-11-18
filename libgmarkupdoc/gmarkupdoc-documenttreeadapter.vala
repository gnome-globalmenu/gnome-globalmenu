using GLib;
using Gtk;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public class NodeTreeAdapter: Node, Gtk.TreeModel {
		private Gtk.TreeStore _tree;
		private Document _document;
		private Node _root;
		public weak Node root {
			get {
				return _root;
			}
			construct {
				_root = value;
				_document = _root.document;
			}
		
		}
		private HashTable<weak Node, weak TreeIterBox> _iterbox_hash;
		public NodeTreeAdapter(Node root) {
			this.root = root;
		}	
		private void add_to_tree(Node node) {
			node.transverse((node) => {
					message("%s", node.name);
					TreeIterBox box = new TreeIterBox();
					if(node == _root) {
						_tree.insert(out box.iter, null, -1);
					} else {
						_tree.insert(out box.iter, get_iterbox(node.parent).iter, node.parent.index(node));
					}
					set_iterbox(node, box);
					_tree.set(box.iter, 0, node, -1);
				});
		}
		construct {
			_tree = new Gtk.TreeStore(1, typeof(constpointer));
			_iterbox_hash = new HashTable<weak Node, weak TreeIterBox>.full(direct_hash, direct_equal, null, g_object_unref);
			add_to_tree(_root);
			_document.inserted += (document, parent, child, refnode) => {
				if(!_root.hasChild(parent)) return;
				message("inserted %s %s", parent.name, child.name);
				add_to_tree(child);
			};
			_document.removed += (document, parent, child) => {
				if(child.hasChild(_root)) {
					foreach(weak Node child in _root.children) {
						_tree.remove(get_iterbox(child).iter);
						_iterbox_hash.remove(child);
					}
					return;
				}
				if(!_root.hasChild(parent)) return;
				_tree.remove(get_iterbox(child).iter);
				_iterbox_hash.remove(child);
			};
			_document.updated += (document, node, prop) => {
				if(!_root.hasChild(node)) return;
				Gtk.TreeIter iter = get_iterbox(node).iter;
				_tree.row_changed(_tree.get_path(iter), iter);
			};
			_tree.row_changed += (o, p, i) => { row_changed(p, i);};
			_tree.row_deleted += (o, p) => { row_deleted(p);};
			_tree.row_has_child_toggled += (o, p, i) => { row_has_child_toggled(p, i);};
			_tree.row_inserted += (o, p, i) => { row_inserted(p, i);};
			_tree.rows_reordered += (o, p, i, n) => {rows_reordered(p, i, n);};
		}
		public GLib.Type get_column_type (int index_) {
			return _tree.get_column_type(index_);
		}
		public Gtk.TreeModelFlags get_flags () {
			return _tree.get_flags();
		}
		public bool get_iter (out Gtk.TreeIter iter, Gtk.TreePath path){
			return _tree.get_iter(out iter, path);
		}
		public int get_n_columns () {
			return _tree.get_n_columns();
		}
		public Gtk.TreePath get_path (Gtk.TreeIter iter) {
			return _tree.get_path(iter);
		}
		public void get_value (Gtk.TreeIter iter, int column, ref GLib.Value value) {
			_tree.get_value(iter, column, ref value);
		}
		public bool iter_children (out Gtk.TreeIter iter, Gtk.TreeIter? parent) {
			return _tree.iter_children(out iter, parent);
		}
		public bool iter_has_child (Gtk.TreeIter iter) {
			return _tree.iter_has_child(iter);
		}
		public int iter_n_children (Gtk.TreeIter? iter) {
			return _tree.iter_n_children(iter);
		}
		public bool iter_next (ref Gtk.TreeIter iter) {
			return _tree.iter_next(ref iter);
		}
		public bool iter_nth_child (out Gtk.TreeIter iter, Gtk.TreeIter? parent, int n) {
			return _tree.iter_nth_child(out iter, parent, n);
		}
		public bool iter_parent (out Gtk.TreeIter iter, Gtk.TreeIter child) {
			return _tree.iter_parent(out iter, child);
		}
		public void ref_node (Gtk.TreeIter iter) {
			_tree.ref_node(iter);	
		}
		public void unref_node (Gtk.TreeIter iter) {
			_tree.unref_node(iter);	
		}

		private class TreeIterBox:GLib.Object {
			public Gtk.TreeIter iter;
		}
		private weak TreeIterBox get_iterbox(Node node) {
			return _iterbox_hash.lookup(node);
		}
		private void set_iterbox(Node node, TreeIterBox iterbox) {
			_iterbox_hash.insert(node, (TreeIterBox) iterbox.ref());
		}
	}
}
