/** Grab mnemonic keys on a Gtk.Widget from a Gnomenu.Window **/
internal class Gnomenu.MnemonicKeys {
	private HashTable<uint, Gtk.Widget> keys
	    = new HashTable<uint, Gtk.Widget>(direct_hash, direct_equal);
	public Gnomenu.Shell shell {get; private set;}
	private Gnomenu.Window current_grab = null;

	public MnemonicKeys(Gnomenu.Shell shell) {
		this.shell = shell;
	}

	public void grab(Gnomenu.Window window) {
		if(current_grab != null) {
			ungrab();
		}
		Gdk.ModifierType mods = Gdk.ModifierType.MOD1_MASK;
		for(int i = 0; i< shell.length; i++) {
			var item = shell.get_item(i) as Gnomenu.MenuItem;
			if(item == null) continue;
			Gnomenu.MenuLabel label = item.get_child() as Gnomenu.MenuLabel;
			if(label == null) continue;
			uint keyval = label.mnemonic_keyval;
			debug("grabbing key for %s:%u", label.label, keyval);
			window.grab_key(keyval, mods);
			window.grab_key(keyval, mods | Gdk.ModifierType.MOD2_MASK);
			window.grab_key(keyval, mods | Gdk.ModifierType.MOD3_MASK);
			window.grab_key(keyval, mods | Gdk.ModifierType.MOD3_MASK | Gdk.ModifierType.MOD2_MASK);
			keys.insert(keyval, item);
		}
		current_grab = window;
		current_grab.set_key_widget(get_toplevel());
	}

	private Gtk.Widget? get_toplevel() {
		if(shell is Gtk.Widget) {
			return (shell as Gtk.Widget).get_toplevel();
		}
		if(shell is Gnomenu.Adapter) {
			return (shell as Gnomenu.Adapter).gtk_shell.get_toplevel();
		}
		return null;
	}
	public void ungrab() {
		Gdk.ModifierType mods = Gdk.ModifierType.MOD1_MASK;
		if(current_grab != null) {
			foreach(uint keyval in keys.get_keys()) {
				debug("ungrabbing %u", keyval);
				current_grab.ungrab_key(keyval, mods);
				current_grab.ungrab_key(keyval, mods | Gdk.ModifierType.MOD2_MASK);
				current_grab.ungrab_key(keyval, mods | Gdk.ModifierType.MOD3_MASK);
				current_grab.ungrab_key(keyval, mods | Gdk.ModifierType.MOD3_MASK | Gdk.ModifierType.MOD2_MASK);
			}
			current_grab.set_key_widget(null);
		}

		keys.remove_all();
		current_grab = null;
	}
	
}
