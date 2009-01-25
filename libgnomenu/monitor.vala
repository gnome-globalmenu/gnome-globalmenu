using Gnomenu;

namespace Gnomenu {
public class Monitor: GLib.Object {
	private Wnck.Screen? gdk_to_wnck_screen (Gdk.Screen? screen) {
		if(screen != null)
			return Wnck.Screen.get(screen.get_number());
		return null;
	}
	private Gdk.Screen _screen;
	public Gdk.Screen screen {
		get {
			return _screen;	
		}
		set {
			Wnck.Screen new_screen = gdk_to_wnck_screen(value);
			if(new_screen == null) {
				new_screen = Wnck.Screen.get_default();
			}
			if(new_screen == _wnck_screen) return;
			_screen = value;

			if(_wnck_screen != null) {
				detach_from_screen(_wnck_screen);
			}


			_wnck_screen = new_screen;
			attach_to_screen(_wnck_screen);

		}
	}

	public ulong current_xid {
		get {
			if(_current_window != null) {
				return _current_window.get_xid();
			}
			return 0;
		}	
	}

	public virtual signal void window_changed(ulong old_xid);
	
	public Monitor() {
	
	}

	construct {
		screen = Gdk.Screen.get_default();
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
		if(window == _current_window) {
			window.set_data("window-closed", window);
			update_current_window();
		}
	}
	private void on_window_opened(Wnck.Screen screen, Wnck.Window window) {
		if(window.get_window_type() == Wnck.WindowType.DESKTOP)
			_desktop = window;
		if(_current_window == null)
			update_current_window();
	}
	private void on_active_window_changed (Wnck.Screen screen, Wnck.Window previous_window) {
		update_current_window();
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
		if(_current_window != null) {
			weak Wnck.Window transient_for = _current_window.get_transient();
			if(transient_for != null) 
				_current_window = transient_for;
			switch(_current_window.get_window_type()) {
				case Wnck.WindowType.NORMAL:
				case Wnck.WindowType.DESKTOP:
					break;
				default:
					//if(old.get_data("window-closed") != null) {
						_current_window = _desktop;
						old.set_data("window-closed", null);
					//}
					break;
			}
		}
		if(old == _current_window) return;
		if(old != null) {
			window_changed(old.get_xid());
		} else {
			window_changed(0);
		}
	}
	private void detach_from_screen(Wnck.Screen screen) {
		_wnck_screen.window_opened -= on_window_opened;
		_wnck_screen.window_closed -= on_window_closed;
		_wnck_screen.active_window_changed -= on_active_window_changed;
		_desktop = null;
	}
	private void attach_to_screen(Wnck.Screen screen) {
		_wnck_screen.window_closed += on_window_closed;
		_wnck_screen.window_opened += on_window_opened;
		_wnck_screen.active_window_changed += on_active_window_changed;
		Wnck.Window new_desktop = find_desktop(_wnck_screen);
		_desktop = new_desktop;

		_wnck_screen.active_window_changed (null);
	}
}
}
