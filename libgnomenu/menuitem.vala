using Gtk;

namespace Gnomenu {
	public class MenuItem : Gtk.MenuItem {
		public MenuItem() { }
		static int icon_width;
		static int icon_height;
		static construct {
			install_style_property(new 
					ParamSpecInt("indicator-size", 
						"Indicator Size",
						"Size of check or radio indicator", 
						0, int.MAX, 13, ParamFlags.READABLE));
			icon_size_lookup (IconSize.MENU, out icon_width, out icon_height);
		}
		construct {
			_item_type = MenuItemType.NORMAL;
			create_labels();
		}
		public MenuBar? menubar { get; set; }
		public int position {
			get { return _position;} 
			set {
				if(_position == value) return;
				_position = value;
				if(_id == null && _label == null)
				update_label_text();
			}
		}
		public string? id { 
			get { return _id; }
		   	set { 
				if(_id == value) return;
				_id = value; 
				if(_label == null)
				update_label_text();
			}
		}
		public string? label {
			get { return _label; }
			set {
				if(_label == value) return;
				_label = value;
				update_label_text();
			}
		}
		public string? icon {
			get { return _icon; }
			set {
				if(_icon == value) return;
				_icon = value;
				update_icon();
			}
		}
		public string? accel_text {
			get { return _accel_text; }
			set {
				if(_accel_text == value) return;
				_accel_text = value;
				update_label_text();
			}
		}
		public string? font {
			get { return _font; }
			set {
				if(_font == value) return;
				_font = value;
				weak Pango.FontDescription desc;
				if(_font != null) 
					desc = Pango.FontDescription.from_string(_font);
				else 
					desc = null;
				weak Widget bin_child = get_child();
				bin_child.modify_font(desc);
			}
		}
		public string path {
			get {
				/* return: beginning with "/" if a menu bar is found. Not if not found.*/
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
							update_label_gravity();
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
				}
				if(_item_type == MenuItemType.IMAGE) {
					icon_widget = new Gtk.Image();
					icon_widget.set_parent(this);
					icon_widget.visible = true;
					update_icon();
				} else {
					if(icon_widget != null) {
						icon_widget.unparent();
						icon_widget = null;
					}
				}
				queue_resize();
			}
		}
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
				update_label_gravity();
				update_arrow_type();
			}
		}
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
		private Gtk.Image icon_widget;

		private override void toggle_size_request(void* requisition) {
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
					*((int*) requisition ) = icon_width + toggle_spacing;
				break;
				default:
					*((int*) requisition ) = 0;
				break;
			}
		}
		private override bool expose_event(Gdk.EventExpose event) {
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
			switch(_item_type) {
				case MenuItemType.CHECK:
					Gtk.paint_check(style,
						window,
						(StateType)state,
						shadow_type,
						event.area, 
						this,
						"check",
						allocation.x,
						allocation.y,
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
						allocation.x,
						allocation.y,
						indicator_size,
						indicator_size);
				break;
			}
			return false;
		}
		private override void forall(Gtk.Callback callback, void* data) {
			/*see patch.sh! */
			bool include_internals = false;
			if(include_internals) {
				if(_item_type == MenuItemType.IMAGE)
					callback(icon_widget);
			}
			base.forall(callback, data);
		}
		private override void activate() {
			menubar.activate(this);
		}
		private override void size_request(out Requisition req) {
			if(_item_type == MenuItemType.IMAGE) {
				Requisition icon_req;
				icon_widget.size_request(out icon_req); /*Then throw it away*/
			}
			base.size_request(out req);	
		}
		private override void size_allocate(Gdk.Rectangle a) {
			Gdk.Rectangle ca;
			base.size_allocate(a);
			if(_item_type == MenuItemType.IMAGE) {
				/*FIXME: alignment !*/
				ca.x = a.x;
				ca.y = a.y;
				ca.width = icon_width;
				ca.height = icon_height;
				icon_widget.size_allocate(ca);
			}
		}
		private void update_label_gravity() {
			if(!item_type_has_label(_item_type)) return;
			double text_angle = gravity_to_text_angle(gravity);
			Label label = get_label_widget();
			assert(label != null);
			switch(gravity) {
				case Gravity.DOWN:
				case Gravity.UP:
					label.set_alignment( (float)0.0, (float)0.5);
				break;
				case Gravity.LEFT:
				case Gravity.RIGHT:
					label.set_alignment( (float)0.5, (float)0.0);
				break;
			}
			label.angle = text_angle;
		}
		private void update_label_text() {
			if(!item_type_has_label(_item_type)) return;
			string text;
			text = _label;
			if(text == null)
				text = path;

			Label label = get_label_widget();;
			assert(label != null);
			if(accel_text != null) {
				text = text + " - " + accel_text;
			}
			label.label = text;
		}
		private void update_icon() {
			if(_item_type != MenuItemType.IMAGE) return;
			icon_widget.set_from_stock(icon, IconSize.MENU);
		}
		private override void parent_set(Gtk.Widget old_parent) {
			update_label_text();
		}
		private void create_labels() {
			assert(item_type_has_label(_item_type));
			add(new Label("N/A"));
			get_child().visible = true;
			(get_child() as Label).use_underline = true;
			/*FIXME: perhaps calling update_label_gravity is better?*/
			(get_child() as Label).set_alignment( (float)0.0, (float)0.5);
		}
		private weak Label? get_label_widget() {
			weak Label label = get_child() as Label;
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
