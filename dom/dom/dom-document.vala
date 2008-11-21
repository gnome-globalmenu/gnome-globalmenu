using GLib;
using Gee;

namespace DOM {
	public class Document: Node {
		public Document() {
			Document.with_type_name("#");
		}
		public Document.with_type_name(string typename) {
			base(null, Node.Type.DOCUMENT, "#document");
			_id_map = new Gee.HashMap<weak string, weak Element>(str_hash, str_equal, direct_equal);
			documentType = new DocumentType(typename);
		}
		/* Document Interface */
		public DocumentType documentType {get; construct;}
		public Element? documentElement {
			get {
				foreach(Node child in childNodes) {
					if(child.nodeType == Node.Type.ELEMENT) {
						return child as Element;
					}
				}
				return null;
			}
		}

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
			return new Attr(this, name);
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
	}
}
