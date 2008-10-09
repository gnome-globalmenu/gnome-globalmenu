using GLib;
using GLibCompat;
namespace GMarkupDoc {
	public class Section: GLib.Object, DocumentModel {
		private DocumentModel _document;
		public weak DocumentModel document  {
			get { return _document;} 
			construct {_document = value;}
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
				(_root as GLibCompat.Object).add_toggle_ref(toggle_ref_notify, this);
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
					(_root as GLibCompat.Object).remove_toggle_ref(toggle_ref_notify, this);
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
				if(is_inside(n)) this.updated(n, prop);
			};
			document.inserted += (d, p, n, pos) => {
				if(is_inside(p)) this.inserted(p, n, pos);
			};
			document.removed += (d, p, n) => {
				if(is_inside(p)) this.removed(p, n);
			};
			pseudo_root = new Root(document);
		}
		private static void toggle_ref_notify(void* data, GLib.Object object, bool is_last) {
			if(!is_last) return;
			Section t = (Section) data;
			(object as GLibCompat.Object).remove_toggle_ref(toggle_ref_notify, t);
			t.invalid = true;
			t._root = t.pseudo_root;
		}
	}

}
