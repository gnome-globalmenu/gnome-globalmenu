using Gtk;
namespace GnomenuGtk {
	protected class Serializer {
		Serializer () {
		
		}
		public static string to_string(MenuBar menubar, bool pretty_print = false) {
			Serializer s = new Serializer();
			s.menubar = menubar;
			s.pretty_print = pretty_print;
			s.sb = new StringBuilder("");
			s.visit(menubar);
			return s.sb.str;
		}
		private void visit(Widget widget) {
			if(widget is MenuBar) visit_menubar(widget as MenuBar);
			else
			if(widget is Menu) visit_menu(widget as Menu);
			else
			if(widget is MenuItem) visit_menuitem(widget as MenuItem);
			else
			if(widget is Label) visit_label(widget as Label);
			else
			if(widget is Gtk.Container) visit_container(widget as Container);
		}
		private void visit_container(Container container) {
			weak List<weak Widget> children = container.get_children();
			foreach(weak Widget child in children) {
				visit(child);
			}
		}
		private void visit_menubar(MenuBar menubar) {
			indent();
			sb.append("<menu>");
			linebreak();
			level++;
			visit_container(menubar);
			level--;
			indent();
			sb.append("</menu>");
			linebreak();
		}
		private void visit_menu(Menu menu) {
			indent();
			sb.append("<menu>");
			linebreak();
			visit_container(menu);
			indent();
			sb.append("</menu>");
			linebreak();
		}
		private void visit_menuitem(MenuItem menuitem) {
			if(menuitem is TearoffMenuItem) return;
			indent();
			sb.append("<item");
			visit_container(menuitem);
			
			if(menuitem is SeparatorMenuItem) 
				sb.append(" type=\"s\"");

			if(menuitem is ImageMenuItem) {
				Image image = (menuitem as ImageMenuItem).image as Image;
				if(image != null) {
					if(image.storage_type == ImageType.STOCK) {
						/*FIXME: only stock icons are supported! Do more!*/
						sb.append(" type=\"i\"");
						sb.append(Markup.printf_escaped(" icon=\"%s\"", 
								image.stock));
					}
				}
			}
			if(menuitem is CheckMenuItem) {
				var checkmenuitem = menuitem as CheckMenuItem;
				if(checkmenuitem.draw_as_radio) 
					sb.append(" type=\"r\"");
				else 
					sb.append(" type=\"c\"");

				if(!checkmenuitem.inconsistent) {
					if(checkmenuitem.active)
						sb.append(" state=\"1\"");
					else
						sb.append(" state=\"0\"");
				}
			}

			if(menuitem.visible == false) {
				sb.append(" visible=\"0\"");
			}
			if(menuitem.sensitive == false) {
				sb.append(" sensitive=\"0\"");
			}

			if(menuitem.submenu == null) {
				sb.append("/>");
				linebreak();
			} else {
				sb.append_c('>');
				linebreak();
				level++;
				if(menuitem.submenu != null) {
					visit_menu(menuitem.submenu);
				}
				level--;
				indent();
				sb.append("</item>");
				linebreak();
			}
		}
		private void visit_label(Label label) {
			sb.append(Markup.printf_escaped(" label=\"%s\"", label.label));
			if(label is AccelLabel) {
				(label as AccelLabel).refetch();
				string accel_string = (label as AccelLabel).accel_string;
				accel_string.strip();
				if(accel_string.length > 0 && accel_string != "-/-" /*refer to gtkaccellabel.c:802*/ ) {
					sb.append(Markup.printf_escaped(" accel=\"%s\"", accel_string));
				}
			}
		}

		private MenuBar menubar;
		private StringBuilder sb;
		private bool pretty_print;
		private int level;
		private bool newline;
		private void indent() {
			if(!pretty_print) return;
			if(!newline) return;
			for(int i = 0; i < level; i++) {
				sb.append_c(' ');
			}
			newline = false;
		}
		private void linebreak() {
			if(!pretty_print) return;
			sb.append_c('\n');
			newline = true;
		}
	}

}
