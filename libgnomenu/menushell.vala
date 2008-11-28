using Gtk;

namespace Gnomenu {
	public class MenuShell : Gtk.Widget {
		public int length {get{return _items_count;}}

		public virtual bool selecting {
			get {
				return _selecting;
			}
			set {
				_selecting = value;
				if(value == false) {
					selected_item = null;
				} else {
				}
			}
		}
		public weak MenuItem? selected_item {
			get {
				for(int i = 0; i< length; i++) {
					weak MenuItem m = get(i);
					if(m.state == StateType.SELECTED)
						return m;
				}
				return null;
			}
			set {
				for(int i = 0; i < length; i++) {
					weak MenuItem m = get(i);
					if(m.state == StateType.SELECTED) {
						if(m != value) {
							m.state = StateType.NORMAL;
							window.invalidate_rect((Gdk.Rectangle)m.allocation, false);
							m.deselect();
						}
					} else {
						if(m == value) {
							m.state = StateType.SELECTED;
							window.invalidate_rect((Gdk.Rectangle)m.allocation, false);
							m.select();
						}
					}
				}
			}
		}
		construct {
			_items_count = 0;
			_items = new MenuItem[10];
			set_flags(WidgetFlags.NO_WINDOW); /*should be in _init*/
			set_events(get_events() 
				| Gdk.EventMask.BUTTON_PRESS_MASK
				| Gdk.EventMask.KEY_PRESS_MASK
				| Gdk.EventMask.BUTTON_RELEASE_MASK
				| Gdk.EventMask.POINTER_MOTION_MASK
				| Gdk.EventMask.POINTER_MOTION_HINT_MASK
				);
		}

		public weak MenuItem get(int index) {
			assert(index >=0 && index < _items_count);
			return _items[index];
		}
		public MenuItem set(int index, MenuItem item) {
			assert(index >=0 && index < _items_count);
			MenuItem old = _items[index];
			old.parent = null;
			item.parent = this;
			_items[index] = item;
			return old;
		}
		public bool has(int index) {
			return (index >= 0 && index < _items_count);
		}
		public weak MenuItem append(MenuItem item) {
			if(_items_count == _items.length) {
				_items.resize(_items.length * 2);
			}
			_items[_items_count] = item;
			item.parent = this;
			_items_count ++;
			return item;
		}
		public void truncate(int length) {
			assert(length >= 0 && length <= _items_count);
			for(int i = length; i< _items_count; i++ ){
				_items[i].parent = null;
			}
			_items_count = length;
			for(int i = _items_count; i< _items.length; i++) {
				_items[i] = null;
			}
		}



/*GTK Widget Interface*/
		private override bool expose_event (Gdk.EventExpose event) {
			message("expose");
			for(int i = 0; i < length; i++) {
				get(i).paint(event.area);
			}
			return true;
		}
		private override void realize() {
			base.realize();
			Gdk.WindowAttr attr;
			attr.x = allocation.x;
			attr.y = allocation.y;
			attr.width = allocation.width;
			attr.height = allocation.height;
			attr.window_type = Gdk.WindowType.CHILD;
			attr.event_mask = get_events() ;
			attr.wclass = Gdk.WindowClass.INPUT_ONLY;
			int mask = Gdk.WindowAttributesType.X | Gdk.WindowAttributesType.Y;
			event_window = new Gdk.Window(window, attr, mask);
			event_window.set_user_data(this);
		}
		private override void unrealize() {
			event_window = null;
			base.unrealize();
		}
		private override void size_allocate(Gdk.Rectangle a) {
			message("size_allocate: %d %d %d %d", a.x, a.y, a.width, a.height);
			allocation = (Gtk.Allocation)a;
			if(0 != (get_flags() & WidgetFlags.REALIZED)) {
				event_window.move_resize(a.x, a.y, a.width, a.height);
			}

			for(int i = 0; i < length; i++) {
				Gtk.Allocation ca;
				if(this is MenuBar) {
					ca.x = a.x + a.width * i / length;
					ca.width = a.width / length;
					ca.height = a.height;
					ca.y = a.y;
				}
				if(this is Menu) {
					ca.y = a.y + a.width * i / length;
					ca.height = a.height / length;
					ca.width = a.width;
					ca.x = a.x;

				}
				this.get(i).allocation = ca;
			}
		}
		private override void size_request(out Gtk.Requisition r) {
			message("size_request");
			if(this is MenuBar) {
				r.width = this.length * 50;
				r.height = 10;
			} 
			if(this is Menu) {
				r.height = this.length * 10;
				r.width = 10;
			}
		}
		private override void map() {
			message("map");
			base.map();
			event_window.show();
		}
		private override void unmap() {
			event_window.hide();
			base.unmap();
		}
		private override bool button_press_event(Gdk.EventButton event) {
			int x = (int)event.x;
			int y = (int)event.y;
			message("button press %d, %d", x, y);
			translate_event_pos(ref x, ref y);
			weak MenuItem c = pos_to_item(x, y);
			if(c == null /*Clicking on nowhere, eg, out of the application*/
			|| selected_item == c /*Clicking in on the current selection*/
			){
				selecting = false;
			} else {
				selected_item = c;
				selecting = true;
			}

			return true;
		}
		private override bool motion_notify_event(Gdk.EventMotion event) {
			if(!selecting) return true;
			int x = (int)event.x;
			int y = (int)event.y;
			translate_event_pos(ref x, ref y);
			weak MenuItem c = pos_to_item(x, y);
			selected_item = c;
			return true;	
		}
		private override bool button_release_event(Gdk.EventButton event) {
			int x = (int)event.x;
			int y = (int)event.y;
			message("button release %d, %d", x, y);

			translate_event_pos(ref x, ref y);
			weak MenuItem c = pos_to_item(x, y);
			if(c.submenu == null) {
				c.activate();
			}
			return true;	
		}
		private void translate_event_pos(ref int x, ref int y) {
			x += allocation.x;
			y += allocation.y;
		}
		private weak MenuItem? pos_to_item(int x, int y) {
			for(int i = 0; i < length; i++) {
				weak MenuItem c = get(i);
				if(	x >= c.allocation.x 
					&& x <= c.allocation.width + c.allocation.x
					&& y >= c.allocation.y
					&& y <= c.allocation.height + c.allocation.y	
					) {
					return c;
				}
			}
			return null;
		}

		private bool key_press_event(Gdk.EventKey event) {
			message("key %u", event.keyval);
			uint LEFT = Gdk.keyval_from_name("LEFT");
			uint RIGHT = Gdk.keyval_from_name("RIGHT");
			uint UP = Gdk.keyval_from_name("UP");
			uint DOWN = Gdk.keyval_from_name("DOWN");
			if(event.keyval == LEFT) {

			}
			if(event.keyval == RIGHT) {

			}
			if(event.keyval == UP) {

			}
			if(event.keyval == DOWN) {

			}
			return true;	
		}
		protected Gdk.Window event_window;
		private bool _selecting;
		private MenuItem[] _items;
		private int _items_count;
	}
}
