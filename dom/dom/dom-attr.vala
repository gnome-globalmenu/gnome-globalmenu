using GLib;
using Gee;

namespace DOM {
	/*******
	 * Differ from the specification:
	 *
	 * No TEXT node is created to parse the attr value.
	 */
	public class Attr : Node {
		public Attr (Document owner, string name) {
			base(owner, Node.Type.ATTRIBUTE, name);
			if(ownerDocument != null && ownerDocument.documentType != null) {
				this.isId = ownerDocument.documentType.isId(name);
			} else
				this.isId = false;

			this.specified = false;
		}
		/* Attr Interface */
		public bool isId { get; construct; }
		public string name {
			get {
				return nodeName;
			}
		}
		public override string? nodeValue {
			get {
				if(!specified) {
					if(ownerElement != null && ownerDocument != null && ownerDocument.documentType != null)
						return ownerDocument.documentType.default_attribute_value(ownerElement.tagName, name);
					else
						return "";
				} else
					return _nodeValue;
			} 
			construct set {
				_specified = (value != null);
				if(isId) {
					if(_ownerElement != null && ownerDocument != null) {
						if(nodeValue != null)
							ownerDocument.unregister_element(nodeValue, _ownerElement);
						if(value != null)
							ownerDocument.register_element(value, _ownerElement);
					}
				}
				_nodeValue = value;
			}
		}
		public string value {
			get {
				return nodeValue;
			} 
			set {
				nodeValue = value;
			}
		}
		public bool specified { get; construct; }
		public weak Element ownerElement {
			get {
				return _ownerElement;
			} set {
				if(isId) {
					if(nodeValue != null && ownerDocument != null) {
						if(_ownerElement != null)
							ownerDocument.unregister_element(nodeValue, _ownerElement);
						if(value != null)
							ownerDocument.register_element(nodeValue, value);
					}
				}
				if(_ownerElement != null) 
					_ownerElement.remove_weak_pointer((void**)(&_ownerElement));
				_ownerElement = value;
				if(_ownerElement != null)
					_ownerElement.add_weak_pointer((void**)(&_ownerElement));
			}
		}
		/* private */
		private string _nodeValue;
		private weak Element _ownerElement;
		~Attr() {
			if(isId) {
				if(_ownerElement != null && ownerDocument != null)
					ownerDocument.unregister_element(value, _ownerElement);
			}
			if(_ownerElement != null) {
				_ownerElement.remove_weak_pointer((void**)(&_ownerElement));
			}
		}
	}
}
