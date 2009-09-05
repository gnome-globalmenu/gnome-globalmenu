
internal class Gnomenu.Monitor: GLib.Object {
	private static Wnck.Screen? gdk_screen_to_wnck_screen (Gdk.Screen? screen) {
		if(screen != null)
			return Wnck.Screen.get(screen.get_number());
		return null;
	}

	public void attach(Gdk.Screen gdk_screen) {
		detach_from_screen();
		_screen = gdk_screen_to_wnck_screen(gdk_screen);
		attach_to_screen();
	}

	public abstract signal void active_window_changed(Gnomenu.Window? prev_window);

	public Gnomenu.Window _active_window = null;
	public Gnomenu.Window active_window {
		get {
			return _active_window;
		}
		private set {
			Gnomenu.Window old = _active_window;
			_active_window = value;
			active_window_changed(old);
		}
	}

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
	private Wnck.Window? _current_window = null;

	private bool disposed = false;

	private void on_window_closed(Wnck.Screen screen, Wnck.Window window) {
		if(_desktop == window) {
			_desktop = null;
		}
		if(window == _current_window) {
			update_current_window();
		}
	}

	private void on_window_opened(Wnck.Screen screen, Wnck.Window window) {
		if(window.get_window_type() == Wnck.WindowType.DESKTOP)
			_desktop = window;
		if(_current_window == null)
			update_current_window();
	}

	private void on_active_window_changed (Wnck.Screen screen, Wnck.Window? previous_window) {
		update_current_window();
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
	private void update_current_window() {
		Wnck.Window old = _current_window;
		_current_window = _screen.get_active_window();

		if(_current_window == null) {
			/* Try to use the desktop window if there is no current window
			 * AKA fallback to nautilus desktop menubar if there is no current window
			 * */
			_current_window = _desktop;
		}

		if(_current_window != null) {
			/* Look for the transient_for(or Parent) window */
			switch(_current_window.get_window_type()) {
				case Wnck.WindowType.NORMAL:
				case Wnck.WindowType.DESKTOP:
				case Wnck.WindowType.UTILITY:
				case Wnck.WindowType.DIALOG:
					break;
				default:
					_current_window = _desktop;
					break;
			}
		}
		if(old == _current_window) {
			/* if the current_window is not changed, do nothing */
			return;
		}
		/* emit the window changed signal */
		active_window = Window.foreign_new(_current_window.get_xid());
	}
	private void detach_from_screen() {
		if(_screen != null) {
			_screen.window_opened -= on_window_opened;
			_screen.window_closed -= on_window_closed;
			_screen.active_window_changed -= on_active_window_changed;
		}
		_desktop = null;
		_current_window = null;
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
			update_current_window();
		}
	}
}
