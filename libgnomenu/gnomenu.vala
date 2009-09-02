namespace Gnomenu {

	public const string NET_GLOBALMENU_MENU_CONTEXT = "_NET_GLOBALMENU_MENU_CONTEXT";
	public const string NET_GLOBALMENU_MENU_EVENT = "_NET_GLOBALMENU_MENU_EVENT";
	public const string NET_GLOBALMENU_MENU_SELECT = "_NET_GLOBALMENU_MENU_SELECT";
	public const string NET_GLOBALMENU_MENU_DESELECT = "_NET_GLOBALMENU_MENU_DESELECT";

	public enum BackgroundType {
		NONE = 0,
		COLOR = 1,
		PIXMAP = 2,
	}
	public enum Gravity { /*Text Gravity, differ from Gdk.Gravity*/
		DOWN = 0, 
		UP = 1, /*Rarely used: up-side-down!*/
		LEFT = 2,
		RIGHT = 3
	}

	internal double gravity_to_text_angle(Gravity g) {
		switch(g) {
			case Gravity.UP:
				return 180;
			case Gravity.LEFT:
				return 270;
			case Gravity.RIGHT:
				return 90;
			case Gravity.DOWN:
			default:
				return 0;
		}
	}
	internal Gtk.ArrowType gravity_to_arrow_type(Gravity g) {
		switch(g) {
			case Gravity.UP:
				return Gtk.ArrowType.UP;
			case Gravity.LEFT:
				return Gtk.ArrowType.LEFT;
			case Gravity.RIGHT:
				return Gtk.ArrowType.RIGHT;
			case Gravity.DOWN:
			default:
				return Gtk.ArrowType.DOWN;
		}
	}

}
