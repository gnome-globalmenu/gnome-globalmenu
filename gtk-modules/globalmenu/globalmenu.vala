using Gtk;

namespace GlobalMenuGTK {

	public enum Flags {
		NONE = 0,
		DISABLE_PIXBUF = 1,
		HYBRID = 2,
	}
	[CCode (cname = "gtk_widget_get_toplevel_window")]
	protected extern weak Gtk.Window? gtk_widget_get_toplevel_window (Gtk.Widget widget);
	[CCode (cname = "gdk_window_set_menu_context")]
	protected extern void gdk_window_set_menu_context (Gdk.Window window, string? context);
	[CCode (cname = "gdk_window_get_menu_event")]
	protected extern string gdk_window_get_menu_event (Gdk.Window window, Gdk.Atom atom);

	protected ulong changed_hook_id;
	protected ulong attached_hook_id;
	protected ulong detached_hook_id;

	protected bool disable_pixbuf = false;
	protected bool hybrid = false;
	public void init(Flags flags) {

		if((flags & Flags.DISABLE_PIXBUF) != 0) disable_pixbuf = true;
		if((flags & Flags.HYBRID ) != 0) hybrid = true;

		changed_hook_id = Signal.add_emission_hook(
				Signal.lookup("dyn-patch-changed", typeof(MenuBar)),
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
			Signal.lookup("dyn-patch-changed", typeof(MenuBar)),
			changed_hook_id);
		Signal.remove_emission_hook (
			Signal.lookup("dyn-patch-attached", typeof(MenuBar)),
			attached_hook_id);
		Signal.remove_emission_hook (
			Signal.lookup("dyn-patch-detached", typeof(MenuBar)),
			detached_hook_id);

		List<weak Widget> toplevels = Gtk.Window.list_toplevels();
		foreach(Widget toplevel in toplevels) {
			if(!(toplevel is Window)) continue;
			MenuBar menubar = (MenuBar) DynPatch.get_menubar(toplevel);
			if(menubar == null) continue;
			unbind_menubar_from_window(menubar, toplevel as Window);
			menubar.queue_resize();
			if(0 != (menubar.get_flags() & WidgetFlags.REALIZED)) {
				if(menubar.visible) {
					menubar.unrealize();
					menubar.map();
				} 
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
			Gtk.Window toplevel = DynPatch.get_window(self);

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
		GLib.Type gtk_notebook_type = GLib.Type.from_name("GtkNotebook");
		while(parent is Gtk.Widget) {
			GLib.Type type = parent.get_type();

			if(type.is_a(panel_applet_type)
			|| type.is_a(gnomenu_menu_bar_type)
			|| type.is_a(panel_menu_bar_type)
			|| type.is_a(gtk_notebook_type)
			)  {
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
			if(typename.str("BonoboDockBand")!= null) {
				debug("Hiding %s", typename);
				parent.hide();
				break;
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
		List<weak Widget> children = widget.get_children();
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
		window.property_notify_event -= window_property_notify_event;
		window.realize -= window_realize;
		debug("Unbind bar %p from window %p", menubar, window);
	}
	private void bind_menubar_to_window(MenuBar menubar, Window window) {

		window.add_events(Gdk.EventMask.PROPERTY_CHANGE_MASK);
		window.property_notify_event += window_property_notify_event;
		window.realize += window_realize;
		debug("Bind bar %p from window %p", menubar, window );
	}

	private void window_realize(Gtk.Window window) {
		MenuBar menubar = DynPatch.get_menubar(window);
		Signal.emit_by_name(menubar, "dyn-patch-changed", 
				typeof(Widget), menubar, null);
			
	}
	private MenuItem? lookup_item(Window window, string path) {
		MenuBar menubar = DynPatch.get_menubar(window);
		debug("path = %s", path);
		if(menubar != null) {
			MenuItem item = Locator.locate(menubar, path);
			if(item != null) {
				debug("item %p is activated", item);
				return item;
			} else {
				warning("item lookup failure");
				return null;
			}
		} else {
			warning("menubar lookup failure");
			return null;
		}
	}
	private bool window_property_notify_event (Window window, Gdk.EventProperty event) {
		if(event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_EVENT", false)) {
			var path = gdk_window_get_menu_event(window.window, event.atom);
			var item = lookup_item(window, path);
			if(item != null) item.activate();
		}
		if(event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_HIGHLIGHT", false) ||
		   event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_SELECT", false)
		) {
			var path = gdk_window_get_menu_event(window.window, event.atom);
			var item = lookup_item(window, path);
			if(item != null) {
				item.select();
				debug("item %p is selected", item);
				if(item.submenu != null) {
					if(hybrid == false)
						item.submenu.show();
					 else {
						debug("client side popup");
						item.submenu.popup(null, null, null, 3, 
							Gtk.get_current_event_time()
							);
					}
				}
			}
		}
		if(event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_DEHIGHLIGHT", false) ||
		   event.atom == Gdk.Atom.intern("_NET_GLOBALMENU_MENU_DESELECT", false)
		) {
			var path = gdk_window_get_menu_event(window.window, event.atom);
			var item = lookup_item(window, path);
			if(item != null) {
				item.deselect();
				debug("item %p is selected", item);
				if(item.submenu != null) {
					if(hybrid == false)
						item.submenu.hide();
					 else {
						debug("client side popup");
						item.submenu.popdown();
					}
				}
			}
		}
		return false;
	}
}
