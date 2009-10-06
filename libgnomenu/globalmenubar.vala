public class Gnomenu.GlobalMenuBar : Gnomenu.MenuBar {
	private Gnomenu.Window _root_window;
	private Gnomenu.Monitor active_window_monitor;
	private HashTable<uint, Gtk.Widget> keys
	    = new HashTable<uint, Gtk.Widget>(direct_hash, direct_equal);


	public bool per_monitor_mode {
		get { return active_window_monitor.per_monitor_mode;}
		set {
			active_window_monitor.per_monitor_mode = value;
		}
	}

	private bool _grab_keys = true;
	public bool grab_keys {
		get { return _grab_keys; }
		set {
			_grab_keys = value;
			regrab_keys();
		}
	}

	public Gnomenu.Window active_window {
		get { return active_window_monitor.active_window; }
	}

	public signal void active_window_changed(Gnomenu.Window? prev_window);


	construct {
		active_window_monitor = new Gnomenu.Monitor(this.get_screen());
		active_window_monitor.managed_shell = this;
		/*FIXME: How do we sync the monitor_num with the applet? */
		active_window_monitor.monitor_num = -1;
		active_window_monitor.active_window_changed += regrab_keys0;
		active_window_monitor.shell_rebuilt += regrab_keys;
		active_window_monitor.active_window_changed += emit_active_window_changed;
		this.activate += item_activated;
		this.select += item_selected;
		this.deselect += item_deselected;
		this.screen_changed += _screen_changed;
		this.hierarchy_changed += _hierarchy_changed;
		this.hierarchy_changed += _hierarchy_changed_chain_keys;
	}

	private void emit_active_window_changed(Gnomenu.Window? prev_window) {
		active_window_changed(prev_window);
	}

	private void item_activated (Gnomenu.Item item){
		if(active_window != null) {
			active_window.emit_menu_event(item.item_path);
		}
	}
	private void item_selected (Gnomenu.Item item){
		if(active_window != null) {
			active_window.emit_menu_select(item.item_path, null);
		}
	}
	private void item_deselected (Gnomenu.Item item){
		if(active_window != null) {
			active_window.emit_menu_deselect(item.item_path);
		}
	}

	private bool really_should_grab_keys () {
		return _grab_keys &&
			active_window_monitor.has_pointer();
	}
	private void regrab_keys() {
		regrab_keys0(active_window);
	}
	private void regrab_keys0(Gnomenu.Window? prev_window) {
		if(prev_window != null) {
			ungrab_mnemonic_keys(prev_window);
			prev_window.set_key_widget(null);
		}
		if(really_should_grab_keys() && active_window != null) {
			active_window.set_key_widget(this.get_toplevel());
			grab_mnemonic_keys(active_window);
		}
	}

	private void attach_to_screen(Gdk.Screen screen) {
		active_window_monitor.attach(screen);
		_root_window = new Window(get_root_window());
		_root_window.set_key_widget(this.get_toplevel());
		grab_menu_bar_key();
		if(really_should_grab_keys() && active_window != null) {
			grab_mnemonic_keys(active_window);
		}
		var settings = get_settings();
		settings.notify["gtk-menu-bar-accel"] += regrab_menu_bar_key;
			
	}
	private void detach_from_screen(Gdk.Screen screen) {
		if(_root_window != null) {
			_root_window.set_key_widget(null);
			ungrab_menu_bar_key();
			if(active_window != null)
				ungrab_mnemonic_keys(active_window);
		}
		var settings = get_settings();
		settings.notify["gtk-menu-bar-accel"] -= regrab_menu_bar_key;
		_root_window = null;
	}

	private void grab_mnemonic_keys(Gnomenu.Window window) {
		Gdk.ModifierType mods = Gdk.ModifierType.MOD1_MASK;
		foreach(Gtk.Widget widget in get_children()) {
			Gnomenu.MenuItem item = widget as Gnomenu.MenuItem;
			if(item == null) continue;
			Gnomenu.MenuLabel label = item.get_child() as Gnomenu.MenuLabel;
			if(label == null) continue;
			uint keyval = label.mnemonic_keyval;
			debug("grabbing key for %s:%u", label.label, keyval);
			window.grab_key(keyval, mods);
			keys.insert(keyval, widget);
		}
	}

	private void ungrab_mnemonic_keys(Gnomenu.Window window) {
		Gdk.ModifierType mods = Gdk.ModifierType.MOD1_MASK;
		foreach(uint keyval in keys.get_keys()) {
			debug("ungrabbing %u", keyval);
			window.ungrab_key(keyval, mods);
		}
		keys.remove_all();
	}

	private bool sync_monitor_num() {
		var screen = get_screen();
		active_window_monitor.monitor_num = 
		screen.get_monitor_at_window(this.window);
		return false;
	}
	private void _hierarchy_changed(Gtk.Widget? old_toplevel) {
		var toplevel = this.get_toplevel() as Gtk.Plug;
		if(toplevel != null) {
			toplevel.configure_event += sync_monitor_num;
		}
		if(old_toplevel != null)
			toplevel.configure_event -= sync_monitor_num;
	}

	private void chainup_key_changed(Gtk.Window window) {
		GLib.Type type = typeof(Gtk.Window);
		var window_class = (Gtk.WindowClass) type.class_ref();
		debug("chainup to Gtk.Window keys changed");
		window_class.keys_changed(window);
	}
	private void _hierarchy_changed_chain_keys(Gtk.Widget? old_toplevel) {
		warning("chain up hack");
		var toplevel = this.get_toplevel() as Gtk.Plug;
		if(toplevel != null) {
		/* Manually chain-up to the default keys_changed handler,
		 * Working around a problem with GtkPlug/GtkSocket */
			toplevel.keys_changed += chainup_key_changed;
		}
		if((old_toplevel as Gtk.Plug)!= null) {
		/* Manually chain-up to the default keys_changed handler,
		 * Working around a problem with GtkPlug/GtkSocket */
			(old_toplevel as Gtk.Plug).keys_changed -= chainup_key_changed;
		}
	}

	private void _screen_changed(Gdk.Screen? previous_screen) {
		Gdk.Screen screen = get_screen();
		if(previous_screen != screen) {
			if(previous_screen != null) detach_from_screen(previous_screen);
			if(screen != null) attach_to_screen(screen);
		}
	}

	private void regrab_menu_bar_key() {
		debug("regrab menu_bar key");
		ungrab_menu_bar_key();
		grab_menu_bar_key();
	}
	private void ungrab_menu_bar_key() {
		int keyval = (int) _root_window.get_data("menu-bar-keyval");
		Gdk.ModifierType mods = 
			(Gdk.ModifierType) _root_window.get_data("menu-bar-keymods");

		_root_window.ungrab_key(keyval, mods);
		_root_window.set_data("menu-bar-keyval", null);
		_root_window.set_data("menu-bar-keymods", null);
	}
	private void grab_menu_bar_key() {
		/*FIXME: listen to changes in GTK_SETTINGS.*/
		uint keyval;
		Gdk.ModifierType mods;
		get_accel_key(out keyval, out mods);
		_root_window.grab_key(keyval, mods);
		_root_window.set_data("menu-bar-keyval", (void*) keyval);
		_root_window.set_data("menu-bar-keymods", (void*) mods);
	}
	/**
	 * return the accelerator key combination for invoking menu bars
	 * in GTK Settings. It is usually F10.
	 */
	private void get_accel_key(out uint keyval, out Gdk.ModifierType mods) {
		Gtk.Settings settings = get_settings();
		string accel = null;
		settings.get("gtk_menu_bar_accel", &accel, null);
		if(accel != null)
			Gtk.accelerator_parse(accel, out keyval, out mods);
	}
}
