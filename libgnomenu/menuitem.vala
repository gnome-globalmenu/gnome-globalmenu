
using Gtk;

namespace Gnomenu {
	public class MenuItem : Gtk.Widget {
		public MenuItem() {}
		construct {
			set_flags(WidgetFlags.NO_WINDOW);
		}
		public string id;
		public string label;
		public Menu submenu {
			get {
				return _submenu;
			}
			set {
				if(_submenu != null) _submenu.item = null;
				_submenu = value;
				if(_submenu != null) _submenu.item = this;
			}
		}
		public void select() {
			if(_submenu != null) {
				int x;
				int y;
				get_popup_position(out x, out y);
				submenu.popup(x, y);
			}
		}
		public void deselect() {
			if(_submenu != null) {
				submenu.popdown();
			}
		}
		public void activate() {
			message("activate");
			weak MenuShell p = this.parent as MenuShell;
			weak MenuItem i;
			while(p is Menu) {
				i = (p as Menu).item;
				p = i.parent as MenuShell;
			}
			/*OK p is the MenuBar now*/
			p.selecting = false;
		}
		public void get_origin(out int x, out int y) {
			if(0!= (parent.get_flags() & WidgetFlags.REALIZED)){
				parent.window.get_origin(out x, out y);
			} else {
				x = 0;
				y = 0;
			}
			x += allocation.x;
			y += allocation.y;
		}
		public void get_popup_position(out int x, out int y) {
			get_origin(out x, out y);
			if(parent is MenuBar) {
				y += allocation.height;
			}
			if(parent is Menu) {
				x += allocation.width;
			}
		}

		private override bool expose_event(Gdk.EventExpose event) {
			switch(state) {
				case StateType.NORMAL:
					Gtk.paint_arrow	(style,
							window,
							(StateType)state,
							ShadowType.NONE,
							event.area,
							parent,
							null,
							ArrowType.UP,
							true, 
							allocation.x,
							allocation.y,
							allocation.width,
							allocation.height);
				break;
				case StateType.SELECTED:
					Gtk.paint_arrow	(style,
							window,
							(StateType)state,
							ShadowType.NONE,
							event.area,
							parent,
							null,
							ArrowType.DOWN,
							true, 
							allocation.x,
							allocation.y,
							allocation.width,
							allocation.height);
				break;
			}
			return false;
		}
		private override void size_request(out Gtk.Requisition r) {
			r.width = 100;
			r.height = 16;
		}
		private Menu _submenu;
		private Pango.Layout layout;
	}
}
