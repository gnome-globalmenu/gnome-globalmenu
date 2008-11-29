
using Gtk;

namespace Gnomenu {
	public enum MenuItemType {
		NORMAL,
		CHECK,
		RADIO,
		IMAGE,
	}
	public enum MenuItemState {
		UNTOGGLED,
		TOGGLED,
		TRISTATE,
	}
	public class MenuItem : Gtk.MenuItem {
		public MenuItem() {}
		static construct {
			install_style_property(new 
					ParamSpecInt("indicator-size", 
						"Indicator Size",
						"Size of check or radio indicator", 
						0, int.MAX, 13, ParamFlags.READABLE));
		}
		public MenuBar menubar {
			get;
			set;
		}
		public string? font {
			get {
				return _font;
			}
			set {
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
		public string? label {
			get {
				weak Widget bin_child = get_child();
				if(!(bin_child is Label)) return null;
				weak string text = (bin_child as Label).label;
				if(text == "") return null;
				return text;
			}
			set {
				weak Widget bin_child = get_child();
				weak Label label_widget;
				if(!(bin_child is Gtk.Label)) {
					replace(new Label(""));
					bin_child = get_child();
					bin_child.visible = true;
				}
				label_widget = bin_child as Label;
				if(value == null) {
					value = "";
				}
				label_widget.label = value;
			}
		}
		public string path {
			get {
				/* return: beginning with "/" if a menu bar is found. Not if not found.*/
				MenuItem item = this;
				MenuShell parent = item.parent as MenuShell;
				_path = position.to_string();

				while(!(parent is MenuBar)) {
					item = (parent as Menu).get_attach_widget() as MenuItem;
					if(item == null) break;
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
		public int position {
			set {
				int new_pos = value;
				set_data("position", (void*)new_pos);
			}
			get {
				return (int)get_data("position");
			}
		}
		public string? item_type {
			get {
				return item_type_to_string(_item_type);
			}
			set {
				MenuItemType new_type = item_type_from_string(value);
				if(new_type != _item_type) {
					_item_type = new_type;
					queue_resize();
				}
			}
		}
		public string? item_state {
			get {
				return item_state_to_string(_item_state);
			}
			set {
				MenuItemState new_state = item_state_from_string(value);
				if(new_state != _item_state) {
					_item_state = new_state;
					queue_draw();
				}
			}
		}
		public Gravity gravity {
			get {
				return _gravity;
			}
			set {
				_gravity = value;
				update_label_gravity();
			}
		}
		private string _path; /*merely a buffer*/
		private string _font;
		private Gravity _gravity;
		private MenuItemType _item_type;
		private MenuItemState _item_state;
		private Widget? replace(Widget new_child) {
			Widget old_child = get_child();
			if(old_child != null) {
				remove(old_child);
			}
			add(new_child);
			return old_child;
		}
		private void update_label_gravity() {
			double text_angle = gravity_to_text_angle(gravity);
			Label label = get_child() as Label;
			if(label != null) {
				label.angle = text_angle;
			}
		}
		private override void add(Gtk.Widget widget) {
			base.add(widget);
			if(widget is Label) {
				update_label_gravity();
			}
		}
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
	}
}
