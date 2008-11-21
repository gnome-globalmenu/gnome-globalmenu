using GLib;
using Gee;

namespace DOM {
	/*******
	 * Differ from the specification:
	 *
	 * No TEXT node is created to parse the attr value.
	 */
	public class Attr : Node {
		public Attr (Document owner, string name, bool is_id) {
			base(owner, Node.Type.ATTRIBUTE, name);
			this.is_id = is_id;
		}
		/* Attr Interface */
		public string name {
			get {
				return nodeName;
			}
		}
		public string value {
			get {
				return nodeValue;
			} 
			construct set {
				if(is_id) {
					if(_ownerElement != null && ownerDocument != null) {
						if(nodeValue != null)
							ownerDocument.unregister_element(nodeValue, _ownerElement);
						if(value != null)
							ownerDocument.register_element(value, _ownerElement);
					}
				}
				nodeValue = value;
			}
		}
		public bool specified {
			get {
				return value == null;
			}
		}
		public weak Element ownerElement {
			get {
				return _ownerElement;
			} set {
				if(is_id) {
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
		private weak Element _ownerElement;
		public bool is_id { get; construct; }
		~Attr() {
			if(is_id) {
				if(_ownerElement != null && ownerDocument != null)
					ownerDocument.unregister_element(value, _ownerElement);
			}
			if(_ownerElement != null) {
				_ownerElement.remove_weak_pointer((void**)(&_ownerElement));
			}
		}
	}
}
