using GLib;
using Gee;

namespace DOM {

	public class Comment : CharacterData {
		public Comment(Document owner, string data) {
			(base as Node)(owner, Node.Type.COMMENT, "#comment");
			this.data = data;
		}	
	}
}
