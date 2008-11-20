using GLib;
using Gee;

namespace DOM {
	public class Document: Node {
		public Document() {
			base(null, Node.Type.DOCUMENT, "#document");
			_id_map = new Gee.HashMap<weak string, weak Element>(str_hash, str_equal, direct_equal);
			_weak_pointers = new Gee.HashSet<void**>();
		}
		~Document() {
			foreach(void** pointer in _weak_pointers) {
				*pointer = null;
			}
		}
		/* Document Interface */
		public DocumentType documentType;
		public Element documentElement;

		public Element createElement(string tagName) {
			return new Element(this, tagName);
		}
		public DocumentFragment createDocumentFragment() {
			return new DocumentFragment(this);
		}
		public Text createTextNode(string data) {
			return new Text(this, data);
		}
		public Comment createComment(string data) {
			return new Comment(this, data);
		}
		public Attr createAttribute(string name) {
			return new Attr(this, name, name == "id");
		}
		public Element? getElementById(string id) {
			return _id_map.get(id);
		}
/*
   Not IMPLEMENTED.
		public Node implemetation {get;}
		public Node createEntityReference(string name) { return null;}
		public Node createCDATASection(string data) { return null;}
*/
		/* private */
		private Gee.Map<weak string, weak Element> _id_map;			
		private Gee.Set<void**> _weak_pointers;

		public void register_element (string id, Element? element) {
			if(element == null) {
				if(!_id_map.contains(id)) return;
				_id_map.remove(id);
			}
			if(_id_map.contains(id)) return;
			_id_map.set(id, element);
		}
		public void unregister_element (string id, Element element) {
			if(_id_map.contains(id) &&
				_id_map.get(id) == element);
			_id_map.remove(id);
		}
		public void add_weak_pointer ( void** pointer ) {
			_weak_pointers.add(pointer);
		}
		public void remove_weak_pointer ( void** pointer ) {
			_weak_pointers.remove(pointer);
		}
	}
}
