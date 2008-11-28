
using Gtk;

namespace Gnomenu {
	public class MenuItem : GLib.Object {
		public MenuItem() {}
		public void paint(Gdk.Rectangle? area = null) {
			switch(state) {
				case StateType.NORMAL:
					Gtk.paint_arrow	(parent.get_style(),
							parent.get_parent_window(),
							state,
							ShadowType.NONE,
							area,
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
					Gtk.paint_arrow	(parent.get_style(),
							parent.get_parent_window(),
							state,
							ShadowType.NONE,
							area,
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
		}
		public string id;
		public MenuShell parent;
		public StateType state;
		public Gtk.Allocation allocation;
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
			weak MenuShell p = this.parent;
			weak MenuItem i;
			while(p is Menu) {
				i = (p as Menu).item;
				p = i.parent;
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
				x += allocation.height;
			}
		}
		private Menu _submenu;
	}
}
