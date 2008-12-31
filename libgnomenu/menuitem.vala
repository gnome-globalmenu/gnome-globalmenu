using Gtk;

namespace Gnomenu {
	/**
	 * fancy menu item used by GlobalMenu.PanelApplet
	 *
	 * Difference with Gtk.MenuItem:
	 *
	 * The various derived XXXMenuItem types are integrated
	 * into Gnomenu.MenuItem to allow easier tranformation
	 * from XML representation to Widgets.
	 *
	 * label and image are directly set on the MenuItem.
	 * each item has an id,
	 * associated with a position(after attached to a MenuShell)
	 *
	 * MenuItem also holds a back-reference to the toplevel 
	 * menu bar. When an item is activated, the activated signal
	 * on the menu bar is also invoked.
	 * 
	 */
	public class MenuItem : Gtk.MenuItem {
		public MenuItem() { }

		static int icon_width;
		static int icon_height;
		static construct {
			install_style_property(new 
					ParamSpecInt("indicator-size", 
						_("Indicator Size"),
						_("Size of check or radio indicator"), 
						0, int.MAX, 13, ParamFlags.READABLE));
			icon_size_lookup (IconSize.MENU, out icon_width, out icon_height);
			/*Load gtk-menu-images setting*/
			Gtk.ImageMenuItem item = new Gtk.ImageMenuItem();
		}

		construct {
			disposed = false;
			_item_type = MenuItemType.NORMAL;
			create_labels();
		}

		public override void dispose() {
			if(!disposed) {
				disposed = true;
				if(_image_widget != null) {
					_image_widget.unparent();
					_image_widget = null;
				}
			}
			base.dispose();
		}
		/**
		 * a back-reference to the toplevel menubar
		 */
		public MenuBar? menubar { get; set; }

		/**
		 * the position of the menu item in the menushell,
		 * starting from 0.
		 *
		 * Notice that although this property is not readonly,
		 * it should be change only by the MenuShell 
		 * (perahsp also Parser)
		 */
		public int position {
			get { return _position;} 
			set {
				if(_position == value) return;
				_position = value;
				if(_id == null && _label == null)
				update_label_text();
			}
		}

		/**
		 * the id of the menu item.
		 *
		 * id is used to construct the path to uniquely represent
		 * the item in the menubar.
		 */
		public string? id { 
			get { return _id; }
		   	set { 
				if(_id == value) return;
				_id = value; 
				if(_label == null)
				update_label_text();
			}
		}
		/**
		 * the label text in the item.
		 *
		 */
		public string? label {
			get { return _label; }
			set {
				if(_label == value) return;
				_label = value;
				update_label_text();
			}
		}
		/**
		 * the icon in the item.
		 * Notice that the specific meaning of 
		 * the string is not yet defined.
		 *
		 * For now, it can be any of the Gtk stock item
		 * names.
		 */
		public string? icon {
			get { return _icon; }
			set {
				if(_icon == value) return;
				_icon = value;
				update_image();
			}
		}
		/**
		 * the text to describe the accelerator key combination.
		 *
		 * Notice that this is nothing more than a text.
		 * The applet doesn't handle these accelerator keys.
		 */
		public string? accel_text {
			get { return _accel_text; }
			set {
				if(_accel_text == value) return;
				_accel_text = value;
				update_label_text();
			}
		}

		/**
		 * the font description of the label.
		 * It can be any valid PangoFontDescription string.
		 *
		 * e.g "bold", "Sans Bold".
		 *
		 * Anything more than "Bold" is not recommend
		 * since it interacts badly with themes.
		 */
		public string? font {
			get { return _font; }
			set {
				if(_font == value) return;
				_font = value;
				update_font();
			}
		}

		/**
		 * Obtain the path of the this item.
		 *
		 * The path is constructed by backtracing the 
		 * menu hierarch until reaching the toplevel menu bar.
		 *
		 * Notice that the [rev:] prefix in global menu specification
		 * is not implemented (yet).
		 *
		 * Here are several examples of returned strings:
		 *
		 * /0/1/3/0
		 *
		 * /File/New/Message
		 *
		 * /0/New/Message
		 *
		 * 0/New/Message (If the toplevel menu bar is not found).
		 *
		 * The return value in the last case should 
		 * probably be replaced by null.
		 */
		public string path {
			get {
				MenuItem item = this;
				MenuShell parent = item.parent as MenuShell;
				if(id != null)
					_path = id;
				else
					_path = position.to_string();

				while(parent != null && !(parent is MenuBar)) {
					item = (parent as Menu).get_attach_widget() as MenuItem;
					if(item == null) break;
					if(item.id != null) 
						_path = item.id + "/" + _path;
					else
						_path = item.position.to_string() + "/" + _path;
					parent = item.parent as MenuShell;
					if(parent == null) break;
				}
				if(parent is MenuBar) {
					_path = "/" + _path;
				}
				return _path;
			}
		}

		/**
		 * set/get the type string of the item.
		 * Valid values can be found in
		 *
		 * { @link item_type_to_string }
		 *
		 */
		public string? item_type {
			get { return item_type_to_string(_item_type); }
			set construct {
				MenuItemType new_type = item_type_from_string(value);
				MenuItemType old_type = _item_type;
				if(new_type == _item_type) return;
				_item_type = new_type;
				switch(_item_type) {
					case MenuItemType.SEPARATOR:
						remove_child();
					break;
					case MenuItemType.NORMAL:
					case MenuItemType.IMAGE:
					case MenuItemType.CHECK:
					case MenuItemType.RADIO:
						if(old_type != MenuItemType.NORMAL
						&& old_type != MenuItemType.IMAGE
						&& old_type != MenuItemType.RADIO
						&& old_type != MenuItemType.CHECK
						) {
							remove_child();
							create_labels();
							update_label_text();
						}
					break;
					case MenuItemType.ARROW:
						if(old_type != MenuItemType.ARROW) {
							remove_child();
							create_arrow();
							update_arrow_type();
						}
					break;
					case MenuItemType.ICON:
						if(old_type != MenuItemType.ICON) {
							remove_child();
							_icon_widget = new Gtk.Image();
							_icon_widget.visible = true;
							add(_icon_widget);
							update_image();
						}
					break;
				}
				if(_item_type == MenuItemType.IMAGE) {
					_image_widget = new Gtk.Image();
					_image_widget.set_parent(this);
					_image_widget.visible = true;
					update_image();
				} else {
					if(_image_widget != null) {
						_image_widget.unparent();
						_image_widget = null;
					}
				}
				queue_resize();
			}
		}


		/**
		 * get/set the state of the item. It can be either triggled or not,
		 * or in a tristate.
		 *
		 * { @link item_state_to_string }
		 */
		public string? item_state {
			get { return item_state_to_string(_item_state); }
			set {
				MenuItemState new_state = item_state_from_string(value);
				if(new_state == _item_state) return;
				_item_state = new_state;
				queue_draw();
			}
		}
		public Gravity gravity {
			get { return _gravity; }
			set {
				if(_gravity == value) return;
				_gravity = value;
				if(item_type_has_label(_item_type))
					get_label_widget().gravity = value;
				update_arrow_type();
			}
		}
		
		private bool disposed;

		private string _path; /*merely a buffer*/
		private string _font;
		private string _label;
		private string _icon;
		private string _accel_text;
		private string _id;
		private int _position;
		private Gravity _gravity;
		private MenuItemType _item_type;
		private MenuItemState _item_state;

		private bool _show_image {
			get {
				bool rt = false;
				get_settings().get("gtk-menu-images", &rt, null);
				return rt;
			}
		}

		public Gtk.Image? image {
			get {
				if(_item_type == MenuItemType.IMAGE)
					return _image_widget;
				if(_item_type == MenuItemType.ICON)
					return _icon_widget;
				return null;
			}
		}
		private Gtk.Image _image_widget;
		private Gtk.Image _icon_widget;

		public override void toggle_size_request(void* requisition) {
			int toggle_spacing = 0;
			int indicator_size = 0;
			style_get("toggle-spacing", &toggle_spacing,
				"indicator-size", &indicator_size, null);
			switch(_item_type) {
				case MenuItemType.CHECK:
				case MenuItemType.RADIO:
					*((int*) requisition ) = indicator_size + toggle_spacing;
				break;
				case MenuItemType.IMAGE:
				/*FIXME: BTT, TTB uses icon_height*/
					if(!_show_image) {
						*((int*) requisition) = 0;
						break;
					}
					if(image != null && _icon != null) {
						Requisition req;
						image.size_request(out req);
						*((int*) requisition ) = req.width + toggle_spacing;
					} else {
						/*no image*/
						*((int*) requisition ) = 0;
					}
				break;
				default:
					*((int*) requisition ) = 0;
				break;
			}
		}

		/**
		 *
		 * The parent handler is first invoked.
		 * Then the check box or the radio box
		 * is drawn.
		 *
		 */
		public override bool expose_event(Gdk.EventExpose event) {
			base.expose_event(event);
			int toggle_spacing = 0;
			int indicator_size = 0;
			int horizontal_padding = 0;
			style_get(
				"toggle-spacing", &toggle_spacing,
				"indicator-size", &indicator_size,
 			    "horizontal-padding", &horizontal_padding,
				null);
			ShadowType shadow_type = item_state_to_shadow_type(_item_state);
			/*FIXME: alignment !*/
			int x = 0;
			int y = 0;
			int offset = (toggle_size - indicator_size)/2;
			int spacing = toggle_spacing /2;
			switch(get_direction()) {
				case Gtk.TextDirection.LTR:
					x = allocation.x + offset + spacing;
					y = allocation.y + offset;
				break;
				case Gtk.TextDirection.RTL:
					x = allocation.x + allocation.width - toggle_size - offset - spacing;
					y = allocation.y + offset;
				break;
			}
			switch(_item_type) {
				case MenuItemType.CHECK:
					Gtk.paint_check(style,
						window,
						(StateType)state,
						shadow_type,
						event.area, 
						this,
						"check",
						x,
						y,
						indicator_size,
						indicator_size);
				break;
				case MenuItemType.RADIO:
					Gtk.paint_option(style,
						window,
						(StateType)state,
						shadow_type,
						event.area, 
						this,
						"option",
						x,
						y,
						indicator_size,
						indicator_size);
				break;
			}
			return false;
		}

		/**
		 * Overriding Gtk.Container.forall.
		 *
		 * There is a noterious problem in Vala Gtk bindings, causing
		 * the ccode function differs from GtkContainerClass.forall.
		 *
		 * Therefore this function's ccode is patched with patch.sh
		 * to avoid the problems. the dummy variable include_internals
		 * should be kept here.
		 *
		 */
		public override void forall(Gtk.Callback callback, void* data) {
			/*see patch.sh! */
			bool include_internals = false;
			if(include_internals) {
				if(_item_type == MenuItemType.IMAGE)
					callback(_image_widget);
			}
			base.forall(callback, data);
		}
		public override void activate() {
			menubar.activate(this);
		}
		private static void show_image_notify_r(Gtk.Widget widget, Gtk.Settings settings) {
			if(widget is MenuItem) {
				MenuItem item = widget as MenuItem;
				if(item._image_widget != null) {
					item._image_widget.visible = item._show_image;
				}
				item.queue_resize();
			} else {
				if(widget is Container) {
					List<weak Gtk.Widget> children = gtk_container_get_children(widget as Container);
					foreach(Gtk.Widget child in children)
						show_image_notify_r(child, settings);
				}
			}
		}
		private static void show_image_notify(Gtk.Settings settings) {
			List<weak Gtk.Window> toplevels = gtk_window_list_toplevels();
			foreach(Gtk.Container c in toplevels) {
				show_image_notify_r(c, settings);
			}
		}
		public override void screen_changed(Gdk.Screen previous_screen) {
			if(!has_screen()) return;
			Gtk.Settings settings = get_settings();
			if(settings.get_data("gnomenu-menu-item-connection") == null) {
				settings.notify["gtk-menu-images"] += show_image_notify;
				/*set it to non-null value*/
				settings.set_data("gnomenu-menu-item-connection", settings);
				show_image_notify(settings);
			}
		}
		public override void size_request(out Requisition req) {
			if(_item_type == MenuItemType.IMAGE) {
				Requisition image_req;
				_image_widget.size_request(out image_req); 
				/*Then throw it away*/
			}
			if(_item_type == MenuItemType.ICON) {
				int horizontal_padding = 0;
				style_get ("horizontal-padding", 
						&horizontal_padding,
						null);
				_icon_widget.size_request(out req);
				req.width += (int)border_width * 2 + horizontal_padding * 2;
				req.height += (int)border_width * 2;
			} else {
				base.size_request(out req);	
			}
		}
		public override void size_allocate(Gdk.Rectangle a) {
			Gdk.Rectangle ca = {0, 0, 0, 0};
			if(_item_type == MenuItemType.ICON) {
				int horizontal_padding = 0;
				style_get ("horizontal-padding", 
						&horizontal_padding,
						null);
				ca.x = a.x + (int)border_width + horizontal_padding;
				ca.y = a.y + (int)border_width;
				ca.width = a.width - (int)border_width * 2 - horizontal_padding;
				ca.height = a.height - (int)border_width * 2;
				_icon_widget.size_allocate(ca);
				if((get_flags() & WidgetFlags.REALIZED) != 0) {
					event_window.move_resize(allocation.x,
							allocation.y,
							allocation.width, allocation.height);
				}
				allocation = (Allocation) a;
			} else {
				base.size_allocate(a);
			}
			if(_item_type == MenuItemType.IMAGE) {
				/*FIXME: alignment !*/
				int toggle_spacing = 0;
				Requisition icon_req;
				_image_widget.get_child_requisition(out icon_req);
				style_get(
					"toggle-spacing", &toggle_spacing,
					null);
				ca.width = icon_req.width;
				ca.height = icon_req.height;
				int xoffset = (toggle_size - icon_req.width + toggle_spacing)/2;
				int yoffset = (a.height - icon_req.height)/2;
				ca.y = a.y + yoffset;
				switch(get_direction()) {
					case Gtk.TextDirection.LTR:
						ca.x = a.x + xoffset;
					break;
					case Gtk.TextDirection.RTL:
						ca.x = a.x + a.width - ca.width - xoffset;
					break;
				}
				_image_widget.size_allocate(ca);
			}
		}
		private void update_font() {
			Pango.FontDescription desc;
			if(_font != null) 
				desc = pango_font_description_from_string(_font);
			else 
				desc = null;
			weak Widget bin_child = get_child();
			bin_child.modify_font(desc);
		}
		private void update_label_text() {
			if(!item_type_has_label(_item_type)) return;
			string text;
			text = _label;
			if(text == null)
				text = path;

			MenuLabel label = get_label_widget();;
			assert(label != null);
			label.label = text;
			label.accel = accel_text;
		}
		private void update_image() {
			if(_item_type != MenuItemType.IMAGE
			&& _item_type != MenuItemType.ICON) return;
			if(icon != null && icon.has_prefix("theme:")) {
				weak string icon_name = icon.offset(6);
				image.set_from_icon_name(icon_name, IconSize.MENU);
			} else 
			if(icon != null && icon.has_prefix("file:")) {
				weak string filename = icon.offset(5);
				image.set_from_file(filename);
			} else 
			if(icon != null && icon.has_prefix("pixbuf:")) {
				weak string b64data = icon.offset(7);
				int len = 0;
				uchar [] data = Base64.decode(b64data, out len);
				Gdk.Pixdata pixdata = {0};
				pixdata.deserialize(data);
				Gdk.Pixbuf pixbuf = gdk_pixbuf_from_pixdata(pixdata, true);
				image.set_from_pixbuf(pixbuf);
			} else {
				image.set_from_stock(icon, IconSize.MENU);
			}
		}
		public override void parent_set(Gtk.Widget old_parent) {
			update_label_text();
		}
		private void create_labels() {
			assert(item_type_has_label(_item_type));
			add(new MenuLabel());
			get_child().visible = true;
			(get_child() as MenuLabel).gravity = gravity;
			update_font();	
		}
		private weak MenuLabel? get_label_widget() {
			weak MenuLabel label = get_child() as MenuLabel;
			return label;
		}
		private void remove_child() {
			Gtk.Widget child = get_child();
			if(child != null) remove(child);
		}
		private void create_arrow() {
			assert(_item_type == MenuItemType.ARROW);
			add(new Arrow(gravity_to_arrow_type(_gravity), ShadowType.NONE));
			get_child().visible = true;
		}
		private void update_arrow_type() {
			if(_item_type != MenuItemType.ARROW) return;
			(get_child() as Arrow).set(gravity_to_arrow_type(_gravity), ShadowType.NONE);
		}
	}
}
