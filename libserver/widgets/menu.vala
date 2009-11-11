public class Gnomenu.Menu : Gtk.Menu, Gnomenu.Shell {
	public Menu() { }	
	static construct {
		MenuItem _include_menu_item_definiation;
	}
	construct {
		use_rgba_colormap = default_use_rgba_colormap;
	}
	private bool _use_rgba_colormap = false;
	public static bool default_use_rgba_colormap;
	/* if the menu is logically the topmost shell,
	 * even if it is a submenu of some item */
	public bool is_topmost { get; set; default = false;}
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
	public override void destroy() {
		gtk_menu_shell_remove_all(this);
		base.destroy();
	}
	/******
	 * Gnomenu.Shell interface
	 ********* */
	public Item? owner {
		get {
			if(is_topmost) return null;
			return get_attach_widget() as Item;
		}
	}
	public Item? get_item(int position) {
		return gtk_menu_shell_get_item(this, position) as Item;
	}
	public Item? get_item_by_id(string id) {
		foreach(var child in get_children()) {
			var item = child as Item;
			if(item == null) continue;
			if(item.item_id == id) return item;
		}
		return null;
	}
	public int get_item_position(Item item) {
		return gtk_menu_shell_get_item_position(this, item as MenuItem);
	}
	public int length {
		get {
			return gtk_menu_shell_get_length(this);
		}
		set {
			gtk_menu_shell_set_length(this, value);
		}
	}
}
