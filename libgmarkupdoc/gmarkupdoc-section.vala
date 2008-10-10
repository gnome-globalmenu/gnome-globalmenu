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
		private Node pseudo_root; 
		private bool invalid;
		private bool disposed;
		public weak Node root {
			get {return _root;}
		}
		public weak Node set_root {
			construct {
				_root = value;
				_root.weak_ref(weak_ref_notify, this);
			}
		}
		public Section(DocumentModel document, GMarkupDoc.Node root) {
			this.document = document;
			this.set_root = root;
		}
		public override void dispose() {
			if(!disposed) {
				disposed = true;
				if(!invalid)
					_root.weak_unref(weak_ref_notify, this);
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
		public virtual Text CreateText(string text) {
			return document.CreateText(text);
		}
		public virtual Special CreateSpecial(string text) {
			return document.CreateSpecial(text);
		}
		public virtual Tag CreateTag(string tag) {
			return document.CreateTag(tag);
		}
		public virtual Tag CreateTagWithAttributes(string tag, 
				string[] attr_names, string[] attr_values) {
			return document.CreateTagWithAttributes(tag, attr_names, attr_values);
		}
		construct {
			disposed = false;
			invalid = false;
			document.updated += (d, n, prop) => {
				//message("%s", is_inside(n).to_string());
				if(!this.invalid && is_inside(n)) this.updated(n, prop);
			};
			document.inserted += (d, p, n, pos) => {
				if(!this.invalid && is_inside(p)) this.inserted(p, n, pos);
			};
			document.removed += (d, p, n) => {
//				message(" %s is_inside %s, = %s", p.name, this.root.name, is_inside(p).to_string());
				if(!this.invalid && is_inside(p)) this.removed(p, n);
			};
			pseudo_root = new Root(document);
		}
		private static void weak_ref_notify(void* data, GLib.Object object) {
			Section t = (Section) data;
			t.invalid = true;
			t._root = t.pseudo_root;
		}
	}

}
