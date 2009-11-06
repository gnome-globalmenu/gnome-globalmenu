/**
 * convert the widget representation to xml representation.
 */
public class Gnomenu.Serializer {
	public static string to_string(GLib.Object obj, bool pretty_print = false) {
		var s = new Serializer();
		s.pretty_print = pretty_print;
		s.visit(obj);
		return s.sb.str;
	}

	Serializer () {
		this.sb = new StringBuilder("");
	}	

	private void visit(GLib.Object node) {
		if(node is Shell) {
			visit_shell(node as Shell);
		}
		if(node is Item) {
			visit_item(node as Item);
		}
	}
	private void visit_shell(Shell shell) {
		int i;
		if(shell.length > 0) {
			indent();
			sb.append_printf("<menu>");
			newline();
			level++;
			for(i = 0; i< shell.length; i++) {
				visit(shell.get_item(i));
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
	private void visit_item(Item item) {
		if(item.has_sub_shell) {
			indent();
			sb.append_printf("<item");
			visit_item_attributes(item);
			sb.append_c('>');
			newline();
			level++;
			visit_shell(item.sub_shell);
			level--;
			indent();
			sb.append_printf("</item>");
			newline();
		} else {
			indent();
			sb.append_printf("<item");
			visit_item_attributes(item);
			sb.append("/>");
			newline();
		}
	}
	private void visit_item_attributes(Item item) {
		if(item.item_label != null) {
			sb.append(Markup.printf_escaped(" label=\"%s\"", item.item_label));
		}
		if(item.item_type != ItemType.NORMAL) {
			sb.append(Markup.printf_escaped(" type=\"%s\"", 
						Item.type_to_string(item.item_type)));
		}
		if(item.item_state != ItemState.TRISTATE) {
			sb.append(Markup.printf_escaped(" state=\"%s\"", 
						Item.state_to_string(item.item_state)));
		}
		if(item.item_font != null) {
			sb.append(Markup.printf_escaped(" font=\"%s\"", item.item_font));
		}
		if(item.item_id != null) {
			sb.append(Markup.printf_escaped(" id=\"%s\"", item.item_id));
		}
		if(item.client_side_sub_shell) {
			sb.append(Markup.printf_escaped(" client-side=\"1\""));
		}
		if(item.item_accel_text != null) {
			sb.append(Markup.printf_escaped(" accel=\"%s\"", item.item_accel_text));
		}
		if(item.item_visible == false) {
			sb.append(" visible=\"false\"");
		}
		if(item.item_sensitive == false) {
			sb.append(" sensitive=\"false\"");
		}
		if(item.item_use_underline == false) {
			sb.append(" underline=\"false\"");
		}
		if(item.item_icon != null) {
			sb.append(Markup.printf_escaped(" icon=\"%s\"", item.item_icon));
		}
	}

	private StringBuilder sb;
	private int level;
	private bool _newline;
	private bool pretty_print;

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

