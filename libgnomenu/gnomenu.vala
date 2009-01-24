namespace Gnomenu {
	public const string NET_GLOBALMENU_MENU_CONTEXT = "_NET_GLOBALMENU_MENU_CONTEXT";
	public const string NET_GLOBALMENU_MENU_EVENT = "_NET_GLOBALMENU_MENU_EVENT";

	public enum MenuItemType {
		NORMAL = 0,
		CHECK = 1,
		RADIO = 2,
		IMAGE = 3,
		SEPARATOR = 4,
		ARROW = 5,
		ICON = 6,
	}
	public enum MenuItemState {
		UNTOGGLED = 0,
		TOGGLED = 1,
		TRISTATE = 2,
	}
	protected MenuItemState item_state_from_string(string? str) {
		switch(str) {
			case "true":
			case "toggled":
			case "t":
			case "1":
				return MenuItemState.TOGGLED;
			case "false":
			case "untoggled":
			case "f":
			case "0":
				return MenuItemState.UNTOGGLED;
			case null:
			default:
				return MenuItemState.TRISTATE;
		}
	}
	protected weak string? item_state_to_string(MenuItemState state) {
		switch(state) {
			case MenuItemState.UNTOGGLED:
				return "untoggled";
			case MenuItemState.TOGGLED:
				return "toggled";
			case MenuItemState.TRISTATE:
				return null;
		}
		return null;
	}
	protected MenuItemType item_type_from_string(string? str) {
		switch(str) {
			case "check":
			case "c":
				return MenuItemType.CHECK;
			case "radio":
			case "r":
				return MenuItemType.RADIO;
			case "image":
			case "i":
				return MenuItemType.IMAGE;
			case "arrow":
			case "a":
				return MenuItemType.ARROW;
			case "separator":
			case "s":
				return MenuItemType.SEPARATOR;
			case "icon":
				return MenuItemType.ICON;
			case null:
			default:
				return MenuItemType.NORMAL;
		}
	}
	protected bool item_type_has_label(MenuItemType type) {
		if(type == MenuItemType.NORMAL
		|| type == MenuItemType.IMAGE
		|| type == MenuItemType.CHECK
		|| type == MenuItemType.RADIO
		) return true;
		return false;	
	}
	protected weak string? item_type_to_string(MenuItemType type) {
		switch(type) {
			case MenuItemType.CHECK:
				return "check";
			case MenuItemType.RADIO:
				return "radio";
			case MenuItemType.NORMAL:
				return null;
			case MenuItemType.IMAGE:
				return "image";
			case MenuItemType.ICON:
				return "icon";
			case MenuItemType.ARROW:
				return "arrow";
			case MenuItemType.SEPARATOR:
				return "separator";
		}
		return null;
	}
}
