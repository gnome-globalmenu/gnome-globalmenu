namespace Gnomenu {
	public const string NET_GLOBALMENU_MENU_CONTEXT = "_NET_GLOBALMENU_MENU_CONTEXT";
	public const string NET_GLOBALMENU_MENU_EVENT = "_NET_GLOBALMENU_MENU_EVENT";

	protected Item.State item_state_from_string(string? str) {
		switch(str) {
			case "true":
			case "toggled":
			case "t":
			case "1":
				return Item.State.TOGGLED;
			case "false":
			case "untoggled":
			case "f":
			case "0":
				return Item.State.UNTOGGLED;
			case null:
			default:
				return Item.State.TRISTATE;
		}
	}
	protected weak string? item_state_to_string(Item.State state) {
		switch(state) {
			case Item.State.UNTOGGLED:
				return "untoggled";
			case Item.State.TOGGLED:
				return "toggled";
			case Item.State.TRISTATE:
				return null;
		}
		return null;
	}
	protected Item.Type item_type_from_string(string? str) {
		switch(str) {
			case "check":
			case "c":
				return Item.Type.CHECK;
			case "radio":
			case "r":
				return Item.Type.RADIO;
			case "image":
			case "i":
				return Item.Type.IMAGE;
			case "arrow":
			case "a":
				return Item.Type.ARROW;
			case "separator":
			case "s":
				return Item.Type.SEPARATOR;
			case "icon":
				return Item.Type.ICON;
			case null:
			default:
				return Item.Type.NORMAL;
		}
	}
	protected bool item_type_has_label(Item.Type type) {
		if(type == Item.Type.NORMAL
		|| type == Item.Type.IMAGE
		|| type == Item.Type.CHECK
		|| type == Item.Type.RADIO
		) return true;
		return false;	
	}
	protected weak string? item_type_to_string(Item.Type type) {
		switch(type) {
			case Item.Type.CHECK:
				return "check";
			case Item.Type.RADIO:
				return "radio";
			case Item.Type.NORMAL:
				return null;
			case Item.Type.IMAGE:
				return "image";
			case Item.Type.ICON:
				return "icon";
			case Item.Type.ARROW:
				return "arrow";
			case Item.Type.SEPARATOR:
				return "separator";
		}
		return null;
	}
}
