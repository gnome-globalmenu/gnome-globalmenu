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

	construct {
		update_transient();
		property_notify_event += property_notify_event_default_handler;
	}
	private Gdk.Window _window;
	public Gdk.Window window {
		get {
			return _window;
		} 
		construct set {
			if(_window != null) 
				_window.remove_filter(event_filter);
			_window = value;
			if(_window != null) 
				_window.add_filter(event_filter);
		}
	}
	public uint xid {
		get {
			if(_window != null)
				return Gdk.x11_drawable_get_xid(_window);
			return 0;
		}
	}

	private Gtk.Widget key_widget;
	private bool disposed = false;

	public Window (Gdk.Window window) {
		this.window = window;
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
	private Gdk.FilterReturn event_filter(Gdk.XEvent xevent, Gdk.Event gdk_ev) {
		if(disposed) {
			critical("event_filter invoked on a disposed window");
			return Gdk.FilterReturn.CONTINUE;
		}
		void * pointer = &xevent;
		Xlib.AnyEvent* event = (Xlib.AnyEvent*) pointer;
		Gdk.Event ev = gdk_ev.copy(); /* copy the gdk_event*/
		switch(event->type) {
			case Xlib.EventType.PropertyNotify:
				ev.type = Gdk.EventType.PROPERTY_NOTIFY;
				ev.property.atom = ((Xlib.PropertyEvent) event).atom.to_gdk();
				ev.property.time = ((Xlib.PropertyEvent) event).time;
				ev.property.state = ((Xlib.PropertyEvent) event).state;
				property_notify_event(ev.property.atom.name());
			break;
			case Xlib.EventType.KeyPress:
				if(key_widget != null &&
					key_widget.window != null) {
					/* Send a Fake key press event to the key widget
					 * if it is realized. */
					Gdk.Window gwin = key_widget.window;
					Xlib.Window xwin = Xlib.Window.from_gdk(gwin);
					weak Xlib.Display xd = Xlib.Display.from_gdk(Gdk.Display.get_default());
					event->window = xwin;
					Xlib.SendEvent(xd, xwin, false, 0, event);
				}
			break;
		
		}
		return Gdk.FilterReturn.CONTINUE;
	}
	private Gnomenu.Window get_target() {
		if(transient != null) {
			return transient.get_target();
		}
		return this;
	}
	public virtual signal void property_notify_event(string? prop) { }
	/* FIXME: Will be fixed in VALA 0.7.6*/
	void property_notify_event_default_handler (string? prop){
		if(prop == NET_GLOBALMENU_MENU_CONTEXT) {
			get_target().menu_context_changed();
		}
		if(prop == NET_GLOBALMENU_MENU_EVENT) {
			get_target().menu_event(get(NET_GLOBALMENU_MENU_EVENT));
		}
		if(prop == NET_GLOBALMENU_MENU_SELECT) {
			string value = get(NET_GLOBALMENU_MENU_SELECT);
			var arr = value.split("@");
			if(arr != null && arr.length >=1)
				get_target().menu_select(arr[0], arr[1]);
		}
		if(prop == NET_GLOBALMENU_MENU_DESELECT) {
			string value = get(NET_GLOBALMENU_MENU_DESELECT);
			get_target().menu_deselect(value);
		}
		if(prop == WM_TRANSIENT_FOR) {
			update_transient();
		}
	}

	private void update_transient() {
		var wnck_window = Wnck.Window.get(xid);
		Gnomenu.Window new_transient = null;
		Gnomenu.Window old = transient;
		transient_changed(old);
		if(wnck_window != null) {
			var wnck_transient = wnck_window.get_transient();
			if(wnck_transient != null) {
				var gnomenu_window = foreign_new(wnck_transient.get_xid());
				new_transient = gnomenu_window;
				debug("transient-for = '%s'", new_transient.get("WM_CLASS"));
			}
		}
		transient = new_transient;
		transient_changed(old);
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
		return get_target().get(NET_GLOBALMENU_MENU_CONTEXT);
		
	}
	public void set_menu_context(string? value) {
		get_target().set(NET_GLOBALMENU_MENU_CONTEXT, value);
	}
	/**
	 * emit the remote event indicating  menu item is activated
	 */
	public void emit_menu_event (string path) {
		get_target().set(NET_GLOBALMENU_MENU_EVENT, path);
	}

	/**
	 * emit a remote event indicating a menu item is selected,
	 * pos is a hint for the position to popup the submenu(if there is one)
	 */
	public void emit_menu_select(string path, string? pos) {
		if(pos != null)
			get_target().set(NET_GLOBALMENU_MENU_SELECT, path + "@" + pos);
		else
			get_target().set(NET_GLOBALMENU_MENU_SELECT, path);
	}

	/**
	 * emit a remote event indicating a menu item is selected,
	 */
	public void emit_menu_deselect(string path) {
		get_target().set(NET_GLOBALMENU_MENU_DESELECT, path);
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
}
