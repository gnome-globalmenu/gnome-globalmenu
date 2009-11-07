public class MenuBarInfo {
	public enum QuirkType {
		/* usual thing */
		NONE = 0,
		/* There is already a global menu on the window,
		 * this one is merely a regular widget */
		REGULAR_WIDGET = 1,
		/* for GtkMenuBar in a bonobo plug */
		HIDE_PARENT = 2,
		/* for wxGTK 2.8.10 programs */
		CHANGE_STYLE = 4,
	}
	public QuirkType quirks;

	private weak Gtk.MenuBar _menubar;
	private Gnomenu.Settings settings;

	public Gtk.MenuBar menubar {
		get { return _menubar; }
		private set { _menubar = value; }
	
	}
	private bool dirty = false;
	private Gdk.Window? event_window {
		get {
			if(menubar == null) return null;
			var toplevel = menubar.get_toplevel();
			if(toplevel == null) return null;
			return toplevel.window;
		}
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


	public MenuBarInfo (Gtk.MenuBar menubar) {
		this.menubar = menubar;
		MenuBarInfoFactory.get().associate(menubar, this);
		menubar.weak_ref(menubar_disposed, this);

		settings = new Gnomenu.Settings(menubar.get_screen());

		MenuBar.set_children_menubar(menubar);
		menubar.queue_resize();
		if(menubar.is_mapped()) MenuBar.map(menubar);
		message("info created");
	}
	
	~MenuBarInfo() {
		message("dispose MenuBarInfo");
		release_menubar();
	}

	private void release_menubar() {
		if(menubar == null) return;
		menubar.weak_unref(menubar_disposed, this);
	}

	public void queue_changed() {
		if(dirty == false) {
			dirty = true;
			Timeout.add(1000, send_globalmenu_message);
		}
	}

	[CCode (cname = "gdk_window_set_menu_context")]
	protected extern void gdk_window_set_menu_context (Gdk.Window window, string? context);
	private bool send_globalmenu_message() {
		message("FIXME: STUB send_globalmenu_message()");
		dirty = false;

		set_by_atom(Gdk.Atom.intern("_NET_GLOBALMENU_MENU_CONTEXT", false),
				Serializer.to_string(menubar)
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
