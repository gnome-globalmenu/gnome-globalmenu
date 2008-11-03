using GLib;
using GLibCompat;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public class Section: GLib.Object, DocumentModel {
		private DocumentModel _document;
		public weak DocumentModel document  {
			get { return _document;} 
			construct {_document = value;}
		}
		public weak HashTable node_pool {
			get { return _document.node_pool; }
		}
		public int unique {get {return _document.unique;}}
		private Node _root;
		private bool disposed;
		public weak Node root { get {return _root;} }
		public Section(DocumentModel document, GMarkup.Node root) {
			this.document = document;
			this._root = root;
		}
		private bool is_inside(Node node) {
			if(node.document != document) return false;
			for(weak Node n = node;
				n != null;
				n = n.parent) {
				if(n == root)
					return true;
			}
			message("is_inside = false");
			return false;	
		}
		public weak Node createNode(NodeType type) {
			return document.createNode(type);
		}
		construct {
			document.updated += (d, n, prop) => {
				if(is_inside(n)) this.updated(n, prop);
			};
			document.inserted += (d, p, n, pos) => {
				if(is_inside(p)) this.inserted(p, n, pos);
			};
			document.removed += (d, p, n) => {
				if(is_inside(p)) this.removed(p, n);
				if(n == root) {
					foreach(weak Node child in n.children) {
						this.removed(n, child);
					}
				}
			};
		}
	}

}
