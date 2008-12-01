using Gtk;
using GtkAQD;

namespace GnomenuGtk {

	[CCode (cname = "gdk_window_get_desktop_hint")]
	protected extern bool gdk_window_get_is_desktop (Gdk.Window window);
	[CCode (cname = "gtk_widget_get_toplevel_window")]
	protected extern weak Gtk.Window? gtk_widget_get_toplevel_window (Gtk.Widget widget);
	[CCode (cname = "gdk_window_set_menu_context")]
	protected extern void gdk_window_set_menu_context (Gdk.Window window, string context);
	[CCode (cname = "gdk_window_get_menu_event")]
	protected extern string gdk_window_get_menu_event (Gdk.Window window);

	protected void add_emission_hooks() {
		uint signal_id = Signal.lookup("changed", typeof(Gtk.MenuBar));
		Signal.add_emission_hook (signal_id, 0, changed_eh, null);
		uint signal_id_hc = Signal.lookup("hierarchy-changed", typeof(Gtk.Widget));
		Signal.add_emission_hook (signal_id_hc, 0, hierachy_changed_eh, null);
	}

	private bool changed_eh (SignalInvocationHint ihint, 
			[CCode (array_length_pos = 1.9) ]
			Value[] param_values
	) {
		Gtk.MenuBar self = param_values[0].get_object() as Gtk.MenuBar;
		if(self != null) {
			if(ihint.run_type != SignalFlags.RUN_FIRST) return true;
			Gtk.Window toplevel = self.get_ancestor(typeof(Gtk.Window)) as Gtk.Window;
			if(toplevel != null && (0 != (toplevel.get_flags() & WidgetFlags.REALIZED))) {
				gdk_window_set_menu_context(toplevel.window, 
						"""<menu>
							<item id="Hello"/>
							<item id="FIXME" font="Serif Bold 20"/>
							<item id="World"/>
						</menu>"""
						);
			}
		} 
		return true;
	}

	private bool hierachy_changed_eh (SignalInvocationHint ihint, 
			[CCode (array_length_pos = 1.9) ]
			Value[] param_values
	) {

		Gtk.MenuBar self = param_values[0].get_object() as Gtk.MenuBar;
		Gtk.Widget old_toplevel = param_values[1].get_object() as Gtk.Widget;
		
		Gtk.Window old_toplevel_window = gtk_widget_get_toplevel_window(old_toplevel);
		Gtk.Window toplevel_window = gtk_widget_get_toplevel_window(self);

		if(old_toplevel_window != null) {
			old_toplevel_window.property_notify_event -= window_property_notify_event;
			old_toplevel_window.set_data("__menubar__", null);
		}
		if(toplevel_window != null) {
			toplevel_window.set_data_full("__menubar__", self.ref(), g_object_unref);
			toplevel_window.add_events(Gdk.EventMask.PROPERTY_CHANGE_MASK);
			toplevel_window.property_notify_event += window_property_notify_event;
	  	} 
		return true;
	}

	private bool window_property_notify_event (Gtk.Window window, Gdk.EventProperty event) {
		if(event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_EVENT", false)) {
			message("%s", gdk_window_get_menu_event(window.window));
		}
		return false;
	}

}
