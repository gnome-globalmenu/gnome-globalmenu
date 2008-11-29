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
		public Window.foreign(ulong native) {
			this.native= (ulong) native;
		}
		public Window(WindowType type) {
			type = type;
		}
		construct {
			set_events(get_events() 
			| Gdk.EventMask.PROPERTY_CHANGE_MASK
			);
			if(_native != 0) {
				window = gdk_window_foreign_new(_native);
				if(window != null) {
					window.set_events((Gdk.EventMask)get_events());
					set_flags(WidgetFlags.REALIZED);
					window.set_user_data(this);
					style.attach(window);
				}
			}
			disposed = false;
		}
		public string menu_context {
			get {
				_menu_context = get("_NET_GLOBALMENU_MENU_CONTEXT");
				return _menu_context;
			}
			set {
				_menu_context = value;
				set("_NET_GLOBALMENU_MENU_CONTEXT", value);
			}
		}
		public string? get(string property_name) {
			return get_by_atom(Gdk.Atom.intern(property_name, false));	
		}
		public void set(string property_name, string? value) {
			set_by_atom(Gdk.Atom.intern(property_name, false), value);	
		}
		public string? get_by_atom(Gdk.Atom atom) {
			string context;
			Gdk.Atom actual_type;
			int actual_format;
			int actual_length;
			gdk_property_get(window,
				atom,
				atom,
				0, (ulong) long.MAX, false, 
				out actual_type, 
				out actual_format, 
				out actual_length, 
				out context);
			return context;
		}
		public void set_by_atom(Gdk.Atom atom, string? value) {
			if(value != null) {
				gdk_property_change(window,
					atom, atom,
					8,
					Gdk.PropMode.REPLACE,
					value, 
					(int) value.size() + 1
				);
			} else {
				Gdk.property_delete(window, atom);
			}
		}
		private bool disposed;
		public signal void property_changed(string name);
		private ulong _native;
		private string _menu_context;
		private override void realize() {
			if(native != 0) {
				foreach(Widget child in get_children()) {
					child.realize();
				}
				return;
			}
			base.realize();
		}
		private override void map() {
			if(native != 0) {
				add_events(Gdk.EventMask.EXPOSURE_MASK);
				foreach(Widget child in get_children()) {
					child.map();
				}
				return;
			}
			base.map();
		}
		private override void unmap() {
			if(native != 0) {
				foreach(Widget child in get_children()) {
					child.unmap();
				}
				return;
			}
			base.unmap();
		}
		private override bool expose_event(Gdk.EventExpose event) {
			if(native != 0) {
				foreach(Widget child in get_children()) {
					propagate_expose(child, event);
				}
				return true;
			}
			base.expose_event(event);
			return false;
		}
		private override void unrealize() {
			if(native != 0) {
				foreach(Widget child in get_children()) {
					child.unrealize();
				}
				return;
			}
			base.unrealize();
		}
		private override bool property_notify_event(Gdk.EventProperty event) {
			property_changed(event.atom.name());
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
