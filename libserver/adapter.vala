/**
 * Adapting any Gtk.MenuShell to a Gnomenu.Shell.
 * AKA, if you have a Gtk.MenuShell, and want to 
 * use Gnomenu.Parser to put Gnomenu.MenuItem into it,
 * then use this adapter.
 *
 * WARNING:
 * Never touch any children with the type Gnomenu.MenuItem!
 */
public class Gnomenu.Adapter : GLib.Object, Gnomenu.Shell {
	public Gtk.MenuShell gtk_shell {get; construct set;}

	public Adapter(Gtk.MenuShell gtk_shell) {
		this.gtk_shell = gtk_shell;
	}

	construct {
		a2g.insert(this, gtk_shell);
		g2a.insert(gtk_shell, this);
	}
	bool disposed = false;
	public override void dispose() {
		if(!disposed) {
			a2g.remove(this);
			g2a.remove(gtk_shell);
			disposed = true;
		}
		base.dispose();
	}

	public static unowned Adapter? get_adapter(Gtk.MenuShell gtk_shell) {
		if(g2a == null) return null;
		unowned Adapter a = g2a.lookup(gtk_shell);
		return a;
	}

	private static HashTable<unowned Adapter, Gtk.MenuShell> a2g
	= new HashTable<unowned Adapter, Gtk.MenuShell>(direct_hash, direct_equal);
	private static HashTable<Gtk.MenuShell, unowned Adapter> g2a
	= new HashTable<Gtk.MenuShell, unowned Adapter>(direct_hash, direct_equal);

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
