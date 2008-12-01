using Gtk;

namespace Gnomenu {
	public class MenuItem : Gtk.MenuItem {
		public MenuItem() { }
		static construct {
			install_style_property(new 
					ParamSpecInt("indicator-size", 
						"Indicator Size",
						"Size of check or radio indicator", 
						0, int.MAX, 13, ParamFlags.READABLE));
		}
		construct {
			add(new Label("N/A"));
			get_child().visible = true;
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
				if(new_type == _item_type) return;
				_item_type = new_type;
				if(_item_type == MenuItemType.SEPARATOR) {
					Widget child = get_child();
					remove(child);
				} else {
					if(get_child() == null) {
						add(new Label(""));
						get_child().visible = true;
						update_label_gravity();
						update_label_text();
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
			}
		}
		private string _path; /*merely a buffer*/
		private string _font;
		private string _label;
		private string _id;
		private int _position;
		private Gravity _gravity;
		private MenuItemType _item_type;
		private MenuItemState _item_state;

		private override void toggle_size_request(void* requisition) {
			switch(_item_type) {
				case MenuItemType.CHECK:
				case MenuItemType.RADIO:
					int toggle_spacing = 0;
					int indicator_size = 0;
					style_get("toggle-spacing", &toggle_spacing,
						"indicator-size", &indicator_size, null);
					*((int*) requisition ) = indicator_size + toggle_spacing;
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
			weak string detail = null;
			switch(_item_type) {
				case MenuItemType.CHECK:
					detail = "check";
				break;
				case MenuItemType.RADIO:
					detail = "option";
				break;
			}
			if(detail != null)
				Gtk.paint_option(style,
						window,
						(StateType)state,
						shadow_type,
						event.area, 
						this,
						detail,
						allocation.x,
						allocation.y,
						indicator_size,
						indicator_size);
			return false;
		}

		private override void activate() {
			menubar.activate(this);
		}
		private void update_label_gravity() {
			if(_item_type == MenuItemType.SEPARATOR) return;
			double text_angle = gravity_to_text_angle(gravity);
			Label label = get_child() as Label;
			assert(label != null);
			label.angle = text_angle;
		}
		private void update_label_text() {
			if(_item_type == MenuItemType.SEPARATOR) return;
			string text;
			text = _label;
			if(text == null)
				text = path;

			Label label = get_child() as Label;
			assert(label != null);
			label.label = text;
		}
		private override void parent_set(Gtk.Widget old_parent) {
			update_label_text();
		}
	}
}
