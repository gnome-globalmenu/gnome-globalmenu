using Gtk;

namespace Gnomenu {
	public class MenuShell : Gtk.Container {
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
			old.unparent();
			item.parent = this;
			_items[index] = item;
			return old;
		}
		public int index(MenuItem item) {
			for(int i = 0; i < _items_count; i++) {
				if(_items[i] == item) {
					return i;
				}
			}	
			return -1;
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
		public void truncate(int new_length) {
			assert(new_length >= 0 && new_length <= _items_count);
			for(int i = new_length; i< _items_count; i++ ){
				_items[i].unparent();
			}
			_items_count = new_length;
			for(int i = _items_count; i< _items.length; i++) {
				_items[i] = null;
			}
		}

/* GTK Container Interface */
		/** this function is heavily patched by sed */
		private override void forall(Gtk.Callback cb, void* data) {
			bool include_internal;
			for(int i = 0; i < _items_count; i++) {
				cb(_items[i]);
			}
		}
		private override void remove(Gtk.Widget child) {
			for(int i = 0; i< _items_count; i++) {
				if(_items[i] == child) {
					_items[i].unparent();
					_items[i] = null;
					if( i < _items_count - 1) {
						Memory.move(&_items[i], &_items[i+1], sizeof(MenuItem) * (_items_count - i - 1));
					}
					Memory.set(&_items[_items_count-1], 0, sizeof(MenuItem));
					_items_count --;
					break;
				}
			}
		}
		private override void add(Gtk.Widget child) {
			append(child as MenuItem);
		}
/*GTK Widget Interface*/
		private override bool expose_event (Gdk.EventExpose event) {
			return base.expose_event(event);
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

			int x = a.x;
			int y = a.y;
			for(int i = 0; i < length; i++) {
				Gtk.Allocation ca;
				Gtk.Requisition cr;
				get(i).get_child_requisition(out cr);
				ca.x = x;
				ca.y = y;
				ca.width = cr.width;
				ca.height = cr.height;
				if(this is MenuBar) {
					x += ca.width;
					ca.height = a.height;
				}
				if(this is Menu) {
					y += ca.height;
					ca.width = a.width;
				}
				this.get(i).size_allocate((Gdk.Rectangle)ca);
			}
		}
		private override void size_request(out Gtk.Requisition r) {
			message("size_request");
			r.width = 0;
			r.height = 0;
			for(int i = 0; i < length; i++) {
				Gtk.Requisition cr;
				(get(i) as Gtk.Widget).size_request(out cr);
				if(this is MenuBar) {
					r.width += cr.width;
					r.height = cr.height > r.height? cr.height:r.height;
				} 
				if(this is Menu) {
					r.height += cr.height;
					r.width = cr.width > r.width? cr.width:r.width;
				}
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

			if(c!= null && c.submenu == null) {
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

		private bool key_press_event(Gtk.Widget toplevel, Gdk.EventKey event) {
			message("key %u", event.keyval);
			uint LEFT = Gdk.keyval_from_name("Left");
			uint RIGHT = Gdk.keyval_from_name("Right");
			uint UP = Gdk.keyval_from_name("Up");
			uint DOWN = Gdk.keyval_from_name("Down");
			if(selecting) {
				int i = index(selected_item);
				if(event.keyval == LEFT) {
					i--;
				}
				if(event.keyval == RIGHT) {
					i++;
				}
				if(event.keyval == UP) {
					i--;
				}
				if(event.keyval == DOWN) {
					i++;
				}
				selected_item = get(i);
			}
			return true;	
		}

		private override void hierarchy_changed(Gtk.Widget previous_toplevel) {
			message("hierarchy changed");
			if(previous_toplevel != null) {
				previous_toplevel.key_press_event -= key_press_event;
			}
			weak Gtk.Widget toplevel = get_toplevel();
			if(toplevel != null) {
				toplevel.key_press_event += key_press_event;
			}
		}
		protected Gdk.Window event_window;
		private bool _selecting;
		private MenuItem[] _items;
		private int _items_count;
	}
}
