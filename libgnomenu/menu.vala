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
			foreach(weak Widget child in gtk_container_get_children(this)) {
				Item item = child as Item;
				if(item.item_id == id) return item;
			}
			return null;
		}
		public int get_item_position(Item item) {
			int i = 0;
			foreach(weak Widget child in gtk_container_get_children(this)) {
				if(child == item as Widget) {
					return i;	
				}
				i++;
			}
			return -1;
		}
		public void truncate(int length) {
			gtk_menu_shell_truncate(this, length);
		}
		public int length {
			get {
				return gtk_menu_shell_length(this);
			}
		
		}
		public void insert_item(Item item, int pos) {
			this.insert(item as MenuItem, pos);
		}
		public void remove_item(Item item) {
			this.remove(item as MenuItem);
		}
	}
}
