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
		construct {
			set_events(get_events() 
			| Gdk.EventMask.PROPERTY_CHANGE_MASK
			);
		}
		private ulong _native;

		private override void realize() {
			window = Gdk.Window.foreign_new((Gdk.NativeWindow)_native);
			message("%lu => %p", (ulong) _native, window);
			window.set_events((Gdk.EventMask)get_events());
			set_flags(WidgetFlags.REALIZED);
			window.set_user_data(this);
		}
		private override void map() { }
		private override void unmap() { }
		private override bool property_notify_event(Gdk.EventProperty event) {
			message("%s was changed", event.atom.name());
			return false;
		}
	}
}
