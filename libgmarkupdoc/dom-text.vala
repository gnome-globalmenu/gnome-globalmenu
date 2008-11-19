using GLib;
using Gee;

namespace DOM {
	public class Text : CharacterData {
		public Text(Document owner, string data) {
			(base as Node)(owner, Node.Type.TEXT, "#text");
			this.data = data;
		}
		/* Text Interface */
		public Text splitText(long offset) throws Exception {
			if(offset >= data.length || offset <= 0) throw new Exception.INDEX_SIZE_ERR("splitting has to be in the center of a string");

			string splited_data = data.substring(offset, data.length - offset);
			data = data.substring(0, offset);
			Text splitted = ownerDocument.createTextNode(splited_data);
			if(this.parentNode != null)
				return this.parentNode.insertBefore(splitted, this.nextSibling) as Text;
			else return splitted;
		}
	}
}
