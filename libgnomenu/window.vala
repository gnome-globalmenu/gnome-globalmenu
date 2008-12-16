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
	public class Window : Gtk.Window {
		/**
		 * If the window is not realized (this.window == null)
		 */
		public bool invalid {get {return window == null;}}
		public bool is_foreign {get; construct; }
		public ulong xid {get {return _xid;}}

		public Window (WindowType type) {
			this.type = type;
			is_foreign = false;
		}
		private Window.foreign() {
			is_foreign = true;
		}
		public static Window? new_from_native(ulong native) {
			Gdk.Window gdk_window;
			gdk_window = gdk_window_lookup(native);
			if(gdk_window == null ) {
				gdk_window = gdk_window_foreign_new(native);
			}
			Window rt = new_from_gdk_window(gdk_window);
			return rt;
		}
		public static Window? new_from_gdk_window(Gdk.Window window) {
			bool _is_foreign;
			_is_foreign = false;
			if(window != null) {
				if(
					window.get_window_type() == Gdk.WindowType.FOREIGN ||
					window.get_window_type() == Gdk.WindowType.ROOT
				)
				_is_foreign = true;
			}
			if(!_is_foreign) return null;

			Window rt = new Window.foreign();
			rt._xid = gdk_window_xid(window);
			rt.set_events(rt.get_events() 
			| Gdk.EventMask.PROPERTY_CHANGE_MASK
			);
			rt.window = window;
			message("create: %p ref_count= %u", window, window.ref_count);
			rt.window.set_events((Gdk.EventMask)rt.get_events());
			rt.set_flags(WidgetFlags.REALIZED);
			rt.window.set_user_data(rt);
			/* To avoid a warning, 
			 * perhaps it is problematic */
			rt.style.attach(rt.window);
			return rt;
		}
		construct {
			disposed = false;
		}
		/**
		 * the xml representation of the menu of the window
		 */
		public string? menu_context {
			get {
				_menu_context = get(NET_GLOBALMENU_MENU_CONTEXT);
				return _menu_context;
			}
			set {
				_menu_context = value;
				set(NET_GLOBALMENU_MENU_CONTEXT, value);
			}
		}
		/**
		 * emitted when a menu item is activated
		 */
		public void emit_menu_event (string path) {
			set(NET_GLOBALMENU_MENU_EVENT, path);
		}
		public string? get(string property_name) {
			return get_by_atom(Gdk.Atom.intern(property_name, false));	
		}
		public void set(string property_name, string? value) {
			set_by_atom(Gdk.Atom.intern(property_name, false), value);	
		}
		public string? get_by_atom(Gdk.Atom atom) {
			if(invalid) return null;
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
			return_if_fail(!invalid);
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

		private bool disposed;

		private ulong _xid;
		private string _menu_context;
		public override void realize() {
			if(is_foreign) return;
			base.realize();
		}
		public override void map() { 
			if(is_foreign) return; 
			base.map();
		} 
		public override void unmap() { 
			if(is_foreign) return;
			base.unmap();
		}
		public override bool map_event(Gdk.Event event) {
			/* Here we ignore the default Gtk.Window.map_event
			 * there is a workaround in Gtk.Window.map_event:
			 *
			 * if the widget is not mapped, the wrapped Gdk.Window
			 * is hiden.
			 *
			 * but for a foreign window, we never try to set the
			 * mapped state of the widget. The workaround
			 * will think there is a buggy wm, and 
			 * hide the foreign window. 
			 *
			 * We don't expect that to happen. If the default handler
			 * is not disabled for for foreign windows,
			 * every time the workspace is switched a lot of windows
			 * will disappear.
			 */
			if(is_foreign) return false;
			return base.map_event(event);
		}
		public override bool expose_event(Gdk.EventExpose event) { 
			if(is_foreign) return false;
			return base.expose_event(event);
		}

		public override void unrealize() {
			if(is_foreign) return;
			base.unrealize();
		}

		public override bool property_notify_event(Gdk.EventProperty event) {
			if(event.atom == Gdk.Atom.intern(NET_GLOBALMENU_MENU_EVENT, false)) {
				menu_event(get(NET_GLOBALMENU_MENU_EVENT));
			}
			if(event.atom == Gdk.Atom.intern(NET_GLOBALMENU_MENU_CONTEXT, false)) {
				menu_context_changed();
			}
			return false;
		}
		public override void dispose () {
			if(is_foreign) {
				if(!disposed) {
					disposed = true;
					window.set_user_data(null);
					/*Don't destroy it ever*/
					window = null;
					unset_flags(WidgetFlags.REALIZED);
				}
			}
			base.dispose();
		}
	}
}
