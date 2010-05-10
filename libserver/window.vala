/**
 * The Window widget extends Gtk.Window widget.
 *
 * The Window widget is capable of wrapping a Gdk.Window
 * or a is_foreign native window.
 *
 * It adds methods to bind any string as a property of
 * the underlining Gdk.Window.
 *
 * It emits property-notify-event signal when any properties of the underlineing Gdk.Window
 * has changed.
 *
 * As convenient wrappers,
 * it also addes the menu-context property and menu-event signal.
 *
 */
public class Gnomenu.Window : GLib.Object {
	private static const string WM_TRANSIENT_FOR = "WM_TRANSIENT_FOR";

	internal Wnck.Window? get_wnck_window() {
		return Wnck.Window.get(get_xid());
	}
	public bool is_on_active_workspace() {
		var wnck_window = get_wnck_window();
		/*FIXME: is it ok to return true 
		 * if the wnck window is destroyed?*/
		if(wnck_window == null) return true;
		var screen = wnck_window.get_screen();
		var workspace = screen.get_active_workspace();
		if(workspace == null) return true;
		return wnck_window.is_on_workspace(workspace);
	}

	private int _monitor_num = -1;
	public int get_monitor_num() {
		return _monitor_num;
	}
	public signal void monitor_num_changed(int prev_num);

	private Gdk.Window _window;
	public Gdk.Window window {
		get {
			return _window;
		} 
		set {
			if(_window != null) 
				_window.remove_filter(this.event_filter);
			_window = value;
			if(_window != null) {
				recompute_transient();
				_window.add_filter(this.event_filter);
			}
		}
	}
	public uint get_xid() {
		if(_window != null)
			return (uint) Gdk.x11_drawable_get_xid(_window);
		error("getting xid before _window has been initialized");
		return 0;
	}

	private Gtk.Widget key_widget;
	private bool disposed = false;

	public Window (Gdk.Window window) {
		this.window = window;
		recompute_monitor_num();
	}
	public static Window? foreign_new (ulong xid) {
		Gdk.Window gdk_window;
		gdk_window = Gdk.Window.lookup((Gdk.NativeWindow)xid);
		if(gdk_window == null ) {
			gdk_window = Gdk.Window.foreign_new((Gdk.NativeWindow)xid);
		}
		if(gdk_window == null) return null;
		return new Window(gdk_window);
	}

	/*must be a toplevel */
	public void set_key_widget(Gtk.Widget? widget) {
		key_widget = widget;
	}
	public override void dispose() {
		if(!disposed) {
			disposed = true;
			window = null;
		}
	}
	[CCode (instance_pos = -1)]
	private Gdk.FilterReturn event_filter(Gdk.XEvent xevent, Gdk.Event event) {
		void* pointer = &xevent;
		X.Event * e = (X.Event*)pointer;
		return real_event_filter(e, event);
	}
	[CCode (cname="XSendEvent")]
	internal extern static X.Status XSendEvent(X.Display display, 
			X.Window target, bool flag, long mask, X.Event * event);

	[CCode (instance_pos = -1)]
	private Gdk.FilterReturn real_event_filter(X.Event* xevent, Gdk.Event event) {
		if(disposed) {
			critical("event_filter invoked on a disposed window");
			return Gdk.FilterReturn.CONTINUE;
		}
		switch(xevent->type) {
			case X.EventType.PropertyNotify:
				string name = Gdk.x11_xatom_to_atom(xevent->xproperty.atom).name();
				property_notify_event(name);
			break;
			case X.EventType.KeyPress:
				if(key_widget == null || key_widget.window == null) break;
				/* If there is a key listener widget, redirect the event to the widget's window */
				X.Window new_target= (X.Window) Gdk.x11_drawable_get_xid(key_widget.window);
				unowned X.Display display = (X.Display) Gdk.x11_drawable_get_xdisplay(key_widget.window);
				xevent->xany.window = new_target;
				XSendEvent(display, new_target, false, 0, xevent);
			break;
			case X.EventType.ConfigureNotify:
				recompute_monitor_num();
			break;
		}
		return Gdk.FilterReturn.CONTINUE;
	}
	private Gnomenu.Window rewire_target {
		get {
			if(transient != null)
				return transient.rewire_target;
			else return this;
		}
	}

	public virtual signal void property_notify_event(string prop) {
		if(prop == WM_TRANSIENT_FOR) {
			debug("transient property changed");
			recompute_transient();
			menu_context_changed();
		} else {
			rewire_globalmenu_events(prop);
		}
	}

	private void rewire_globalmenu_events(string prop) {
		if(prop == NET_GLOBALMENU_MENU_CONTEXT) {
			debug("window (%p) prop menu context is reported changed", this);
			rewire_target.menu_context_changed();
		} else
		if(prop == NET_GLOBALMENU_MENU_EVENT) {
			rewire_target.menu_event(get(NET_GLOBALMENU_MENU_EVENT));
		} else
		if(prop == NET_GLOBALMENU_MENU_SELECT) {
			string val = this.get(NET_GLOBALMENU_MENU_SELECT);
			var arr = val.split("@");
			if(arr != null && arr.length >=1)
				rewire_target.menu_select(arr[0], arr[1]);
		} else
		if(prop == NET_GLOBALMENU_MENU_DESELECT) {
			string val = this.get(NET_GLOBALMENU_MENU_DESELECT);
			rewire_target.menu_deselect(val);
		}
		
	}
	private void recompute_monitor_num() {
		Gdk.Screen screen = _window.get_screen();
		int old_num = _monitor_num;
		/* for desktop we make it universal */
		if(_window.get_type_hint() == Gdk.WindowTypeHint.DESKTOP) {
			_monitor_num = -1;
		} else {
			_monitor_num = screen.get_monitor_at_window(_window);
		}
		if(old_num != _monitor_num) {
			monitor_num_changed(old_num);
		}
	}

	private void recompute_transient() {
		if( _window.get_window_type() == Gdk.WindowType.ROOT) {
			/* WNCK doesn't wrap the root window, 
			 * and the root window doesn't have a transient-for
			 * */
			return;
		}
		var wnck_window = get_wnck_window();
		Gnomenu.Window old = transient;

		if(wnck_window == null) {
			error("xwindow %u has been destroyed", get_xid());
			return;
		}
		var wnck_transient = wnck_window.get_transient();
		if(wnck_transient == null) {
			transient = null;
		} else {
			ulong new_xid = wnck_transient.get_xid();
			if(old == null || new_xid != old.get_xid()) {
				transient = foreign_new(new_xid);
				debug("transient-for changed to = '%s'", transient.get("WM_CLASS"));
			}
		}
	}

	public new string? get(string property_name) {
		return get_by_atom(Gdk.Atom.intern(property_name, false));	
	}
	public new void set(string property_name, string? value) {
		set_by_atom(Gdk.Atom.intern(property_name, false), value);	
	}

	public string? get_by_atom(Gdk.Atom atom) {
		string context = null;
		Gdk.Atom actual_type;
		Gdk.Atom type = Gdk.Atom.intern("STRING", false);
		int actual_format;
		int actual_length;
		Gdk.property_get(window,
			atom,
			type,
			0, (ulong) long.MAX, false, 
			out actual_type, 
			out actual_format, 
			out actual_length, 
			out context);
		return context;
	}

	public void set_by_atom(Gdk.Atom atom, string? value) {
		if(value != null) {
			Gdk.Atom type = Gdk.Atom.intern("STRING", false);
			Gdk.property_change(window,
				atom, type,
				8,
				Gdk.PropMode.REPLACE,
				value, 
				(int) value.size() + 1
			);
		} else {
			Gdk.property_delete(window, atom);
		}
	}

	public Gnomenu.Window transient {
		get;
		set;
	}
	/**
	 * get the xml representation of the menu of the window
	 */
	public string? get_menu_context() {
		return rewire_target.get(NET_GLOBALMENU_MENU_CONTEXT);
		
	}
	public void set_menu_context(string? value) {
		rewire_target.set(NET_GLOBALMENU_MENU_CONTEXT, value);
	}
	/**
	 * emit the remote event indicating  menu item is activated
	 */
	public void emit_menu_event (string path) {
		rewire_target.set(NET_GLOBALMENU_MENU_EVENT, path);
	}

	/**
	 * emit a remote event indicating a menu item is selected,
	 * pos is a hint for the position to popup the submenu(if there is one)
	 */
	public void emit_menu_select(string path, string? pos) {
		if(pos != null)
			rewire_target.set(NET_GLOBALMENU_MENU_SELECT, path + "@" + pos);
		else
			rewire_target.set(NET_GLOBALMENU_MENU_SELECT, path);
	}

	/**
	 * emit a remote event indicating a menu item is selected,
	 */
	public void emit_menu_deselect(string path) {
		rewire_target.set(NET_GLOBALMENU_MENU_DESELECT, path);
	}

	/**
	 * globally grab a key to this window.
	 *
	 * return false if failed.
	 */
	public bool grab_key(uint keyval, Gdk.ModifierType state) {
		return Gnomenu.grab_key(window, keyval, state);
	}
	/**
	 * globally ungrab a grabbed a key to this window
	 * return false if failed.
	 */
	public bool ungrab_key(uint keyval, Gdk.ModifierType state) {
		return Gnomenu.ungrab_key(window, keyval, state);
	}
	/**
	 * emitted when the menu context has changed.
	 * or the menu context of the transient_for window has changed;
	 */
	public signal void menu_context_changed();
	/**
	 * emitted when the a menu item is activated.
	 * or the menu context of the transient_for window has changed;
	 * (Not useful in GlobalMenu.PanelApplet).
	 */
	public signal void menu_event(string path);
	/**
	 * emitted when the a menu item is selected
	 * or the menu context of the transient_for window has changed;
	 * (Not useful in GlobalMenu.PanelApplet).
	 */
	public signal void menu_select(string path, string? pos);
	/**
	 * emitted when the a menu item is deselected.
	 * or the menu context of the transient_for window has changed;
	 * (Not useful in GlobalMenu.PanelApplet).
	 */
	public signal void menu_deselect(string path);
	
	public signal void transient_changed(Gnomenu.Window? prev_transient);
}
