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
		public Window(ulong native) {
			this.native= (ulong) native;
		}
		public string menu_context {
			get {
				_menu_context = Gnomenu.get_menu_context(window);
				return _menu_context;
			}
			set {
				_menu_context = value;
				Gnomenu.set_menu_context(window, value);
			}
		}
		construct {
			set_events(get_events() 
			| Gdk.EventMask.PROPERTY_CHANGE_MASK
			);
		}
		public signal void context_changed();
		private ulong _native;
		private string _menu_context;
		private override void realize() {
			window = Gdk.Window.foreign_new((Gdk.NativeWindow)_native);
			window.set_events((Gdk.EventMask)get_events());
			set_flags(WidgetFlags.REALIZED);
			window.set_user_data(this);
		}
		private override void map() { }
		private override void unmap() { }
		private override bool property_notify_event(Gdk.EventProperty event) {
			if(event.atom == 
			Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false)) {
				context_changed();
			}
			return false;
		}
	}
	public string? get_menu_context(Gdk.Window window) {
		string context;
		Gdk.Atom actual_type;
		int actual_format;
		int actual_length;
		gdk_property_get(window,
			Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false),
			Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false),
			0, (ulong) long.MAX, false, 
			out actual_type, 
			out actual_format, 
			out actual_length, 
			out context);
		return context;
	}
	public void set_menu_context(Gdk.Window window, string? context) {
		if(context != null) {
			gdk_property_change(window,
				Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false),
				Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false),
				8,
				Gdk.PropMode.REPLACE,
				context, 
				(int) context.size() + 1
				);
		} else {
			Gdk.property_delete(window,
				Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false));
		}
	}
}
