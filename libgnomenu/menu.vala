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
			toplevel = new Gtk.Window(WindowType.POPUP);
			toplevel.add(this);
			this.visible = true;
		}
		public void popup(int x, int y) {
			assert(item != null);
			selecting = true;
			toplevel.move(x, y);
			toplevel.show();
		}
		public void popdown() {
			toplevel.hide();
			selecting = false;
		}
		private Gtk.Window toplevel;
		private weak MenuItem _item;
	}
}
