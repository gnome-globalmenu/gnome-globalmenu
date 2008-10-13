using GLib;
using GLibCompat;
namespace GMarkupDoc {
	public class Section: GLib.Object, DocumentModel {
		private DocumentModel _document;
		public weak DocumentModel document  {
			get { return _document;} 
			construct {_document = value;}
		}
		public weak HashTable dict {
			get { return _document.dict; }
		}
		public weak string name_attr {
			get { return _document.name_attr; }
		}
		private weak Node _root;
		private bool disposed;
		private bool _root_dead;
		public weak Node root { get {return _root;} }
		public weak Node fake_root { 
			construct { 
				_root = value;
				_root.weak_ref(weak_ref_notify, this);
				_root_dead = false;
			}
		}
		public weak Node orphan { get {return document.orphan;} }
		public Section(DocumentModel document, GMarkupDoc.Node root) {
			this.document = document;
			this.fake_root = root;
		}
		private override void dispose(){
			if(!disposed && !_root_dead) {
				_root.weak_unref(weak_ref_notify, this);
				disposed = true;
			}
		}
		private bool is_inside(Node node) {
			for(weak Node n = node;
				n != null;
				n = n.parent) {
				if(n == root)
					return true;
			}
			return false;	
		}
		public Text CreateText(string text) {
			return document.CreateText(text);
		}
		public Special CreateSpecial(string text) {
			return document.CreateSpecial(text);
		}
		public Tag CreateTag(string tag) {
			return document.CreateTag(tag);
		}
		public Tag CreateTagWithAttributes(string tag, 
				string[] attr_names, string[] attr_values) {
			return document.CreateTagWithAttributes(tag, attr_names, attr_values);
		}
		public void activate(Node node, Quark detail) {
			this.document.activate(node, detail);
		}
		construct {
			disposed = false;
			document.updated += (d, n, prop) => {
				if(!_root_dead && is_inside(n)) this.updated(n, prop);
			};
			document.inserted += (d, p, n, pos) => {
				if(!_root_dead && is_inside(p)) this.inserted(p, n, pos);
			};
			document.removed += (d, p, n) => {
				if(!_root_dead && is_inside(p)) this.removed(p, n);
			};
			document.renamed += (d, node, p, n) => {
				if(!_root_dead && is_inside(node)) this.renamed(node, p, n);
			};
		}
		private static void weak_ref_notify(void* data, GLib.Object node) {
			Section _this = (Section) data;
			_this._root_dead = true;
			debug("A section is destroyed");
			_this.destroyed();
		}
	}

}
