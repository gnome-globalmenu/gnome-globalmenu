/**
 * This is the functions that superrides GtkMenuBar vtable.
 */
internal class Widget {
	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, parent_set)")]
	private extern const int OffsetParentSet;
	[CCode (has_target = false)]
	private delegate void ParentSetFunc(Gtk.Widget? widget, Gtk.Widget? old_parent);

	public Widget() {
		Superrider.superride(typeof(Gtk.Widget), OffsetParentSet, (void*)parent_set);
	}

	~Widget() {
		/*FIXME: remove all extra data members */
	}
	public static void parent_set(Gtk.Widget? widget, Gtk.Widget? old_parent) {
		ParentSetFunc super = (ParentSetFunc) Superrider.peek_super(typeof(Gtk.Widget), OffsetParentSet);
		if(super != null) super(widget, old_parent);

		/* Detect menu bar */
		if(widget is Gtk.MenuBar) {
			var factory = MenuBarAgentFactory.get();
			factory.create(widget as Gtk.MenuBar);
			debug("menubar detected %p", widget);
		}

		if(widget is Gtk.MenuBar || widget is Gtk.Menu) return;

		var parent = widget.parent;
		Gtk.MenuBar menubar = null;
		if(parent != null) {
			menubar = get_menubar(parent);
		}
		set_menubar_r(widget, menubar);
	}

	public static unowned Gtk.MenuBar get_menubar(Gtk.Widget widget) {
		if(widget is Gtk.MenuBar) return widget as Gtk.MenuBar;
		unowned Gtk.MenuBar menubar = widget.get_data<Gtk.MenuBar>("globalmenu-menubar");
		return menubar;
	}

	private static void set_menubar(Gtk.Widget widget, Gtk.MenuBar? menubar) {
		/* never called on a Gtk.MenuBar.
		 * if reached the assertion, check the bt and figure out why.
		 *
		 * */
		assert(!(widget is Gtk.MenuBar));
		var old_menubar = get_menubar(widget);
		if(old_menubar == menubar) return;
		if(old_menubar != null) {
			try_disconnect_label(widget);
			try_disconnect_menuitem(widget);
			MenuBar.queue_changed(old_menubar);
		}

		widget.set_data("globalmenu-menubar", (void*) menubar);

		if(menubar != null) {
			try_connect_label(widget);
			try_connect_menuitem(widget);
			MenuBar.queue_changed(menubar);
		}
	}

	/* This function is called by MenuBar.set_children_menubars */
	internal static void set_menubar_r(Gtk.Widget widget, Gtk.MenuBar? menubar) {
		/* Stop ASAP if we find ourself started the recursive set menubar process
		 * from a widget that is hierarchically above a menubar.
		 *
		 * This should happen only if when menubar == null. When a widget is being
		 * reparented we know it for sure, if the widget is above a menubar.
		 * */
		if(menubar == null && (widget is Gtk.MenuBar)) {
			return;
		}
		set_menubar(widget, menubar);
		if(widget is Gtk.Container) {
			List<weak Gtk.Widget> children = (widget as Gtk.Container).get_children();
			foreach(var child in children) {
				set_menubar_r(child, menubar);
			}
		}
		if(widget is Gtk.MenuItem) {
			var item = widget as Gtk.MenuItem;
			if(item.submenu != null)
			set_menubar_r(item.submenu, menubar);
		}
	}
	private static void try_connect_label(Gtk.Widget widget) {
		if(widget is Gtk.Label)
			widget.notify["label"] += simple_changed;
	}
	private static void try_connect_menuitem(Gtk.Widget widget) {
		if(widget is Gtk.MenuItem) {
			widget.notify["submenu"] += recursive_changed;
			widget.notify["visible"] += simple_changed;
			widget.notify["sensitive"] += simple_changed;
		}
		if(widget is Gtk.CheckMenuItem) {
			widget.notify["active"] += simple_changed;
			widget.notify["inconsistent"] += simple_changed;
			widget.notify["draw-as-radio"] += simple_changed;
		}
	}
	private static void try_disconnect_menuitem(Gtk.Widget widget) {
		if(widget is Gtk.MenuItem) {
			widget.notify -= simple_changed;
			widget.notify -= recursive_changed;
		}
	}
	private static void try_disconnect_label(Gtk.Widget widget) {
		if(widget is Gtk.Label)
			widget.notify -= simple_changed;
	}

	private static void simple_changed(Gtk.Widget widget, ParamSpec pspec) {
		var menubar = Widget.get_menubar(widget);
		assert(menubar != null);
		MenuBar.queue_changed(menubar);
	}

	private static void recursive_changed(Gtk.Widget widget, ParamSpec pspec) {
		assert(widget is Gtk.MenuItem);
		var item = widget as Gtk.MenuItem;
		var submenu = item.submenu;
		var old_submenu = item.get_data<Gtk.Menu>("old_submenu");
		var menubar = Widget.get_menubar(widget);

		if(submenu == old_submenu) {
			simple_changed(widget, pspec);
			return;
		}

		if(old_submenu != null) {
			Widget.set_menubar_r(old_submenu, null);
		}
		if(submenu != null) {
			Widget.set_menubar_r(submenu, menubar);
		}
		widget.set_data("old_submenu", (void*)submenu);

		simple_changed(widget, pspec);
	}
}
