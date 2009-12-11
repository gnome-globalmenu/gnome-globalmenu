internal class MenuBarInfo {
	public enum QuirkType {
		/* usual thing */
		NONE = 0,
		/* this one is merely a regular widget */
		REGULAR_WIDGET = 1,
		/* for GtkMenuBar in a bonobo plug */
		BONOBO_PLUG = 2,
		/* for wxGTK 2.8.10 programs */
		WX_GTK= 4;
		public bool has(QuirkType value) {
			return (value & this) != 0;
		}
		public void add(QuirkType value) {
			this = (value | this);
		}
		public void remove(QuirkType value) {
			this = (this & ~((uint)value));
		}
	}
	public QuirkType quirks;

	private static Gdk.Atom atom_select = Gdk.Atom.intern("_NET_GLOBALMENU_MENU_SELECT", false);
	private static Gdk.Atom atom_deselect = Gdk.Atom.intern("_NET_GLOBALMENU_MENU_DESELECT", false);
	private static Gdk.Atom atom_activate = Gdk.Atom.intern("_NET_GLOBALMENU_MENU_EVENT", false);

	private signal void quirks_changed(QuirkType old_quirks);

	private weak Gtk.MenuBar _menubar;
	public Gnomenu.Settings settings {get; private set;}

	public Gtk.MenuBar menubar {
		get { return _menubar; }
		private set { _menubar = value; }
	}

	private weak Gdk.Window? event_window {
		get;
		set;
	}
	private weak Gtk.Widget toplevel {
		get;
		set;
	}

	private static void menubar_disposed(void* data, Object? object) {
		/*
		 * the only reference to MenuBarInfo is held by the menubar.
		 * when menubar is disposed, the weak_ref_notify is invoked before
		 * its reference on MenuBarInfo is released.
		 *
		 * When this callback is invoked, we are 100% sure that
		 * the menubar is already invalid, and Gtk will take care of all
		 * the signal removal stuff. Therefore we simply detach the info
		 * from the menubar.
		 *
		 */
		((MenuBarInfo*)data)->menubar = null;
	}
	private static void toplevel_disposed(void* data, Object? object) {
		((MenuBarInfo*)data)->toplevel = null;
	}
	private static void event_window_disposed(void* data, Object? object) {
		((MenuBarInfo*)data)->event_window = null;
	}

	public MenuBarInfo (Gtk.MenuBar menubar) {
		this.menubar = menubar;
		MenuBarInfoFactory.get().associate(menubar, this);
		settings = new Gnomenu.LocalSettings();
		settings.notify["show-local-menu"] += show_local_menu_changed;
		settings.notify["show-menu-icons"] += show_menu_icons_changed;

		menubar.weak_ref(menubar_disposed, this);

		menubar.hierarchy_changed += sync_toplevel;
		menubar.hierarchy_changed += sync_quirks;

		sync_quirks();
		sync_toplevel();

		MenuBar.set_children_menubar(menubar);
		show_local_menu_changed();
		message("info created");
	}
	
	~MenuBarInfo() {
		message("dispose MenuBarInfo");
		release_menubar();
		release_toplevel();
		release_event_window();
	}

	private void release_menubar() {
		if(menubar == null) return;
		menubar.hierarchy_changed -= sync_toplevel;
		menubar.hierarchy_changed -= sync_quirks;
		menubar.weak_unref(menubar_disposed, this);
	}

	private void release_toplevel() {
		if(toplevel == null) return;
		toplevel.realize -= sync_event_window;
		toplevel.unrealize -= sync_event_window;
		toplevel.weak_unref(toplevel_disposed, this);
	}

	private void release_event_window() {
		if(event_window == null) return;
		event_window.remove_filter(event_filter);
		event_window.weak_unref(event_window_disposed, this);
		settings.attach_to_window(null);
	}

	public void queue_changed() {
		if(quirks.has(QuirkType.REGULAR_WIDGET)) return;
		if(dirty == false) {
			dirty = true;
			Gdk.threads_add_timeout_full(settings.changed_notify_timeout, send_globalmenu_message);
		}
	}

	private void sync_quirks() {
		var toplevel = menubar.get_toplevel();
		var old_quirks = quirks;

		quirks = QuirkType.NONE;

		if(!toplevel.is_toplevel()) {
			quirks = QuirkType.REGULAR_WIDGET;
		}

		if(has_parent_type_name("PanelMenuBar")) {
			quirks = QuirkType.REGULAR_WIDGET;
		}

		if(has_parent_type_name("Gnomenu")) {
			quirks = QuirkType.REGULAR_WIDGET;
		}

		if(has_parent_type_name("PanelApplet")) {
			quirks = QuirkType.REGULAR_WIDGET;
		}

		if(has_parent_type_name("GtkNotebook")) {
			quirks = QuirkType.REGULAR_WIDGET;
		}

		if(has_parent_type_name("GtkPizza")) {
			quirks = QuirkType.WX_GTK;
		}

		if(has_parent_type_name("BonoboDockBand")) {
			quirks = QuirkType.BONOBO_PLUG;
		}

		message("quirks = %d", quirks);
		this.quirks_changed(old_quirks);
	}

	private void sync_toplevel() {
		release_toplevel();
		if(menubar == null) toplevel = null;
		toplevel = menubar.get_toplevel();
		if(toplevel != null) {
			toplevel.weak_ref(toplevel_disposed, this);
			toplevel.realize += sync_event_window;
			toplevel.unrealize += sync_event_window;
		}
		sync_event_window();
	}

	private void sync_event_window() {
		release_event_window();
		if(toplevel == null) event_window = null;
		event_window = toplevel.window;
		if(event_window != null) {
			event_window.add_filter(event_filter);
			event_window.weak_ref(event_window_disposed, this);
		}
		settings.attach_to_window(event_window);
	}

	private bool has_parent_type_name(string typename_pattern) {
		if(menubar == null) return false;
		for(Gtk.Widget parent = menubar;
				parent != null;
				parent = parent.parent) {

			if(parent.get_type().name().str(typename_pattern) != null) {
				return true;
			}
		}
		return false;
	}

	private void show_menu_icons_changed() {
		message("menu icons changed!");
		this.queue_changed();
	}

	private void show_local_menu_changed() {
		menubar.queue_resize();
		if(quirks.has(QuirkType.WX_GTK)) {
			menubar.style_set(menubar.get_style());
		}
		if(menubar.is_mapped()) {
			MenuBar.map(menubar);
		}
		if(quirks.has(QuirkType.WX_GTK)) {
			for(Gtk.Widget parent = menubar;
					parent != null;
					parent = parent.parent) {

				if(parent.get_type().name().str("BonoboDockBand") == null) continue;
				if(!quirks.has(QuirkType.REGULAR_WIDGET)
				&& !settings.show_local_menu ) {
					if(parent.is_realized())
					parent.window.hide();
				} else {
					if(parent.is_realized())
					parent.window.show();
				}
				break;
			}
		}
	}

	[CCode (instance_pos = -1)]
	private Gdk.FilterReturn event_filter(Gdk.XEvent xevent, Gdk.Event event) {
		/* This weird extra level of calling is to avoid a type cast in Vala
		 * which will cause the loss of delegate target. */
		return real_event_filter(&xevent, event);
	}
	[CCode (instance_pos = -1)]
	private Gdk.FilterReturn real_event_filter(X.Event* xevent, Gdk.Event event) {
		switch(xevent->type) {
			case X.EventType.PropertyNotify:
				Gdk.Atom atom = Gdk.x11_xatom_to_atom(xevent->xproperty.atom);
				if(!(atom_select == atom)
				&& !(atom_deselect == atom)
				&& !(atom_activate == atom))
					break;
				var path = get_by_atom(atom);
				var item = Locator.locate(menubar, path);
				if(item == null) {
					warning("item not found. path=%s", path);
					break;
				}
				if(atom_select == atom) {
					select_item(item);
				}
				if(atom_deselect == atom) {
					deselect_item(item);
				}
				if(atom_activate == atom) {
					activate_item(item);
				}
				break;
		}
		return Gdk.FilterReturn.CONTINUE;
	}

	private void select_item(Gtk.MenuItem item) {
		item.select();
		if(item.submenu != null) {
			item.submenu.show();
		}
	}
	private void deselect_item(Gtk.MenuItem item) {
		item.deselect();
		if(item.submenu != null) {
			item.submenu.hide();
		}
	}
	private void activate_item(Gtk.MenuItem item) {
		item.activate();
	}
	private bool dirty = false;
	private bool send_globalmenu_message() {
		message("FIXME: STUB send_globalmenu_message()");
		dirty = false;

		var ser = new Serializer();
		ser.disable_pixbuf = ! settings.show_menu_icons;
		message("disable_pixbuf = %s", ser.disable_pixbuf.to_string());
		ser.pretty_print = false;
		set_by_atom(Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false),
				ser.to_string(menubar)
				);
		
		return false;
	}

	public string? get_by_atom(Gdk.Atom atom) {
		string context = null;
		Gdk.Atom actual_type;
		Gdk.Atom type = Gdk.Atom.intern("STRING", false);
		int actual_format;
		int actual_length;
		Gdk.property_get(event_window,
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
		if(value != null) {
			Gdk.Atom type = Gdk.Atom.intern("STRING", false);
			Gdk.property_change(event_window,
				atom, type,
				8,
				Gdk.PropMode.REPLACE,
				value, 
				(int) value.size() + 1
			);
		} else {
			Gdk.property_delete(event_window, atom);
		}
	}

}
