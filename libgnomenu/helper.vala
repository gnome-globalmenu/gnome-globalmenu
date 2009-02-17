using Gtk;


namespace Gnomenu {
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
	protected ShadowType item_state_to_shadow_type(Item.State state) {
		switch(state) {
			case Item.State.TOGGLED:
				return ShadowType.IN;
			case Item.State.UNTOGGLED:
				return ShadowType.OUT;
			case Item.State.TRISTATE:
			default:
				return ShadowType.ETCHED_IN;
		}
	}

}

