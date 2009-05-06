using Gtk;

namespace Gnomenu {
	public class Menu : Gtk.Menu, Gnomenu.Shell {
		public Menu() { }	
		/*
		 * We don't do dispose, but gtk won't reset these pointers
		 * related to the colormaps and valgrind won't be happy.
		 */
		public override void dispose() {
			if(!disposed) {
				disposed = true;
			}
			base.dispose();
		}
		static construct {
		}
		construct {
			weak string rgba = Environment.get_variable("LIBGNOMENU_ENABLE_RGBA");
			use_rgba_colormap = (rgba != null);
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
		/******
		 * Gnomenu.Shell interface
		 ********* */
		public Item? owner {
			get {
				return get_attach_widget() as Item;
			}
		}
		public Item? get_item(int position) {
			return gtk_menu_shell_get_item(this, position) as Item;
		}
		public Item? get_item_by_id(string id) {
			foreach(weak Widget child in get_children()) {
				Item item = child as Item;
				if(item.item_id == id) return item;
			}
			return null;
		}
		public int get_item_position(Item item) {
			return gtk_menu_shell_get_item_position(this, item as MenuItem);
		}
		public int length {
			get {
				return gtk_menu_shell_length_without_truncated(this);
			}
			set {
				gtk_menu_shell_truncate(this, value);
			}
		}
	}
}
