using Gtk;

namespace GlobalMenuGTK {

	[CCode (cname = "gtk_widget_get_toplevel_window")]
	protected extern weak Gtk.Window? gtk_widget_get_toplevel_window (Gtk.Widget widget);
	[CCode (cname = "gdk_window_set_menu_context")]
	protected extern void gdk_window_set_menu_context (Gdk.Window window, string? context);
	[CCode (cname = "gdk_window_get_menu_event")]
	protected extern string gdk_window_get_menu_event (Gdk.Window window);

	protected ulong changed_hook_id;
	protected ulong attached_hook_id;
	protected ulong detached_hook_id;
	
	public void init() {

		changed_hook_id = Signal.add_emission_hook(
				Signal.lookup("changed", typeof(MenuBar)),
				0, changed_eh, null);
		attached_hook_id = Signal.add_emission_hook (
				Signal.lookup("dyn-patch-attached", typeof(MenuBar)),
				0, attached_eh, null);
		detached_hook_id = Signal.add_emission_hook (
				Signal.lookup("dyn-patch-detached", typeof(MenuBar)),
				0, detached_eh, null);

	}

	public void uninit() {
		Signal.remove_emission_hook (
			Signal.lookup("changed", typeof(MenuBar)),
			changed_hook_id);
		Signal.remove_emission_hook (
			Signal.lookup("dyn-patch-attached", typeof(MenuBar)),
			attached_hook_id);
		Signal.remove_emission_hook (
			Signal.lookup("dyn-patch-detached", typeof(MenuBar)),
			detached_hook_id);

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

	}
	public bool attached_eh (SignalInvocationHint ihint, 
			[CCode (array_length_pos = 1.9) ]
			Value[] param_values ) {
		if(ihint.run_type != SignalFlags.RUN_FIRST) return true;
		MenuBar menubar = param_values[0].get_object() as MenuBar;
		Gtk.Window window = param_values[1].get_object() as Gtk.Window;
		debug("attached_eh menubar %p to window %p", menubar, window);
		if(menubar_should_be_skipped(menubar)) {
			menubar_set_local(menubar, true);
		} else {
			menubar_set_local(menubar, false);
			bonobo_plug_widget_hack(menubar);
		}
		bind_menubar_to_window(menubar, window);
		return true;
	}
	public bool detached_eh (SignalInvocationHint ihint, 
			[CCode (array_length_pos = 1.9) ]
			Value[] param_values ) {
		if(ihint.run_type != SignalFlags.RUN_FIRST) return true;
		MenuBar menubar = param_values[0].get_object() as MenuBar;
		Gtk.Window window = param_values[1].get_object() as Gtk.Window;

		debug("detached_eh menubar %p from window %p", menubar, window);
		unbind_menubar_from_window(menubar, window);
		return true;
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
			debug("changed_eh");
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
			if(child is MenuBar) {
				MenuBar menubar = child as MenuBar;

				if(menubar_should_be_skipped(menubar)) {
					menubar_set_local(menubar, true);
					return null;
				} else {
					menubar_set_local(menubar, false);
					return menubar;
				}
			}
			if(child is Container) {
				MenuBar menubar = find_menubar(child as Container);
				if(menubar != null) {
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
