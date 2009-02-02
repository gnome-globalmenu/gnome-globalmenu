using Gtk;
namespace Gnomenu {
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
	public class Window : GLib.Object {
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
		private Gdk.Window _window;
		private Gtk.Widget key_widget;
		private bool disposed = false;
		public Window (Gdk.Window window) {
			this.window = window;
		}
		public static Window? foreign_new (ulong xid) {
			Gdk.Window gdk_window;
			gdk_window = gdk_window_lookup(xid);
			if(gdk_window == null ) {
				gdk_window = gdk_window_foreign_new(xid);
			}
			return new Window(gdk_window);
		}
		construct {
			property_notify_event += (t, prop) => {
				if(prop == NET_GLOBALMENU_MENU_CONTEXT) {
					menu_context_changed();
				}
				if(prop == NET_GLOBALMENU_MENU_EVENT) {
					menu_event(get(NET_GLOBALMENU_MENU_EVENT));
				}
			};
		}
		public void set_key_widget(Gtk.Widget widget) {
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
			weak Xlib.AnyEvent event = (Xlib.AnyEvent) xevent;
			Gdk.Event ev = gdk_ev; /* copy the gdk_event*/
			switch(event.type) {
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
						event.window = xwin;
						Xlib.SendEvent(xd, xwin, false, 0, event);
					}
				break;
			
			}
			return Gdk.FilterReturn.CONTINUE;
		}
		public signal void property_notify_event(string name);

		public string? get(string property_name) {
			return get_by_atom(Gdk.Atom.intern(property_name, false));	
		}
		public void set(string property_name, string? value) {
			set_by_atom(Gdk.Atom.intern(property_name, false), value);	
		}
		public string? get_by_atom(Gdk.Atom atom) {
			string context;
			Gdk.Atom actual_type;
			Gdk.Atom type = Gdk.Atom.intern("STRING", false);
			int actual_format;
			int actual_length;
			gdk_property_get(window,
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
				gdk_property_change(window,
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
		/**
		 * the xml representation of the menu of the window
		 */
		private string _menu_context;
		public string? menu_context {
			get {
				_menu_context = get(NET_GLOBALMENU_MENU_CONTEXT);
				return _menu_context;
			}
			set {
				set(NET_GLOBALMENU_MENU_CONTEXT, value);
			}
		}
		/**
		 * emitted when a menu item is activated
		 */
		public void emit_menu_event (string path) {
			set(NET_GLOBALMENU_MENU_EVENT, path);
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
		 */
		public signal void menu_context_changed();
		/**
		 * emitted when the a menu item is activated.
		 * (Not useful in GlobalMenu.PanelApplet).
		 */
		public signal void menu_event(string path);
		
	}
}
