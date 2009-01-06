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

			_wnck_screen = new_screen;
			_wnck_screen.window_closed += on_window_closed;
			_wnck_screen.window_opened += on_window_opened;
			_wnck_screen.active_window_changed += on_active_window_changed;
			_wnck_screen.active_window_changed (null);
			_desktop = find_desktop(_wnck_screen);
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
		}
	}
	private Wnck.Screen? _wnck_screen;
	private Wnck.Window _desktop;
	private Wnck.Window _current_window;

	private bool disposed;
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
		if(old != _current_window);
		window_changed(old);
	}
	private void detach_from_screen(Wnck.Screen screen) {
		_wnck_screen.window_opened -= on_window_opened;
		_wnck_screen.window_closed -= on_window_closed;
		_wnck_screen.active_window_changed -= on_active_window_changed;
	}
}
