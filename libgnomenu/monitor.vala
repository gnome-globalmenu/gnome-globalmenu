
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
	public int monitor_num {get; set;}
	public bool per_monitor_mode {get; set;}

	public abstract signal void active_window_changed(Gnomenu.Window? prev_window);

	public Gnomenu.Window active_window {get; private set;}

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
	private Wnck.Window? _desktop = null;
	private Wnck.Window? _wnck_active_window = null;

	private bool disposed = false;

	private void on_window_closed(Wnck.Screen screen, Wnck.Window window) {
		if(_desktop == window) {
			_desktop = null;
		}
		if(window == _wnck_active_window) {
			update_active_window();
		}
	}

	private void on_window_opened(Wnck.Screen screen, Wnck.Window window) {
		if(window.get_window_type() == Wnck.WindowType.DESKTOP)
			_desktop = window;
		/* FIXME: see if this condition is needed at all*/
		if(_wnck_active_window == null)
			update_active_window();
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
			if(previous_window == null)
				update_active_window();
		} else {
			update_active_window();
		}
	}

	private void update_desktop_window() {
		weak List<weak Wnck.Window> windows = _screen.get_windows();
		_desktop = null;
		foreach(weak Wnck.Window window in windows) {
			if(window.get_window_type() == Wnck.WindowType.DESKTOP) {
				_desktop = window;
			}
		}
	}
	private void update_active_window() {
		Wnck.Window wnck_prev = _wnck_active_window;
		Wnck.Window wnck_new = _screen.get_active_window();

		if(wnck_new == null) {
			/* Try to use the desktop window if there is no current window
			 * AKA fallback to nautilus desktop menubar if there is no current window
			 * */
			_wnck_active_window = _desktop;
		} else {
			_wnck_active_window = wnck_new;
		}

		if(wnck_prev == _wnck_active_window) {
			/* if the current_window is not changed, do nothing */
			return;
		}

		if(_wnck_active_window != null) {
			switch(_wnck_active_window.get_window_type()) {
				case Wnck.WindowType.NORMAL:
				case Wnck.WindowType.UTILITY:
				case Wnck.WindowType.DIALOG:
				case Wnck.WindowType.DESKTOP:
					break;
				default:
					_wnck_active_window = _desktop;
					break;
			}
		}

		var prev = _active_window;
		if(prev != null) {
			prev.menu_context_changed -= rebuild_managed_shell;
		}

		Gnomenu.Window @new = null;
		if(_wnck_active_window != null) {
			/* emit the window changed signal */
			@new = Window.foreign_new(_wnck_active_window.get_xid());
		} else {
			/* if there is not even a desktop window */
			@new = null;
		}

		int win_num = -1;
		if(@new != null) win_num = @new.get_monitor_num();
		if(win_num == -1) win_num = get_monitor_num_at_pointer();
		if(_per_monitor_mode && _monitor_num != -1 
		&& win_num != -1 
		&& win_num != _monitor_num) {
			/* new active window not on the same physical monitor */
			if(_active_window != null
			&& !_active_window.is_on_active_workspace()) {
				/* if the old active_window is not even
				 * on the current workspace detach from
				 * the window (so that active window = null)
				 * */
				_active_window.menu_context_changed 
				    -= rebuild_managed_shell;
				_active_window = null;
			}
		} else {
			_active_window = @new;
			if(_active_window != null) {
				_active_window.menu_context_changed 
				    += rebuild_managed_shell;
			}
		}
		rebuild_managed_shell();
		active_window_changed(prev);
	}

	private int get_monitor_num_at_pointer() {
		if(_screen == null) return -1;
		var gdk_screen = wnck_screen_to_gdk_screen(_screen);
		Gdk.Display display = gdk_screen.get_display();
		int x, y;
		display.get_pointer(null, out x, out y, null);
		return gdk_screen.get_monitor_at_point(x, y);
	}

	private void rebuild_managed_shell() {
		if(_managed_shell == null) return;
		_managed_shell.length = 0;
		if(_active_window == null) return;
		var context = _active_window.get_menu_context();
		if(context == null) return;
		try {
			Parser.parse(_managed_shell, context);
		} catch(GLib.Error e) {
			warning("%s", e.message);
		}
	}

	private void detach_from_screen() {
		if(_screen != null) {
			_screen.window_opened -= on_window_opened;
			_screen.window_closed -= on_window_closed;
			_screen.active_window_changed -= on_active_window_changed;
		}
		_desktop = null;
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
			update_active_window();
		}
	}
}
