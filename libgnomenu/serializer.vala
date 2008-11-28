using Gtk;

namespace Gnomenu {
	public class Serializer {
		Serializer () {
			this.sb = new StringBuilder("");
		}	
		public bool pretty_print;
		public static string to_string(MenuShell shell) {
			var s = new Serializer();
			s.visit(shell);
			return s.sb.str;
		}

		private void visit(GLib.Object node) {
			if(node is MenuShell) {
				visit_shell(node as MenuShell);
			}
			if(node is MenuItem) {
				visit_item(node as MenuItem);
			}
		}
		private void visit_shell(MenuShell shell) {
			int i;
			if(shell.length > 0) {
				indent();
				sb.append_printf("<menu>");
				newline();
				level++;
				for(i = 0; i< shell.length; i++) {
					visit(shell.get(i));
				}
				level--;
				indent();
				sb.append_printf("</menu>");
				newline();
			} else {
				indent();
				sb.append_printf("<menu/>");
				newline();
			}
		}
		private void visit_item(MenuItem item) {
			if(item.submenu != null) {
				indent();
				sb.append_printf("<item>");
				newline();
				level++;
				visit_shell(item.submenu);
				level--;
				indent();
				sb.append_printf("</item>");
				newline();
			} else {
				indent();
				sb.append_printf("<item/>");
				newline();
			}
		}
		private StringBuilder sb;
		private int level;
		private bool _newline;
		private void indent() {
			if(!pretty_print) return;
			if(_newline) {
				for(int i = 0; i < level; i++)
					sb.append_c(' ');
				_newline = false;
			}
		}
		private void newline() {
			if(!pretty_print) return;
			sb.append_c('\n');	
			_newline = true;
		}
	}

}
