/**
 * This is the functions that superrides GtkMenuBar vtable.
 */
internal class MenubarSuperrider{

	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, map)")]
	private extern const int OffsetMap;
	[CCode (has_target = false)]
	private delegate void MapFunc(Gtk.Widget? widget);

#if GTK_VERSION == 3
	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, get_preferred_width)")]
	private extern const int OffsetPreferredWidth;
	[CCode (has_target = false)]
	private delegate void PreferredWidthFunc(Gtk.Widget? widget, ref int minimal_width, ref int natural_width);

	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, get_preferred_height)")]
	private extern const int OffsetPreferredHeight;
	[CCode (has_target = false)]
	private delegate void PreferredHeightFunc(Gtk.Widget? widget, ref int minimal_height, ref int natural_height);
#else
	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, size_request)")]
	private extern const int OffsetSizeRequest;
	[CCode (has_target = false)]
	private delegate void SizeRequestFunc(Gtk.Widget? widget, ref Gtk.Requisition req);
#endif

	[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, can_activate_accel)")]
	private extern const int OffsetCanActivateAccel;
	[CCode (has_target = false)]
	private delegate bool CanActivateAccelfunc(Gtk.Widget? widget);

	public MenubarSuperrider() {
		Superrider.superride(typeof(Gtk.MenuBar), OffsetMap, (void*)map);
		Superrider.superride(typeof(Gtk.MenuBar), OffsetCanActivateAccel, (void*)can_activate_accel);
#if GTK_VERSION == 3
		Superrider.superride(typeof(Gtk.MenuBar), OffsetPreferredWidth, (void*)get_preferred_width);
		Superrider.superride(typeof(Gtk.MenuBar), OffsetPreferredHeight, (void*)get_preferred_height);
#else
		Superrider.superride(typeof(Gtk.MenuBar), OffsetSizeRequest, (void*)size_request);
#endif
	}

	public static void map(Gtk.Widget? widget) {
		var @base = (MapFunc) Superrider.peek_base(typeof(Gtk.MenuBar), OffsetMap);

		debug("map called");

		widget.set_mapped(true);
		@base(widget);
		if(widget.get_window() != null) widget.get_window().hide();
	}
#if GTK_VERSION == 3
	public static void get_preferred_width(Gtk.Widget? widget, ref int natural, ref int minimal) {
		assert(widget is Gtk.MenuBar);

		var super = (PreferredWidthFunc) 
			Superrider.peek_super(typeof(Gtk.MenuBar), OffsetPreferredWidth);

		super(widget, ref natural, ref minimal);

		natural = 0;
		minimal = 0;
	}
	public static void get_preferred_height(Gtk.Widget? widget, ref int natural, ref int minimal) {
		assert(widget is Gtk.MenuBar);

		var super = (PreferredHeightFunc) 
			Superrider.peek_super(typeof(Gtk.MenuBar), OffsetPreferredHeight);

		super(widget, ref natural, ref minimal);

		natural = 10;
		minimal = 10;
	}
#else
	public static void size_request(Gtk.Widget? widget, ref Gtk.Requisition req) {
		assert(widget is Gtk.MenuBar);
		var super = (SizeRequestFunc) 
			Superrider.peek_super(typeof(Gtk.MenuBar), OffsetSizeRequest);

		super(widget, ref req);
		req.width = 0;
		req.height = 0;
	}
#endif
	public static bool can_activate_accel(Gtk.Widget? widget) {
		assert(widget is Gtk.MenuBar);
		return widget.sensitive;
	}

}
