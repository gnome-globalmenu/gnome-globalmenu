using Gtk;


namespace Gnomenu {
	public enum BackgroundType {
		NONE,
		COLOR,
		PIXMAP
	}
	public enum Gravity { /*Text Gravity, differ from Gdk.Gravity*/
		DOWN, 
		UP, /*Rarely used: up-side-down!*/
		LEFT,
		RIGHT
	}

	protected double gravity_to_text_angle(Gravity g) {
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
	protected ArrowType gravity_to_arrow_type(Gravity g) {
		switch(g) {
			case Gravity.UP:
				return ArrowType.UP;
			case Gravity.LEFT:
				return ArrowType.LEFT;
			case Gravity.RIGHT:
				return ArrowType.RIGHT;
			case Gravity.DOWN:
			default:
				return ArrowType.DOWN;
		}
	}
	protected ShadowType item_state_to_shadow_type(MenuItemState state) {
		switch(state) {
			case MenuItemState.TOGGLED:
				return ShadowType.IN;
			case MenuItemState.UNTOGGLED:
				return ShadowType.OUT;
			case MenuItemState.TRISTATE:
			default:
				return ShadowType.ETCHED_IN;
		}
	}

	/** 
	 * MenuShellHelper needs to be written.
	 * It has the potential to break in 
	 * future vala releases.
	 * */
	[Compact]
	[CCode (has_type_id = false)]
	protected class MenuShellHelper {
		public weak MenuItem get(int index) {
			return gtk_container_get_children(this as Container).nth_data(index) as MenuItem;
		}
		public bool has(int index) {
			List<weak Widget> children = gtk_container_get_children(this as Container);
			return index >= 0 && index < children.length();
		}
		public void truncate(int length) {
			while(gtk_container_get_children(this as Container).length() > length) {
				weak Widget child = gtk_container_get_children(this as Container).last().data as Widget;
				(this as Container).remove(child);
			}
		}
		public uint length() {
			return gtk_container_get_children(this as Container).length();
		}
	}
}

