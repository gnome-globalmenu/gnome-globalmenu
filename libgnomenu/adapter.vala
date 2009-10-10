public class Gnomenu.Adapter : GLib.Object, Gnomenu.Shell {
	public Gtk.MenuShell gtk_shell {get; construct set;}
	public Adapter(Gtk.MenuShell gtk_shell) {
		this.gtk_shell = gtk_shell;
	}

	public bool is_topmost { get; set; default = false;}
	/******
	 * Gnomenu.Shell interface
	 ********* */
	public Item? owner {
		get {
			if(is_topmost) return null;
			if(gtk_shell is Gtk.MenuBar) return null;
			if(gtk_shell is Gtk.Menu)
				return (gtk_shell as Gtk.Menu).get_attach_widget() as Item;
			return null;
		}
	}
	public Item? get_item(int position) {
		return gtk_menu_shell_get_item(gtk_shell, position) as Item;
	}
	public Item? get_item_by_id(string id) {
		int length = this.length;
		for(int i = 0; i < length; i++) {
			var item = get_item(i);
			if(item.item_id == id) return item;
		}
		return null;
	}
	public int get_item_position(Item item) {
		return gtk_menu_shell_get_item_position(gtk_shell, (Gtk.MenuItem*)item);
	}
	public int length {
		get {
			return gtk_menu_shell_get_length(gtk_shell);
		}
		set {
			gtk_menu_shell_set_length(gtk_shell, value);
		}
	}
}
