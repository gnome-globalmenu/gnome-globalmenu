using GLib;
using Gee;

namespace DOM {
	public class ToString : Visitor {
		StringBuilder _sb;
		int level;
		public ToString() {
			_sb = new StringBuilder("");
			level = 0;
		}
		public string result {
			get {
				return _sb.str;
			}
		}
		public override void visitAttribute (Attr attr, Stage stage) {
			switch(stage) {
				case Stage.START:
					_sb.append_printf(" %s=\"", attr.name);
				break;
				case Stage.END:
					_sb.append_printf("%s\"", attr.value);
				break;
			}
		}
		public override void visitTextNode (Text text, Stage stage) {
			switch(stage) {
				case Stage.START:
					_sb.append_printf("%s", text.data);
				break;
				case Stage.END:
				break;
			}
		}
		public override void visitElement (Element element, Stage stage) {
			switch(stage) {
				case Stage.START:
					_sb.append_printf("<%s", element.nodeName);
				break;
				case Stage.ATTRIBUTES_END:
					if(element.hasChildNodes()) {
						_sb.append(">");
					} else {
						_sb.append("/>");
					}
				break;
				case Stage.END:
					if(element.hasChildNodes()) {
						_sb.append_printf("</%s>", element.nodeName);
					}
				break;
			}
		}
		public override void visitComment (Comment comment, Stage stage) {
			switch(stage) {
				case Stage.START:
					_sb.append_printf("%s", comment.data);
				break;
				case Stage.END:
				break;
			}
		}
		public override void visitDocument (Document document, Stage stage) {
			
		}
	}
}
