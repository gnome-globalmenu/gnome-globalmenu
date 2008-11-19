using GLib;
using Gee;

namespace DOM {
	public class Document: Node {
		public Document() {
			base(null, Node.Type.DOCUMENT, "#document");
			_id_map = new Gee.HashMap<weak string, weak Element>(str_hash, str_equal, direct_equal);
			_childNodes = new Gee.ArrayList<weak Node>();
		}
		/* Document Interface */
		public DocumentType documentType;
		public Element documentElement;
		public override Gee.List<weak Node> childNodes {
			get {
				return _childNodes;
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
		private int object_built = 0;

		private Gee.List<weak Node> _childNodes;

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
