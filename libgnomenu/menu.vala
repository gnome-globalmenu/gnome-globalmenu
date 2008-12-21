using Gtk;

namespace Gnomenu {
	public class Menu : Gtk.Menu {
		public Menu() { }	
				/*
		private override void dispose() {
			if(!disposed) {
				disposed = true;
				 * This is more or less a problem with
				 * GTK.
				 * these pointers are not set to zero.
				 *
				toplevel.set_colormap(null);
				set_colormap(null);
			}
			base.dispose();
		}
				*/
		construct {
			Gdk.Screen screen = toplevel.get_screen();
			Gdk.Colormap colormap = screen.get_rgba_colormap();
			if(colormap != null) {
				toplevel.set_colormap(colormap);
				set_colormap(colormap);
			}
		}
//		private bool disposed = false;
	}
}
