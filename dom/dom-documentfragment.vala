using GLib;
using Gee;

namespace DOM {
	public class DocumentFragment : Node {
		public DocumentFragment(Document owner) {
			base(owner, Node.Type.DOCUMENT_FRAGMENT, "#document_fragment");
		}	
	}
}
