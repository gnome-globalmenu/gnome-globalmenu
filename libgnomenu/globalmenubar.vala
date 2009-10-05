public class Gnomenu.GlobalMenuBar : Gnomenu.MenuBar {
	private bool _per_monitor_mode = true;
	public bool per_monitor_mode {
		get { return _per_monitor_mode;}
		set {
			_per_monitor_mode = value;
			change_active_window(null);
		}
	}
	private bool _grab_keys = true;
	public bool grab_keys {
		get {
			return _grab_keys;
		}
		set {
			_grab_keys = value;
			update();
		}
	}
	private bool _gnome_shell_mode = false;
	private Gnomenu.Shell _main_shell = null;
	private Gnomenu.Shell main_shell {
		get {
			if(_main_shell != null) return _main_shell;
			return this;
		}
		set {
			if(_main_shell != null) {
				_main_shell.activate -= item_activated;
				_main_shell.select -= item_selected;
				_main_shell.deselect -= item_deselected;
			}
			_main_shell = value;
			if(_main_shell == null) _main_shell = this;
			_main_shell.activate += item_activated;
			_main_shell.select += item_selected;
			_main_shell.deselect += item_deselected;

		}
	}
	public bool gnome_shell_mode {
		get { return _gnome_shell_mode; }
		set {
			if(value != _gnome_shell_mode) {
				_gnome_shell_mode = value;
				if(!_gnome_shell_mode) {
					gtk_menu_shell_remove_all(this);
					main_shell = this;
				} else {
					gtk_menu_shell_remove_all(this);
					Gtk.MenuItem item = new Gtk.MenuItem.with_label("Menu");
					item.visible = true;
					this.append(item);
					Gnomenu.Menu menu = new Gnomenu.Menu();
					menu.is_topmost = true;
					item.submenu = menu;
					main_shell = menu;
				}
			}
			update();
		}
	}
	private Gnomenu.Window _current_window;
	[CCode (notify = true)]
	public Gnomenu.Window current_window { get {
			return _current_window;
		}
		private set {
			var old = _current_window;
			_current_window = value;
			update(old);
		}
	}

	private Gnomenu.Window _root_window;
	private Gnomenu.Monitor active_window_monitor;

	/* the number(id) of the physical monitor this widget
	 * resides */
	private int monitor_num {
		get {
			if(!this.is_realized()) {return -1;}
			Gdk.Screen screen = get_screen();
			return screen.get_monitor_at_window(this.window);
		}
	}

	private int get_monitor_num_at_pointer() {
		if(window == null) return -1;
		Gdk.Screen screen = this.get_screen();
		if(screen == null) return -1;
		Gdk.Display display = this.get_display();
		int x, y;
		display.get_pointer(null, out x, out y, null);
		return screen.get_monitor_at_point(x, y);
	}

	private void change_active_window(Gnomenu.Window? prev) {
		if(prev != null) {
			prev.menu_context_changed -= menu_context_changed;
		}
		Gnomenu.Window @new = active_window_monitor.active_window;
		if(_per_monitor_mode) {
			int num = this.monitor_num;
			int win_num = -1;
			if(@new != null) win_num = @new.get_monitor_num();
			if(win_num == -1) win_num = get_monitor_num_at_pointer();
			if(num != -1 && win_num != -1 && win_num != num) {
				debug("%p, current window on monitor(%d), me on (%d) skipped", this, num, win_num);
				if(current_window != null
				&& !current_window.is_on_active_workspace()) {
					current_window = null;
				}
				return;
			}
		}

		current_window = @new;
		debug("%p, current window changed to %p", this, current_window);
		if(current_window != null) {
			current_window.menu_context_changed += menu_context_changed;
		}
	}
	construct {
		main_shell = this;
		active_window_monitor = new Gnomenu.Monitor(this.get_screen());
		active_window_monitor.active_window_changed += change_active_window;
	}

	private HashTable<uint, Gtk.Widget> keys = new HashTable<uint, Gtk.Widget>(direct_hash, direct_equal);

	private void item_activated (Gnomenu.Shell menubar, Gnomenu.Item item){
		if(current_window != null) {
			current_window.emit_menu_event(item.item_path);
		}
	}
	private void item_selected (Gnomenu.Shell menubar, Gnomenu.Item item){
		if(current_window != null) {
			current_window.emit_menu_select(item.item_path, null);
		}
	}
	private void item_deselected (Gnomenu.Shell menubar, Gnomenu.Item item){
		if(current_window != null) {
			current_window.emit_menu_deselect(item.item_path);
		}
	}
	private void menu_context_changed(Gnomenu.Window window) {
		/*
		 * If window is not current window, 
		 * some where around the signal handler connection is wrong
		 * */
		assert(window == current_window);
		debug("menu_context_changed on %p", window);
		update();
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

	private void regrab_menu_bar_key() {
		debug("regrab menu_bar key");
		ungrab_menu_bar_key();	
		grab_menu_bar_key();	
	}
	private void attach_to_screen(Gdk.Screen screen) {
		active_window_monitor.attach(screen);
		_root_window = new Window(get_root_window());
		_root_window.set_key_widget(this.get_toplevel());
		grab_menu_bar_key();
		if(_grab_keys && _current_window != null) {
			grab_mnemonic_keys(_current_window);
		}
		var settings = get_settings();
		settings.notify["gtk-menu-bar-accel"] += regrab_menu_bar_key;
			
	}
	private void detach_from_screen(Gdk.Screen screen) {
		if(_root_window != null) {
			_root_window.set_key_widget(null);
			ungrab_menu_bar_key();
			if(_current_window != null)
				ungrab_mnemonic_keys(_current_window);
		}
		var settings = get_settings();
		settings.notify["gtk-menu-bar-accel"] -= regrab_menu_bar_key;
		_root_window = null;
	}
	private void chainup_key_changed(Gtk.Window window) {
		GLib.Type type = typeof(Gtk.Window);
		var window_class = (Gtk.WindowClass) type.class_ref();
		debug("chainup to Gtk.Window keys changed");
		window_class.keys_changed(window);
	}
	public override void hierarchy_changed(Gtk.Widget? old_toplevel) {
		var toplevel = this.get_toplevel() as Gtk.Plug;
		/* Manually chain-up to the default keys_changed handler,
		 * Working around a problem with GtkPlug/GtkSocket */
		if(toplevel != null) {
			toplevel.keys_changed += chainup_key_changed;
		}
		if((old_toplevel as Gtk.Plug)!= null) {
			(old_toplevel as Gtk.Plug).keys_changed -= chainup_key_changed;
		}
	}
	public override void screen_changed(Gdk.Screen? previous_screen) {
		Gdk.Screen screen = get_screen();
		if(previous_screen != screen) {
			if(previous_screen != null) detach_from_screen(previous_screen);
			if(screen != null) attach_to_screen(screen);
		}
	}

	private void update(Gnomenu.Window? prev_window = null) {
		if(prev_window == null) {
			prev_window = _current_window;
		}
		if(prev_window != null) {
			/* prev window can still be null */
			ungrab_mnemonic_keys(prev_window);
			prev_window.set_key_widget(null);
		}
		hide();
		if(_current_window == null) return;
		var context = _current_window.get_menu_context();
		if(context == null) return;
		try {
			Parser.parse(main_shell, context);
			show();
		} catch(GLib.Error e) {
			warning("%s", e.message);
		}
		if(_grab_keys) {
			_current_window.set_key_widget(this.get_toplevel());
			grab_mnemonic_keys(_current_window);
		}
		return;
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
