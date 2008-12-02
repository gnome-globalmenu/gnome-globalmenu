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
	[Compact]
	[CCode (has_type_id = false)]
	protected class MenuShellHelper {
		public weak MenuItem get(int index) {
			return (this as Container).get_children().nth_data(index) as MenuItem;
		}
		public bool has(int index) {
			weak List<weak Widget> children = (this as Container).get_children();
			return index >= 0 && index < children.length();
		}
		public void truncate(int length) {
			while((this as Container).get_children().length() > length) {
				weak Widget child = (this as Container).get_children().last().data as Widget;
				(this as Container).remove(child);
			}
		}
		public uint length() {
			return (this as Container).get_children().length();
		}
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
}

