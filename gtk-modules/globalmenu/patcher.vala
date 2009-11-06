public class Patcher {
	MenuBar menubar;
	public Patcher() {
		menubar = new MenuBar();
	}
	~Patcher() {
		Superrider.release_all();
	}
	private class MenuBar {
		[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, map)")]
		private extern const int OffsetMap;
		[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, size_request)")]
		private extern const int OffsetSizeRequest;
		[CCode (cname="G_STRUCT_OFFSET(GtkWidgetClass, can_activate_accel)")]
		private extern const int OffsetCanActivateAccel;
		private static delegate void MapFunc(Gtk.Widget? widget);
		public MenuBar() {
			Superrider.superride(typeof(Gtk.MenuBar), OffsetMap, (void*)map);
		}
		public static void map(Gtk.Widget? widget) {
			MapFunc super = (MapFunc) Superrider.peek_super(typeof(Gtk.MenuBar), OffsetMap);
			message("map called");
			super(widget);
		}
	}
}
