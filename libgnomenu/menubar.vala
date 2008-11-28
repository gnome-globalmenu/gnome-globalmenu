using Gtk;

namespace Gnomenu {
	public class MenuBar : Gtk.MenuBar {
		public MenuBar() {}
		public signal void activate(MenuItem item);
	}
}
