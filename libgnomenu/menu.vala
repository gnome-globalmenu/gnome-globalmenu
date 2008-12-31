using Gtk;

namespace Gnomenu {
	public class Menu : Gtk.Menu {
		public Menu() { }	
		/*
		 * We don't do dispose, but gtk won't reset these pointers
		 * related to the colormaps and valgrind won't be happy.
		 */
		private override void dispose() {
			if(!disposed) {
				disposed = true;
				/*
				 * This is more or less a problem with
				 * GTK.
				 * these pointers are not set to zero.
				 *
				 * following code Doesn't work
				 */
				/*
				toplevel.set_colormap(null);
				set_colormap(null);
				*/
			}
			base.dispose();
		}
		static construct {
			Gtk.Settings.install_property(
					new ParamSpecBoolean("use-rgba-colormap",
						"use RGBA colormap if possible",
						"Use RGBA colormap if possible",
						false,
						ParamFlags.READABLE | ParamFlags.WRITABLE));
		}
		construct {
			get_settings().notify["use-rgba-colormap"] += (settings) => {
				bool val = false;
				settings.get("use-rgba-colormap", &val, null);
				use_rgba_colormap = val;
			};
			bool val = false;
			get_settings().get("use-rgba-colormap", &val, null);
			use_rgba_colormap = val;
		}
		private bool disposed = false;
		private bool _use_rgba_colormap = false;
		public bool use_rgba_colormap {
			get {
				return _use_rgba_colormap;
			}
			set {
				if(_use_rgba_colormap == value) return;
				_use_rgba_colormap = value;
				Gdk.Screen screen = toplevel.get_screen();
				Gdk.Colormap colormap = screen.get_rgba_colormap();
				if(colormap != null) {
					toplevel.set_colormap(colormap);
					set_colormap(colormap);
				}

			}
		}
	}
}
