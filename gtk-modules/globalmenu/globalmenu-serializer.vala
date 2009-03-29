using Gtk;
namespace GlobalMenuGTK {
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
			if(widget is Image) visit_image(widget as Image);
			else
			if(widget is Gtk.Container) visit_container(widget as Container);
		}
		private void visit_container(Container container) {
			List<weak Widget> children = container.get_children();
			debug("%p has %u children", container, children.length());
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

			sb.append(Markup.printf_escaped(" id=\"W%lu\"", (ulong)menuitem));

			label_sb.erase(0, -1);
			last_item_empty = true;
			guessed_type = null;
			visit_container(menuitem);

			if(label_sb.len > 0) {
				sb.append(Markup.printf_escaped(" label=\"%s\"", label_sb.str));
				last_item_empty = false;
			}
			if(menuitem is SeparatorMenuItem
			|| menuitem.get_child() == null) {
				guessed_type = "s";
				last_item_empty = false;
			}


			if(menuitem is ImageMenuItem) {
				Image image = (menuitem as ImageMenuItem).image as Image;
				if(image != null) {
					guessed_type = "i";
					append_icon_attribute(image);
					last_item_empty = false;
				}
			}
			if(menuitem is CheckMenuItem) {
				var checkmenuitem = menuitem as CheckMenuItem;
				if(checkmenuitem.draw_as_radio) 
					guessed_type = "r";
				else 
					guessed_type = "c";
				if(!checkmenuitem.inconsistent) {
					if(checkmenuitem.active)
						sb.append(" state=\"1\"");
					else
						sb.append(" state=\"0\"");
				}
				last_item_empty = false;
			}

			if(menuitem.visible == false) {
				sb.append(" visible=\"0\"");
				last_item_empty = false;
			}
			if(menuitem.sensitive == false) {
				sb.append(" sensitive=\"0\"");
			}

			if(last_item_empty) {
				/* hide the empty items with
				 * no label, no image, or
				 * any other visible elements.
				 *
				 * eg, PidginMenuTray
				 * */
				sb.append(" visible=\"0\"");
			}
			if(guessed_type != null)
				sb.append_printf(" type=\"%s\"", guessed_type);

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
			debug ("append text = %s", label_text);
			if(label is AccelLabel) {
				(label as AccelLabel).refetch();
				string accel_string = (label as AccelLabel).accel_string;
				accel_string.strip();
				if(accel_string.length > 0 && accel_string != "-/-" /*refer to gtkaccellabel.c:802*/ ) {
					sb.append(Markup.printf_escaped(" accel=\"%s\"", accel_string));
				}
			}
		}
		private void visit_image(Image image) {
			/*workaround a bug before Gtk 2.14*/
			if(image.parent is ImageMenuItem) return; 
			/*Disable pure image menu item since
			  the purpose of introducing it was
			  to get Pidgin's menu icon tray working
			  but it turns out not working;
			  and causing issue 243
			sb.append(" type=\"icon\"");
			*/
			/**
			 * adobe reader sucks. It doesn't use
			 * image menu item but use a hbox
			 * to contain the image
			 */
			guessed_type = "i";
			append_icon_attribute(image);
		}

		string pixbuf_encode_b64(Gdk.Pixbuf pixbuf) {
			Gdk.Pixdata pixdata = {0};
			pixdata.from_pixbuf(pixbuf, true);
			return Base64.encode(pixdata.serialize());
		}
		private void append_icon_attribute(Image image) {
			if(image.file != null) {
				sb.append(Markup.printf_escaped(" icon=\"file:%s\"", image.file));
			} else {
				if(image.storage_type == ImageType.STOCK) {
					/* STOCK is not shared between app and the applet */
					string stock = image.stock;
					if(stock.has_prefix("gtk")) {
						/*a GTK stock, shared between app and applet*/
						sb.append(Markup.printf_escaped(" icon=\"%s\"", 
							stock));
					} else {
						if(!disable_pixbuf) {
						Gdk.Pixbuf pixbuf = image.render_icon(image.stock, 
							IconSize.MENU, null);
						if(pixbuf != null)
							sb.append(Markup.printf_escaped(" icon=\"pixbuf:%s\"", 
								pixbuf_encode_b64(pixbuf)));
						}
					}
				}
				if(image.storage_type == ImageType.ICON_NAME) {
					/* THEME is shared between applet and the application*/
					sb.append(Markup.printf_escaped(" icon=\"theme:%s\"", 
							image.icon_name));
				}
				if(image.storage_type == ImageType.PIXBUF) {
					if(!disable_pixbuf) {
					if(image.pixbuf != null)
						sb.append(Markup.printf_escaped(" icon=\"pixbuf:%s\"", 
								pixbuf_encode_b64(image.pixbuf)));
					}
				}		
				if(image.storage_type == ImageType.PIXMAP) {
					ulong pixmap_xid = 0;
					ulong mask_xid = 0;
					if(image.pixmap != null) 
						pixmap_xid = gdk_drawable_xid(image.pixmap);
					if(image.mask != null) 
						mask_xid = gdk_drawable_xid(image.mask);
					sb.append(Markup.printf_escaped(" icon=\"pixmap:%lu,%lu\"", 
						pixmap_xid, mask_xid));
				}
			}
		}
		private MenuBar menubar;
		private StringBuilder sb;
		private StringBuilder label_sb;
		private bool last_item_empty;
		private weak string guessed_type;
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
