using Gtk;

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
		uint signal_id = Signal.lookup("changed", typeof(MenuBar));
		Signal.add_emission_hook (signal_id, 0, changed_eh, null);
		uint signal_id_hc = Signal.lookup("hierarchy-changed", typeof(Gtk.Widget));
		Signal.add_emission_hook (signal_id_hc, 0, hierachy_changed_eh, null);
	}

	private bool changed_eh (SignalInvocationHint ihint, 
			[CCode (array_length_pos = 1.9) ]
			Value[] param_values
	) {
		MenuBar self = param_values[0].get_object() as MenuBar;
		if(self != null) {
			if(ihint.run_type != SignalFlags.RUN_FIRST) return true;
			Gtk.Window toplevel = self.get_ancestor(typeof(Gtk.Window)) as Gtk.Window;
			if(toplevel != null && (0 != (toplevel.get_flags() & WidgetFlags.REALIZED))) {
				gdk_window_set_menu_context(toplevel.window, 
						Serializer.to_string(self)
						);
			}
		} 
		return true;
	}

	private bool hierachy_changed_eh (SignalInvocationHint ihint, 
			[CCode (array_length_pos = 1.9) ]
			Value[] param_values
	) {

		MenuBar self = param_values[0].get_object() as MenuBar;
		if(self == null) return true;
		Gtk.Widget old_toplevel = param_values[1].get_object() as Gtk.Widget;
		
		Gtk.Window old_toplevel_window = gtk_widget_get_toplevel_window(old_toplevel);
		Gtk.Window toplevel_window = gtk_widget_get_toplevel_window(self);

		if(old_toplevel_window != null) {
			old_toplevel_window.property_notify_event -= window_property_notify_event;
			old_toplevel_window.set_data("__menubar__", null);
			uint source_id = (uint) old_toplevel_window.get_data("__keep_alive__");
			Source.remove(source_id);
		}
		if(toplevel_window != null) {
			toplevel_window.set_data_full("__menubar__", self.ref(), g_object_unref);
			toplevel_window.add_events(Gdk.EventMask.PROPERTY_CHANGE_MASK);
			toplevel_window.property_notify_event += window_property_notify_event;
			uint source_id = g_timeout_add_full(Priority.DEFAULT, 500,
					(data) => {
						Gtk.Window window = data as Window;
						window.add_events(Gdk.EventMask.PROPERTY_CHANGE_MASK);
						return true;
					}, toplevel_window.ref(), g_object_unref);
			toplevel_window.set_data("__keep_alive__", (void*) source_id);
	  	} 
		return true;
	}

	private bool window_property_notify_event (Window window, Gdk.EventProperty event) {
		if(event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_EVENT", false)) {
			string path = gdk_window_get_menu_event(window.window);
			MenuBar menubar = window.get_data("__menubar__") as MenuBar;
			message("path = %s", path);
			if(menubar != null) {
				MenuItem item = Locator.locate(menubar, path);
				if(item != null) {
					item.activate();
				} else {
					message("item lookup failure");
				}
			} else {
				message("menubar lookup failure");
			}
		}
		return false;
	}

}
