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
			foreach(Node child in childNodes) {
				if(child is Element && (child as Element).tagName == tagname)
					rt.add(child);
			}
			return rt;
		}

		/******
		 * Default value is not handled
		 *****/
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

		public Attr getAttributeNode(string name) {
			Attr attr = attributes.get(name);
			if(attr == null) {
				attr = ownerDocument.createAttribute(name);
				try {
					attr = setAttributeNode(attr);
				} catch (GLib.Error e) {
					critical("%s", e.message);
				}
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

		/******
		 * Default value is not immediately used to replace the removed attribute.
		 */
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
	}
}
