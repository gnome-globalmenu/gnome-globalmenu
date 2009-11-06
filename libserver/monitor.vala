
internal class Gnomenu.Monitor: GLib.Object {
	private static Wnck.Screen? gdk_screen_to_wnck_screen (Gdk.Screen? screen) {
		if(screen != null)
			return Wnck.Screen.get(screen.get_number());
		return null;
	}
	private static Gdk.Screen? wnck_screen_to_gdk_screen (Wnck.Screen? screen) {
		var display = Gdk.Display.get_default();
		if(screen != null)
			return display.get_screen(screen.get_number());
		return null;
	}

	public void attach(Gdk.Screen gdk_screen) {
		detach_from_screen();
		_screen = gdk_screen_to_wnck_screen(gdk_screen);
		attach_to_screen();
	}

	private Gnomenu.Shell _managed_shell;
	public Gnomenu.Shell managed_shell {
		get { return _managed_shell; } 
		set { 
			if(_managed_shell != null) {
				_managed_shell.set_data("globalmenu-monitor", null);
			}
			_managed_shell = value;
			if(_managed_shell != null) {
				_managed_shell.set_data("globalmenu-monitor", this);
			}
			rebuild_managed_shell();
		}
	}
	private int _monitor_num = -1;
	public int monitor_num {
		get { return _monitor_num; } 
		set {
			if(_monitor_num != value) {
				_monitor_num = value;
				/*Because when the monitor num has
				 * changed, the old active window
				 * may become inadaquate*/
				active_window_moved();
			}
		}
	}
	public bool per_monitor_mode {get; set;}

	public signal void active_window_changed(Gnomenu.Window? prev_window);
	public signal void shell_rebuilt();
	public signal void active_window_lost_focus();
	public signal void active_window_received_focus();

	public Gnomenu.Window active_window {get; private set;}

	public bool has_pointer() {
		if(_monitor_num == -1) return true;
		int num = get_monitor_num_at_pointer();
		if(_monitor_num == num) return true;
		return false;
	}
	/* The window has focus, but is on a different monitor num
	 * thus ignored by this monitor,
	 * unless per_monitor_mode  is disabled.
	 * We need this variable to keep track of the movement
	 * of the focused window.once it is moved into this monitor
	 * we rebuild the menu shell.
	 * */
	private Gnomenu.Window _dummy_window = null;
	private Gnomenu.Window _desktop_window = null;

	public Monitor(Gdk.Screen? screen) {
		attach(screen);
	}

	public override void dispose() {
		if(!disposed) {
			disposed = true;
			detach_from_screen();
		}
	}
	private Wnck.Screen? _screen = null;
	private Wnck.Window? _wnck_desktop_window = null;
	private Wnck.Window? _wnck_active_window = null;
	private Wnck.Window? _wnck_dummy_window = null;

	private bool disposed = false;

	private bool _wnck_active_window_is_closed = false;
	private void on_window_closed(Wnck.Screen screen, Wnck.Window window) {
		if(_wnck_desktop_window == window) {
			update_desktop_window();
		}
		if(window == _wnck_active_window) {
			_wnck_active_window_is_closed = true;
			wnck_status_changed();
			_wnck_active_window_is_closed = false;
		}
	}

	private void on_window_opened(Wnck.Screen screen, Wnck.Window window) {
		if(window.get_window_type() == Wnck.WindowType.DESKTOP) {
			update_desktop_window();
		}
		/* FIXME: see if this condition is needed at all*/
		if(_wnck_active_window == null) {
			wnck_status_changed();
		}
	}

	private void on_active_window_changed (Wnck.Screen screen, Wnck.Window? previous_window) {
		Wnck.Window window = screen.get_active_window();
		/* This is to workaround a weird issue that wnck always 
		 * invokes two signals when the active window is changed,
		 * at least true for metacity:
		 *
		 * 1st for active_window = null,
		 * 2nd for the real active_window.
		 *
		 * */
		if(window == null) {
			/* two nulls means the desktop is focused
			 * one null means the previous_window is
			 * focused */
			if(previous_window == null) {
				wnck_status_changed();
			}
		} else {
			wnck_status_changed();
		}
	}

	private void update_desktop_window() {
		weak List<weak Wnck.Window> windows = _screen.get_windows();
		_wnck_desktop_window = null;
		foreach(weak Wnck.Window window in windows) {
			if(window.get_window_type() == Wnck.WindowType.DESKTOP) {
				_wnck_desktop_window = window;
			}
		}
		if(_wnck_desktop_window != null) {
			_desktop_window = Window.foreign_new(_wnck_desktop_window.get_xid());
		} else {
			_desktop_window = null;
		}
	}
	private bool is_window_on_my_monitor(Gnomenu.Window? win) {
		/* if not on per monitor mode,
		 * assume the window is always on my monitor.
		 * */
		if(per_monitor_mode == false) return true;
		int win_num = -1;
		if(win != null) win_num = win.get_monitor_num();
		if(win_num == -1) {
			debug("fallback to use pointer");
			win_num = get_monitor_num_at_pointer();
		}
		if(_per_monitor_mode && _monitor_num != -1 
		&& win_num != -1 
		&& win_num != _monitor_num) {
			return false;
		}
		return true;
	}
	private void wnck_status_changed() {
		Wnck.Window wnck_prev = _wnck_active_window;
		Wnck.Window wnck_new = _screen.get_active_window();

		if(wnck_new == null) {
			/* Try to use the desktop window if there is no current window
			 * AKA fallback to nautilus desktop menubar if there is no current window
			 * */
			wnck_new = _wnck_desktop_window;
		}

		if(wnck_prev == wnck_new) {
			active_window_received_focus();
			/* if the current_window is not changed, do nothing */
			return;
		}

		if(wnck_new != null) {
			switch(wnck_new.get_window_type()) {
				case Wnck.WindowType.NORMAL:
				case Wnck.WindowType.UTILITY:
				case Wnck.WindowType.DIALOG:
				case Wnck.WindowType.DESKTOP:
					break;
				default:
					wnck_new = _wnck_desktop_window;
					break;
			}
		}
		update_active_window(wnck_new);
	}
	private void update_active_window(Wnck.Window? from_wnck_window) {
		debug("%p, update_active_window called once", this);

		Gnomenu.Window @new = null;
		if(from_wnck_window != null) {
			/* emit the window changed signal */
			@new = Window.foreign_new(from_wnck_window.get_xid());
		} else {
			/* if there is not even a desktop window */
			@new = null;
		}

		if(!is_window_on_my_monitor(@new)) {
			/* new active window not on the same physical monitor */
			if(!@new.is_on_active_workspace()) {
				replace_active_window(_desktop_window);
			}
			replace_dummy_window(@new);
			if(_wnck_active_window_is_closed) {
				replace_active_window(_desktop_window);
			}
			debug("%p, not on my monitor", this);
			active_window_lost_focus();
		} else { /* @new is on my monitor */
			debug("%p, on my monitor", this);
			replace_dummy_window(null);
			replace_active_window(@new);
		}
	}

	private void replace_active_window(Gnomenu.Window? @new) {
		var prev = _active_window;
		_wnck_active_window = null;
		if(_active_window != null) {
			_active_window.menu_context_changed 
				-= rebuild_managed_shell;
			_active_window.monitor_num_changed 
				-= active_window_moved;
		}
		_active_window = @new;
		if(_active_window != null) {
			_wnck_active_window = _active_window.get_wnck_window();
			_active_window.menu_context_changed 
				+= rebuild_managed_shell;
			_active_window.monitor_num_changed 
				+= active_window_moved;
		}
		rebuild_managed_shell();
		active_window_changed(prev);
	}

	private void replace_dummy_window(Gnomenu.Window? @new) {
		_wnck_dummy_window = null;
		if(_dummy_window != null) {
			_dummy_window.monitor_num_changed 
				-= active_window_moved;
		}
		_dummy_window = @new;
		if(_dummy_window != null) {
			_wnck_dummy_window = _dummy_window.get_wnck_window();
			_dummy_window.monitor_num_changed 
				+= active_window_moved;
		}
	}

	private void active_window_moved() {
		if(_dummy_window != null) {
			debug("dummy window_moved to life");
			/* there is a dummy window around,
			 * so the current active window is actually already
			 * expired. replace it with dummy when the dummy window
			 * is moved back to this monitor */
			if(is_window_on_my_monitor(_dummy_window)) {
				var save = _dummy_window;
				replace_dummy_window(null);
				replace_active_window(save);
			}
		} else {
			debug("active window_moved to death");
			/* this is no dummy window around,
			 * so the active window is still in
			 * focus. if the focused active window
			 * is moved out from my monitor,
			 * swap dummy and active window.
			 */
			if(!is_window_on_my_monitor(_active_window)) {
				var save = _active_window;
				replace_active_window(_desktop_window);
				replace_dummy_window(save);
			}
		}
	}
	private int get_monitor_num_at_pointer() {
		if(_screen == null) return -1;
		var gdk_screen = wnck_screen_to_gdk_screen(_screen);
		Gdk.Display display = gdk_screen.get_display();
		int x, y;
		display.get_pointer(null, out x, out y, null);
		return gdk_screen.get_monitor_at_point(x, y);
	}

	public void rebuild_shell(Gnomenu.Shell shell) {
		shell.length = 0;
		if(_active_window == null) {
			return;
		}
		var context = _active_window.get_menu_context();
		if(context == null) {
			return;
		}
		try {
			Parser.parse(shell, context);
		} catch(GLib.Error e) {
			critical("%s", e.message);
		}
	}
	private void rebuild_managed_shell() {
		if(_managed_shell == null) return;
		rebuild_shell(_managed_shell);
		shell_rebuilt();
	}

	private void detach_from_screen() {
		if(_screen != null) {
			_screen.window_opened -= on_window_opened;
			_screen.window_closed -= on_window_closed;
			_screen.active_window_changed -= on_active_window_changed;
		}
		_wnck_desktop_window = null;
		_wnck_dummy_window = null;
		_wnck_active_window = null;
	}

	private void attach_to_screen() {
		if(_screen != null) {
			/* sync to wnck status, may invoke a whole bunch of
			 * signals, therefore we do it before connecting signals */
			_screen.force_update();
			_screen.window_closed += on_window_closed;
			_screen.window_opened += on_window_opened;
			_screen.active_window_changed += on_active_window_changed;

			update_desktop_window();
			wnck_status_changed();
		}
	}
}
