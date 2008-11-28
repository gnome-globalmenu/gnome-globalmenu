using Gtk;


namespace Gnomenu {
	public class Menu : MenuShell {
		public Menu() { 
		}	
		public weak MenuItem item {
			get {return _item; }
			set {
				_item = value;
			}
		}
		construct {
			this.visible = true;
		}
		public void popup(int x, int y) {
			assert(item != null);
			toplevel_window.move(x, y);
			toplevel_window.show();
			selecting = true;
		}
		public void popdown() {
			toplevel_window.hide();
			selecting = false;
		}
		private Gtk.Window toplevel_window {
			get {
				if(_toplevel == null) {
					_toplevel = new Gtk.Window(WindowType.POPUP);
					_toplevel.set_type_hint(Gdk.WindowTypeHint.DROPDOWN_MENU);
					_toplevel.add(this);
				}
				return _toplevel;
			}
		}
		private Gtk.Window _toplevel;
		private weak MenuItem _item;
	}
}
