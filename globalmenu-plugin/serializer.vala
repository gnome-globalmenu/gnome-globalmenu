public class Serializer {
	public Serializer () { }
	public bool disable_pixbuf = false;
	public bool pretty_print = false;

	private bool hybrid = false;
	public string to_string(Gtk.MenuBar menubar) {
		Timer timer = new Timer();
		this.menubar = menubar;
		sb = new StringBuilder("");
		label_sb = new StringBuilder("");
		visit(menubar);
		debug("Serializer consumption = %lf", timer.elapsed(null));
		return sb.str;
	}
	private void visit(Gtk.Widget widget) {
		if(widget is Gtk.MenuBar) visit_menubar(widget as Gtk.MenuBar);
		else
		if(widget is Gtk.Menu) visit_menu(widget as Gtk.Menu);
		else
		if(widget is Gtk.MenuItem) visit_menuitem(widget as Gtk.MenuItem);
		else
		if(widget is Gtk.Label) visit_label(widget as Gtk.Label);
		else
		if(widget is Gtk.Image) visit_image(widget as Gtk.Image);
		else
		if(widget is Gtk.Container) visit_container(widget as Gtk.Container);
	}
	private void visit_container(Gtk.Container container) {
		List<weak Gtk.Widget> children = container.get_children();
		debug("%p has %u children", container, children.length());
		foreach(var child in children) {
			visit(child);
		}
	}
	private void visit_menubar(Gtk.MenuBar menubar) {
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
	private void visit_menu(Gtk.Menu menu) {
		indent();
		sb.append("<menu>");
		linebreak();
		visit_container(menu);
		indent();
		sb.append("</menu>");
		linebreak();
	}
	private void visit_menuitem(Gtk.MenuItem menuitem) {
		if(menuitem is Gtk.TearoffMenuItem) return;
		indent();
		sb.append("<item");

		sb.append(Markup.printf_escaped(" id=\"W%lu\"", (ulong)menuitem));

		if(hybrid) sb.append(" client-side=\"1\"");

		label_sb.erase(0, -1);
		last_item_empty = true;
		guessed_type = null;
		visit_container(menuitem);

		if(label_sb.len > 0) {
			sb.append(Markup.printf_escaped(" label=\"%s\"", label_sb.str));
			last_item_empty = false;
		}
		if(menuitem is Gtk.SeparatorMenuItem
		|| menuitem.get_child() == null) {
			guessed_type = "s";
			last_item_empty = false;
		}


		if(menuitem is Gtk.ImageMenuItem) {
			Gtk.Image image = (menuitem as Gtk.ImageMenuItem).image as Gtk.Image;
			if(image != null) {
				guessed_type = "i";
				append_icon_attribute(image);
				last_item_empty = false;
			}
		}
		if(menuitem is Gtk.CheckMenuItem) {
			var checkmenuitem = menuitem as Gtk.CheckMenuItem;
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
			 * eg, PidginGtk.MenuTray
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
	private void visit_label(Gtk.Label label) {
		string label_text = label.label;
		label_sb.append(label_text);
		debug ("append text = %s", label_text);
		if(label is Gtk.AccelLabel) {
			(label as Gtk.AccelLabel).refetch();
			string accel_string = (label as Gtk.AccelLabel).accel_string.strip();
			if(accel_string.length > 0 && accel_string != "-/-" /*refer to gtkaccellabel.c:802*/ ) {
				sb.append(Markup.printf_escaped(" accel=\"%s\"", accel_string));
			}
		}
	}
	private void visit_image(Gtk.Image image) {
		/*workaround a bug before Gtk 2.14*/
		if(image.parent is Gtk.ImageMenuItem) return; 
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
		void * pixel_data = pixdata.from_pixbuf(pixbuf, true);
		string rt = Base64.encode(pixdata.serialize());
		/* pixdata.pixel_data is set to pixel_data
		 * by Gdk, but pixdata.pixel_data is unowned,
		 * so we have to free it */
		delete pixel_data;
		return rt;
	}
	private void append_icon_attribute(Gtk.Image image) {
		if(image.file != null) {
			sb.append(Markup.printf_escaped(" icon=\"file:%s\"", image.file));
		} else {
			if(image.storage_type == Gtk.ImageType.STOCK) {
				/* STOCK is not shared between app and the applet */
				string stock = image.stock;
				if(stock.has_prefix("gtk")) {
					/*a GTK stock, shared between app and applet*/
					sb.append(Markup.printf_escaped(" icon=\"%s\"", 
						stock));
				} else {
					if(!disable_pixbuf) {
					Gdk.Pixbuf pixbuf = image.render_icon(image.stock, 
						Gtk.IconSize.MENU, null);
					if(pixbuf != null)
						sb.append(Markup.printf_escaped(" icon=\"pixbuf:%s\"", 
							pixbuf_encode_b64(pixbuf)));
					}
				}
			}
			if(image.storage_type == Gtk.ImageType.ICON_NAME) {
				/* THEME is shared between applet and the application*/
				sb.append(Markup.printf_escaped(" icon=\"theme:%s\"", 
						image.icon_name));
			}
			if(image.storage_type == Gtk.ImageType.PIXBUF) {
				if(!disable_pixbuf) {
				if(image.pixbuf != null)
					sb.append(Markup.printf_escaped(" icon=\"pixbuf:%s\"", 
							pixbuf_encode_b64(image.pixbuf)));
				}
			}		
			if(image.storage_type == Gtk.ImageType.PIXMAP) {
				ulong pixmap_xid = 0;
				ulong mask_xid = 0;
				if(image.pixmap != null) 
					pixmap_xid = Gdk.x11_drawable_get_xid(image.pixmap);
				if(image.mask != null) 
					mask_xid = Gdk.x11_drawable_get_xid(image.mask);
				sb.append(Markup.printf_escaped(" icon=\"pixmap:%lu,%lu\"", 
					pixmap_xid, mask_xid));
			}
		}
	}

	private Gtk.MenuBar menubar;
	private StringBuilder sb;
	private StringBuilder label_sb;
	private bool last_item_empty;
	private weak string guessed_type;
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

