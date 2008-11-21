using GLib;
using Gee;

namespace DOM {
	public class Serializer : Visitor {
		StringBuilder _sb;
		/*FIXME: DOM.Configuration */
		public bool format_pretty_print {get; set;}
		public bool discard_default_content {get; set;}

		public string newline {get; set;}
		public Serializer() {
			newline = "\n";
		}
		public bool write(Node nodeArg, Object /*OutputStream*/ destination) {
			assert_not_reached();
		}
		public bool writeToURL(Node nodeArg, string uri) {
			assert_not_reached();
		}
		public string writeToString(Node nodeArg) {
			_sb = new StringBuilder("");
			_need_indent = false;
			_level = 0;
			visit(nodeArg);
			return _sb.str;
		}

		private int _level;
		private bool _need_indent;
		private override void visitAttribute (Attr attr, Stage stage) {
			if(discard_default_content && !attr.specified) return;
			switch(stage) {
				case Stage.START:
					_sb.append_printf(" %s=\"", attr.name);
				break;
				case Stage.END:
					_sb.append_printf("%s\"", attr.value);
				break;
			}
		}
		private override void visitTextNode (Text text, Stage stage) {
			switch(stage) {
				case Stage.START:
					indent();
					_sb.append_printf("%s", text.data);
					break_line();
				break;
				case Stage.END:
				break;
			}
		}
		private override void visitElement (Element element, Stage stage) {
			switch(stage) {
				case Stage.START:
					indent();
					_sb.append_printf("<%s", element.nodeName);
					_level++;
				break;
				case Stage.ATTRIBUTES_END:
					if(element.hasChildNodes()) {
						_sb.append(">");
					} else {
						_sb.append("/>");
					}
					break_line();
				break;
				case Stage.END:
					_level--;
					if(element.hasChildNodes()) {
						indent();
						_sb.append_printf("</%s>", element.nodeName);
						break_line();
					}
				break;
			}
		}
		private override void visitComment (Comment comment, Stage stage) {
			switch(stage) {
				case Stage.START:
					indent();
					_sb.append_printf("%s", comment.data);
					break_line();
				break;
				case Stage.END:
				break;
			}
		}
		private override void visitDocument (Document document, Stage stage) {
			
		}
		private void indent() {
			if(!format_pretty_print) return;
			if(!_need_indent) return;
			for(int i = 0; i < _level; i++) {
				_sb.append_c(' ');
			}
			_need_indent = false;
		}
		private void break_line() {
			if(!format_pretty_print) return;
			_sb.append(_newline);
			_need_indent = true;
		}
	}
}
