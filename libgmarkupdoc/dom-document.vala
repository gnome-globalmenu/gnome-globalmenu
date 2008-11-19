using GLib;
using Gee;

namespace DOM {
	public class Document: Node {
		public Document() {
			base(null, Node.Type.DOCUMENT, "#document");
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
			return new Attr(this, name);
		}
		public Node getElementById(string id) {
			/* TODO: write getElementById. */
			/* FIXME: shouldn't return this */
			return this;
		}
/*
   Not IMPLEMENTED.
		public Node implemetation {get;}
		public Node createEntityReference(string name) { return null;}
		public Node createCDATASection(string data) { return null;}
*/

	}
}
