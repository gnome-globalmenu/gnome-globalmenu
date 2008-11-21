using GLib;
using Gee;

namespace DOM {
	public class Element : Node {
		public Element(Document owner, string tagName) {
			base(owner, Node.Type.ELEMENT, tagName);
		}
		/* Element Interface */
		public string tagName {get {return nodeName;} }

		public Gee.List<Node> getElementsByTagName(string tagname) {
			Gee.ArrayList<Node> rt = new Gee.ArrayList<Node>();
			getElementsByTagName_r(tagname, rt);
			return rt;
		}

		public weak string? getAttribute(string name) {
			Attr attr = getAttributeNode(name);
			return attr.value;
		}

		public void setAttribute(string name, string value) {
			Attr attr = getAttributeNode(name);
			attr.value = value;
		}

		public void removeAttribute(string name) {
			attributes.remove(name);
		}

		/**
		 * FIXME: a new attr is always created even if it doesn't exist
		 * */
		public Attr getAttributeNode(string name) {
			Attr attr = attributes.get(name);
			if(attr == null) {
				attr = ownerDocument.createAttribute(name);
				attributes.set(attr.name, attr);
				attr.ownerElement = this;
			}
			return attr;
		}

		public Attr setAttributeNode(Attr newAttr) throws Exception {
			if(newAttr.ownerDocument != this.ownerDocument) throw new Exception.WRONG_DOCUMENT_ERR("");
			if(newAttr.ownerElement != null) throw new Exception.INUSE_ATTRIBUTE_ERR("");
			attributes.set(newAttr.name, newAttr);
			newAttr.ownerElement = this;
			return attributes.get(newAttr.name);
		}

		public Attr removeAttributeNode(Attr oldAttr) throws Exception {
			Attr oldAttr_ref = oldAttr;
			if(!attributes.contains(oldAttr.name) ||
				attributes.get(oldAttr.name) != oldAttr)
					throw new Exception.NOT_FOUND_ERR("");
			attributes.remove(oldAttr.name);
			return oldAttr_ref;
		}

		/*******
		 * Differ from specification
		 * default value is not handled
		 */
		public bool hasAttribute(string name) {
			return attributes.contains(name);
		}

		private void getElementsByTagName_r(string tagname, Gee.List<Node> list) {
			if(this.tagName == tagname) list.add(this);
			foreach(Node child in childNodes) {
				if(child is Element)
					(child as Element).getElementsByTagName_r(tagname, list);
			}
		}
	}
}
