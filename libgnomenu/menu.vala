using Gtk;

namespace Gnomenu {
	public class Menu : Gtk.Menu {
		public Menu() { }	
		construct {
			Gdk.Screen screen = toplevel.get_screen();
			Gdk.Colormap colormap = screen.get_rgba_colormap();
			if(colormap != null) {
				toplevel.set_colormap(colormap);
				set_colormap(colormap);
			}
		}
	}
}
