/**
 * This is the functions that superrides GtkMenuBar vtable.
 */
internal class MenuBar {
	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, map)")]
	private extern const int OffsetMap;
	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, size_request)")]
	private extern const int OffsetSizeRequest;
	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, can_activate_accel)")]
	private extern const int OffsetCanActivateAccel;
	[CCode (has_target = false)]
	private delegate void MapFunc(Gtk.Widget? widget);
	[CCode (has_target = false)]
	private delegate void SizeRequestFunc(Gtk.Widget? widget, ref Gtk.Requisition req);
	[CCode (has_target = false)]
	private delegate bool CanActivateAccelfunc(Gtk.Widget? widget);

	public MenuBar() {
		Superrider.superride(typeof(Gtk.MenuBar), OffsetMap, (void*)map);
		Superrider.superride(typeof(Gtk.MenuBar), OffsetSizeRequest, (void*)size_request);
		Superrider.superride(typeof(Gtk.MenuBar), OffsetCanActivateAccel, (void*)can_activate_accel);
	}
	public static void map(Gtk.Widget? widget) {
		MapFunc super = (MapFunc) Superrider.peek_super(typeof(Gtk.MenuBar), OffsetMap);
		MapFunc @base = (MapFunc) Superrider.peek_base(typeof(Gtk.MenuBar), OffsetMap);

		debug("map called");

		var factory = MenuBarAgentFactory.get();
		var agent = factory.create(widget as Gtk.MenuBar);
		if(agent.quirks.has(MenuBarAgent.QuirkType.REGULAR_WIDGET)) {
			super(widget);
			return;
		}
		if(agent.settings.show_local_menu) {
			super(widget);
			return;
		}

		widget.set_flags(Gtk.WidgetFlags.MAPPED);
		@base(widget);
		if(widget.window != null) widget.window.hide();
	}
	public static void size_request(Gtk.Widget? widget, ref Gtk.Requisition req) {
		debug("size_request called");
		assert(widget is Gtk.MenuBar);
		var factory = MenuBarAgentFactory.get();
		var agent = factory.create(widget as Gtk.MenuBar);

		SizeRequestFunc super = (SizeRequestFunc) 
			Superrider.peek_super(typeof(Gtk.MenuBar), OffsetSizeRequest);

		super(widget, ref req);

		if(agent.quirks.has(MenuBarAgent.QuirkType.REGULAR_WIDGET))
			return;

		if(agent.settings.show_local_menu) {
			return;
		}

		req.width = 0;
		req.height = 0;
	}
	public static bool can_activate_accel(Gtk.Widget? widget) {
		assert(widget is Gtk.MenuBar);
		return widget.sensitive;
	}

	public static void set_children_menubar(Gtk.MenuBar menubar) {
		List<weak Gtk.Widget> children = menubar.get_children();
		foreach(var child in children) {
			Widget.set_menubar_r(child, menubar);
		}
	}

	public static void queue_changed(Gtk.MenuBar menubar) {
		var factory = MenuBarAgentFactory.get();
		var agent = factory.create(menubar);
		agent.queue_changed();
	}
}
