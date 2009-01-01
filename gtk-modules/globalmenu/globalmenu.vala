using Gtk;

namespace GnomenuGtk {

	[CCode (cname = "gdk_window_get_desktop_hint")]
	protected extern bool gdk_window_get_is_desktop (Gdk.Window window);
	[CCode (cname = "gtk_widget_get_toplevel_window")]
	protected extern weak Gtk.Window? gtk_widget_get_toplevel_window (Gtk.Widget widget);
	[CCode (cname = "gdk_window_set_menu_context")]
	protected extern void gdk_window_set_menu_context (Gdk.Window window, string? context);
	[CCode (cname = "gdk_window_get_menu_event")]
	protected extern string gdk_window_get_menu_event (Gdk.Window window);

	[CCode (cname = "gtk_window_find_menubar")]
	protected extern weak MenuBar gtk_window_find_menubar (Widget window);

	protected ulong changed_hook_id;
	protected ulong hc_hook_id;

	protected void add_emission_hooks() {
		uint signal_id = Signal.lookup("changed", typeof(MenuBar));
		uint signal_id_hc = Signal.lookup("hierarchy-changed", typeof(Gtk.Widget));

		changed_hook_id = Signal.add_emission_hook(signal_id, 0, changed_eh, null);
		hc_hook_id = Signal.add_emission_hook (signal_id_hc, 0, hierachy_changed_eh, null);

		List<weak Widget> toplevels = gtk_window_list_toplevels();
		foreach(Widget toplevel in toplevels) {
			if(!(toplevel is Window)) continue;
			MenuBar menubar = gtk_window_find_menubar(toplevel as Container);
			
			if(menubar == null) continue;
			menubar_set_local(menubar, 
				menubar_should_be_skipped(menubar));
			bind_menubar_to_window(menubar, toplevel as Window);
			Signal.emit_by_name(menubar, "changed", 
					typeof(Widget), menubar, null);
		}
	}

	protected void remove_emission_hooks() {
		List<weak Widget> toplevels = gtk_window_list_toplevels();
		foreach(Widget toplevel in toplevels) {
			if(!(toplevel is Window)) continue;
			MenuBar menubar = (MenuBar) toplevel.get_data("__menubar__");
			if(menubar == null) continue;
			unbind_menubar_from_window(menubar, toplevel as Window);
			menubar.queue_resize();
			if(0 != (menubar.get_flags() & WidgetFlags.REALIZED)) {
				menubar.unrealize();
				if(menubar.visible) {
					menubar.map();
					menubar.queue_draw();
				}
				else 
					menubar.realize();
			}
			if((0 != (toplevel.get_flags() & WidgetFlags.REALIZED))) {
				gdk_window_set_menu_context(toplevel.window, null);
			}
		}
		uint signal_id = Signal.lookup("changed", typeof(MenuBar));
		uint signal_id_hc = Signal.lookup("hierarchy-changed", typeof(Gtk.Widget));

		Signal.remove_emission_hook (signal_id, changed_hook_id);
		Signal.remove_emission_hook (signal_id_hc, hc_hook_id);
	}
	private bool changed_eh (SignalInvocationHint ihint, 
			[CCode (array_length_pos = 1.9) ]
			Value[] param_values
	) {
		MenuBar self = param_values[0].get_object() as MenuBar;
		if(self != null) {
			if(menubar_get_local(self)) return true;
			if(ihint.run_type != SignalFlags.RUN_FIRST) return true;
			Gtk.Window toplevel = (Gtk.Window) self.get_data("__toplevel__");

			if(toplevel != null) {
				if(0 != (toplevel.get_flags() & WidgetFlags.REALIZED)) {
					gdk_window_set_menu_context(toplevel.window, 
							Serializer.to_string(self)
							);
				}
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
			unbind_menubar_from_window(self, old_toplevel_window);
		}

		if(menubar_should_be_skipped(self)) {
			menubar_set_local(self, true);
		} else {
			menubar_set_local(self, false);
			bonobo_plug_widget_hack(self);
		}

		if(toplevel_window != null) {
			bind_menubar_to_window(self, toplevel_window);
	  	} 
		return true;
	}

	private bool menubar_should_be_skipped(MenuBar menubar) {
		weak Gtk.Widget parent = menubar;
		GLib.Type panel_applet_type = GLib.Type.from_name("PanelApplet");
		GLib.Type gnomenu_menu_bar_type = GLib.Type.from_name("GnomenuMenuBar");
		GLib.Type panel_menu_bar_type = GLib.Type.from_name("PanelMenuBar");
		while(parent is Gtk.Widget) {
			GLib.Type type = parent.get_type();

			if(type.is_a(panel_applet_type)
			|| type.is_a(gnomenu_menu_bar_type)
			|| type.is_a(panel_menu_bar_type))  {
				debug("menu bar skipped");
				return true;
			}
			parent = parent.parent;
		} 
		debug("not skipped");
		return false;
	}

	private void bonobo_plug_widget_hack(Gtk.Widget self) {
		weak Gtk.Widget parent = self.parent;
		while(parent is Gtk.Widget) {
			weak string typename = parent.get_type().name();
			if(typename.str("Bonobo")!= null) {
				debug("Hiding %s", typename);
				parent.hide();
			}
			parent = parent.parent;
		} 
	}

	private bool menubar_get_local(MenuBar menubar) {
		bool is_local = true;
		menubar.get("local", &is_local, null);
		return is_local;
	}
	private void menubar_set_local(MenuBar menubar, bool value) {
		menubar.set("local", value, null);
	}
	private MenuBar? find_menubar(Container widget) {
		List<weak Widget> children = gtk_container_get_children(widget);
		foreach(Widget child in children) {
			if(child is MenuBar) return child as MenuBar;
			if(child is Container) {
				MenuBar menubar = find_menubar(child as Container);
				if(menubar != null && !menubar_get_local(menubar)) {
					return menubar;
				}
			}
		}	
		return null;
	}

	private void unbind_menubar_from_window(MenuBar menubar, Window window) {
		MenuBar old_menubar = null;
		old_menubar = (MenuBar) window.get_data("__menubar__");
		if(old_menubar == menubar) {
			window.property_notify_event -= window_property_notify_event;
			window.realize -= window_realize;
			window.set_data("__menubar__", null);
			menubar.set_data("__toplevel__", null);
			debug("Unbind bar %p from window %p(%s)", menubar, window, window.get_name());
		} else {
			debug("old_menubar = %p, menubar = %p, unbinding fails", old_menubar, menubar);
		}
	}
	private void bind_menubar_to_window(MenuBar menubar, Window window) {
		MenuBar old_menubar = null;
		old_menubar = (MenuBar) window.get_data("__menubar__");
		if(old_menubar != null) unbind_menubar_from_window(old_menubar, window);

		menubar.set_data_full("__toplevel__", window.ref(), g_object_unref);
		window.set_data_full("__menubar__", menubar.ref(), g_object_unref);
		window.add_events(Gdk.EventMask.PROPERTY_CHANGE_MASK);
		window.property_notify_event += window_property_notify_event;
		window.realize += window_realize;
		debug("Bind bar %p from window %p(%s)", menubar, window, window.get_name());
	}

	private void window_realize(Gtk.Window window) {
		MenuBar menubar = (MenuBar) window.get_data("__menubar__");
		Signal.emit_by_name(menubar, "changed", 
				typeof(Widget), menubar, null);
			
	}
	private bool window_property_notify_event (Window window, Gdk.EventProperty event) {
		if(event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_EVENT", false)) {
			string path = gdk_window_get_menu_event(window.window);
			MenuBar menubar = window.get_data("__menubar__") as MenuBar;
			debug("path = %s", path);
			if(menubar != null) {
				MenuItem item = Locator.locate(menubar, path);
				if(item != null) {
					item.activate();
					debug("item %p is activated", item);
					if(item.submenu != null) {
						item.submenu.show();
					}
				} else {
					warning("item lookup failure");
				}
			} else {
				warning("menubar lookup failure");
			}
		}
		return false;
	}

}
