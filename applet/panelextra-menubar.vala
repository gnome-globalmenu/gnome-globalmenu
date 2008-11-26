using GLib;
using Gtk;
using GMarkup;

namespace PanelExtra {
	public class MenuBar : Gtk.MenuBar {
		private Gdk.EventExpose __tmp__event;
		private void expose_child(Gtk.Widget widget) {
			this.propagate_expose(widget, __tmp__event);
		}
		
		public MenuBar() { }
		construct {
			this.expose_event += (widget, event)=> {
				if(0 != (widget.get_flags() & (Gtk.WidgetFlags.MAPPED | Gtk.WidgetFlags.VISIBLE))) {
					Gtk.paint_flat_box(widget.style,
							widget.window, (Gtk.StateType) widget.state,
							Gtk.ShadowType.NONE,
							event.area,
							widget, null, 0, 0, -1, -1);

					__tmp__event = event;
					(widget as GtkCompat.Container).forall_children (expose_child);
				}
				return true;
			};
		}
	}
}
