using Gtk;
namespace GnomenuGtk {
	[Compact]
	protected class Serializer {
		Serializer () {
		
		}
		public static string to_string(MenuBar menubar, bool pretty_print = false) {
			Serializer s = new Serializer();
			Timer timer = new Timer();
			s.menubar = menubar;
			s.pretty_print = pretty_print;
			s.sb = new StringBuilder("");
			s.label_sb = new StringBuilder("");
			s.visit(menubar);
			debug("Serializer consumption = %lf", timer.elapsed(null));
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
			List<weak Widget> children = gtk_container_get_children(container);
			debug("%s %p has %u children", container.get_name(), container, children.length());
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
			label_sb.erase(0, -1);
			visit_container(menuitem);
			if(label_sb.len > 0) {
				sb.append(Markup.printf_escaped(" label=\"%s\"", label_sb.str));
			}
			if(menuitem is SeparatorMenuItem
			|| menuitem.get_child() == null) 
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
					if(image.storage_type == ImageType.PIXBUF) {
						Gdk.Pixdata pixdata = {0};
						pixdata.from_pixbuf(image.pixbuf, true);
						string b64_data = Base64.encode(pixdata.serialize());
						sb.append(" type=\"i\"");
						sb.append(Markup.printf_escaped(" icon=\"pixbuf:%s\"", b64_data));
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
			string label_text = label.label;
			label_sb.append(label_text);
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
		private StringBuilder label_sb;
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
