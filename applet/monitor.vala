using Gnomenu;
using Wnck;

public class Monitor: GLib.Object {
	public Wnck.Screen screen {
		get {
			return _wnck_screen;	
		}
		set {
			Wnck.Screen new_screen = value;
			if(new_screen == null) {
				new_screen = Wnck.Screen.get_default();
			}
			if(new_screen == _wnck_screen) return;

			if(_wnck_screen != null) {
				detach_from_screen(_wnck_screen);
			}

			if(_root_gnomenu_window != null) {
				_root_gnomenu_window.destroy();
				ungrab_menu_bar_key(_root_gnomenu_window);
				_root_gnomenu_window = null;
			}
			_wnck_screen = new_screen;
			int number = _wnck_screen.get_number();
			Gdk.Screen gdk_screen = Gdk.Display.get_default().get_screen(number);
			_root_gnomenu_window = Gnomenu.Window.new_from_gdk_window(gdk_screen.get_root_window());
			grab_menu_bar_key(_root_gnomenu_window);
			_wnck_screen.window_closed += on_window_closed;
			_wnck_screen.window_opened += on_window_opened;
			_wnck_screen.active_window_changed += on_active_window_changed;
			_wnck_screen.active_window_changed (null);
			_desktop = find_desktop(_wnck_screen);
		}
	}
	public Gnomenu.MenuBar? menubar {
		get {
			return _menubar;
		}
		set {
			if(_menubar != null) {
				_menubar.activate -= on_activate;	
			}
			_menubar = value;
			if(_menubar != null) {
				_menubar.activate += on_activate;
				update_menubar();				
			}
		}
	}
	public Wnck.Window? current_window {
		get {
			return _current_window;
		}
	}
	public virtual signal void window_changed(Wnck.Window? old_window);
	
	public Monitor() {
	
	}

	construct {
		screen = Wnck.Screen.get_default();
		disposed = false;
	}

	public void dispose() {
		if(!disposed) {
			disposed = true;
			detach_from_screen(_wnck_screen);			
			if(_current_gnomenu_window != null) {
				_current_gnomenu_window.destroy();
				_current_gnomenu_window = null;
			}
			if(_root_gnomenu_window != null) {
				_root_gnomenu_window.destroy();
				_root_gnomenu_window = null;
			}
		}
	}
	private Wnck.Screen? _wnck_screen;
	private Wnck.Window _desktop;
	private Wnck.Window _current_window;
	private Gnomenu.Window _current_gnomenu_window;
	private Gnomenu.Window _root_gnomenu_window;
	private Gnomenu.MenuBar _menubar;

	private bool disposed;

	private void on_activate(Gnomenu.MenuBar menubar, Gnomenu.MenuItem item){
		if(_current_gnomenu_window != null) {
			_current_gnomenu_window.emit_menu_event(item.path);
		}
	}
	private void on_window_closed(Wnck.Screen screen, Wnck.Window window) {
		if(_desktop == window) {
			_desktop = null;
		}
		if(window == current_window) {
			update_current_window();
		}
	}
	private void on_window_opened(Wnck.Screen screen, Wnck.Window window) {
		if(window.get_window_type() == Wnck.WindowType.DESKTOP)
			_desktop = window;
	}
	private void on_active_window_changed (Wnck.Screen screen, Wnck.Window previous_window) {
		weak Wnck.Window window = screen.get_active_window();
		if((window != previous_window) && (window is Wnck.Window)) {
			weak Wnck.Window transient_for = window.get_transient();
			if(transient_for != null) window = transient_for;
			switch(window.get_window_type()) {
				case Wnck.WindowType.NORMAL:
				case Wnck.WindowType.DESKTOP:
					update_current_window();
				break;
				default:
					/*Do nothing if it is a toolbox or so*/
				break;
			}
		}
	}

	private Wnck.Window? find_desktop(Wnck.Screen screen) {
		weak List<weak Wnck.Window> windows = screen.get_windows();
		foreach(weak Wnck.Window window in windows) {
			if(window.get_window_type() == Wnck.WindowType.DESKTOP) {
				/*Vala should ref it*/
				return window;
			}
		}
		return null;
	}
	private void update_current_window() {
		Wnck.Window old = _current_window;
		_current_window = _wnck_screen.get_active_window();
		if(_current_window == null)
			_current_window = _desktop;
		if(old == _current_window) return;
		if( _current_gnomenu_window != null) {
			_current_gnomenu_window.destroy();
			_current_gnomenu_window = null;
		}
		if(_current_window != null) {
			_current_gnomenu_window = 
				Gnomenu.Window.new_from_native(_current_window.get_xid());
		}
		if(_current_gnomenu_window != null) {
			_current_gnomenu_window.menu_context_changed += (window) => {
				update_menubar();
			};
		}
		update_menubar();
		window_changed(old);
	}
	private void detach_from_screen(Wnck.Screen screen) {
		_wnck_screen.window_opened -= on_window_opened;
		_wnck_screen.window_closed -= on_window_closed;
		_wnck_screen.active_window_changed -= on_active_window_changed;
	}
	private void ungrab_menu_bar_key(Gnomenu.Window window) {
		int keyval = (int) window.get_data("menu-bar-keyval");
		Gdk.ModifierType mods = 
			(Gdk.ModifierType) window.get_data("menu-bar-keymods");

		window.ungrab_key(keyval, mods);
		window.key_press_event -= on_menu_bar_key;
		window.set_data("menu-bar-keyval", null);
		window.set_data("menu-bar-keymods", null);
	}
	private void grab_menu_bar_key(Gnomenu.Window window) {
		/*FIXME: listen to changes in GTK_SETTINGS.*/
		int keyval;
		Gdk.ModifierType mods;
		get_accel_key(window, out keyval, out mods);
		window.grab_key(keyval, mods);
		window.key_press_event += on_menu_bar_key; 
		window.set_data("menu-bar-keyval", (void*) keyval);
		window.set_data("menu-bar-keymods", (void*) mods);
	}	
	private bool on_menu_bar_key (Gnomenu.Window window, Gdk.EventKey event) {
		uint keyval;
		Gdk.ModifierType mods;
		get_accel_key(window, out keyval, out mods);
		if(event.keyval == keyval &&
			(event.state & Gtk.accelerator_get_default_mod_mask())
			== (mods & Gtk.accelerator_get_default_mod_mask())) {
			/* We chain up to the toplevel key_press_event,
			 * which is listened by all the menubars within
			 * the applet*/
			Gtk.Widget toplevel = _menubar.get_toplevel();
			if(toplevel != null) 
				toplevel.key_press_event(event);
			return false;
		}
		return true;
	}
	/**
	 * return the accelerator key combination for invoking menu bars
	 * in GTK Settings. It is usually F10.
	 */
	private static void get_accel_key(Gnomenu.Window root_window, out uint keyval, out Gdk.ModifierType mods) {
		Gtk.Settings settings = Gtk.Settings.get_for_screen(root_window.window.get_screen());
		string accel = null;
		settings.get("gtk_menu_bar_accel", &accel, null);
		if(accel != null)
			Gtk.accelerator_parse(accel, out keyval, out mods);
	}
	private void update_menubar() {
		if(_menubar == null) return;
		if(_current_gnomenu_window != null) {
			string context = _current_gnomenu_window.menu_context;
			if(context != null) {
				Parser.parse(_menubar, context);
				_menubar.show();
				return;
			}
		}
		/* elseever */
		_menubar.hide();
	}
}
