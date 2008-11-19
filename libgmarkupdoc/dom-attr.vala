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
				nodeValue = value;
			}
		}
		public bool specified {
			get {
				return value == null;
			}
		}
		public Element ownerElement {get; set;}
	}
}
