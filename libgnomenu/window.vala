using Gtk;
namespace Gnomenu {
	public class Window : Gtk.Window {
		public ulong native {
			get {
				return _native;
			}
			construct {
				_native = value;
			}
		}
		public Gdk.Window gdk_window {
			get;
			construct;
		}
		public bool invalid {get {return window == null;}}
		public Window.foreign(ulong native) {
			this.native = (ulong) native;
		}
		public Window.from_gdk_window(Gdk.Window window) {
			this.gdk_window = window;
		}
		public Window(WindowType type) {
			this.type = type;
		}
		construct {
			_foreign = false;
			set_events(get_events() 
			| Gdk.EventMask.PROPERTY_CHANGE_MASK
/*			| Gdk.EventMask.KEY_PRESS_MASK disable key_press_mask; 
 *			GDK has to use set_user_time on the event but 
 *			that function only works on non-foreign windows.*/
			);
			if(_native != 0) {
				window = gdk_window_foreign_new(_native);
				_foreign = true;
			}
			if(_gdk_window != null) {
				window = _gdk_window;
				_foreign = true;
			}
			if(!invalid) {
				window.set_events((Gdk.EventMask)get_events());
				set_flags(WidgetFlags.REALIZED);
				window.set_user_data(this);
				style.attach(window);
			}
			disposed = false;
		}
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
		public bool grab_key(uint keyval, Gdk.ModifierType state) {
			return Gnomenu.grab_key(window, keyval, state);
		}
		public bool ungrab_key(uint keyval, Gdk.ModifierType state) {
			return Gnomenu.ungrab_key(window, keyval, state);
		}
		private bool disposed;
		public signal void menu_context_changed();
		public signal void menu_event(string path);

		private ulong _native;
		private string _menu_context;
		private bool _foreign;
		private override void realize() {
			if(_foreign == true) {
				foreach(Widget child in get_children()) {
					child.realize();
				}
				return;
			}
			base.realize();
		}
		private override void map() {
			if(_foreign == true) {
				add_events(Gdk.EventMask.EXPOSURE_MASK);
				foreach(Widget child in get_children()) {
					child.map();
				}
				return;
			}
			base.map();
		}
		private override void unmap() {
			if(_foreign == true) {
				foreach(Widget child in get_children()) {
					child.unmap();
				}
				return;
			}
			base.unmap();
		}
		private override bool expose_event(Gdk.EventExpose event) {
			if(_foreign == true) {
				foreach(Widget child in get_children()) {
					propagate_expose(child, event);
				}
				return true;
			}
			base.expose_event(event);
			return false;
		}
		private override void unrealize() {
			if(_foreign == true) {
				foreach(Widget child in get_children()) {
					child.unrealize();
				}
				return;
			}
			base.unrealize();
		}
		private override bool property_notify_event(Gdk.EventProperty event) {
			if(event.atom == Gdk.Atom.intern(NET_GLOBALMENU_MENU_EVENT, false)) {
				menu_event(get(NET_GLOBALMENU_MENU_EVENT));
			}
			if(event.atom == Gdk.Atom.intern(NET_GLOBALMENU_MENU_CONTEXT, false)) {
				menu_context_changed();
			}
			return false;
		}
		private override void dispose () {
			if(!disposed) {
				disposed = true;
				if(native != 0) {
						window.set_user_data(null);
						window = null;
				}
			}
			base.dispose();
		}
	}
}
