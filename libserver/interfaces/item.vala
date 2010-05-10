namespace Gnomenu {
	[CCode (cname = "GnomenuItemType")]
	public enum ItemType {
		NORMAL = 0,
		CHECK = 1,
		RADIO = 2,
		IMAGE = 3,
		SEPARATOR = 4,
		ARROW = 5,
		ICON = 6,
	}
	[CCode (cname = "GnomenuItemState")]
	public enum ItemState {
		UNTOGGLED = 0,
		TOGGLED = 1,
		TRISTATE = 2,
	}
	public interface Item: GLib.Object {
		public abstract Shell shell {get;}
		public abstract Shell sub_shell {get;}
		public abstract bool has_sub_shell {get; set;}
		public abstract bool client_side_sub_shell {get; set;}

		public Shell? topmost_shell {get {
			return shell.topmost_shell;
		} }
		public int item_position {get {
			return shell.get_item_position(this);
		}}
		public abstract string? item_id {get; set;}

		public abstract ItemType item_type {get; set;}
		public abstract bool item_use_underline {get; set;}
		public abstract bool item_sensitive{get; set;}
		public abstract bool item_visible{get; set;}
		public abstract ItemState item_state {get; set;}
		public abstract string? item_label {get; set;}
		public abstract string? item_icon {get; set;}
		public abstract string? item_accel_text {get; set;}
		public abstract string? item_font {get; set;}


		public bool is_child_of(Item possible_parent) {
			Shell parent_shell;
			Item parent_item = this;
			for(parent_item = this; 
				parent_item != null && parent_item != possible_parent; 
				parent_shell = parent_item.shell, parent_item = parent_shell.owner) {
				continue;
			}
				
			return parent_item != null;
		}
		public string item_path_name {owned get {
			if(item_id != null)
				return item_id;
			else
				return item_position.to_string();
			}
		}
		/**
		 * Obtain the path of the this item.
		 *
		 * The path is constructed by backtracing the 
		 * menu hierarch until reaching the topmost menu bar.
		 *
		 * Notice that the [rev:] prefix in global menu specification
		 * is not implemented (yet).
		 *
		 * Here are several examples of returned strings:
		 *
		 * /0/1/3/0
		 *
		 * /File/New/Message
		 *
		 * /0/New/Message
		 *
		 * 0/New/Message (If the topmost menu bar is not found).
		 *
		 * The return value in the last case should 
		 * probably be replaced by null.
		 */
		public string item_path {owned get {
			StringBuilder sb = new StringBuilder("");
			Item item = this;
			Shell parent = item.shell;
			sb.append(item_path_name);
			while(parent != null) {
				item = parent.owner as Item;
				if(item == null) break;
				sb.prepend_unichar('/');
				sb.prepend(item.item_path_name);
				parent = item.shell;
			}
			sb.prepend_unichar('/');
			return sb.str;
		}}

		public static ItemState state_from_string(string? str) {
			switch(str) {
				case "true":
				case "toggled":
				case "t":
				case "1":
					return ItemState.TOGGLED;
				case "false":
				case "untoggled":
				case "f":
				case "0":
					return ItemState.UNTOGGLED;
				case null:
				default:
					return ItemState.TRISTATE;
			}
		}
		public static unowned string? state_to_string(ItemState state) {
			switch(state) {
				case ItemState.UNTOGGLED:
					return "untoggled";
				case ItemState.TOGGLED:
					return "toggled";
				case ItemState.TRISTATE:
					return null;
			}
			return null;
		}
		public static ItemType type_from_string(string? str) {
			switch(str) {
				case "check":
				case "c":
					return ItemType.CHECK;
				case "radio":
				case "r":
					return ItemType.RADIO;
				case "image":
				case "i":
					return ItemType.IMAGE;
				case "arrow":
				case "a":
					return ItemType.ARROW;
				case "separator":
				case "s":
					return ItemType.SEPARATOR;
				case "icon":
					return ItemType.ICON;
				case null:
				default:
					return ItemType.NORMAL;
			}
		}
		public static bool type_has_label(ItemType type) {
			if(type == ItemType.NORMAL
			|| type == ItemType.IMAGE
			|| type == ItemType.CHECK
			|| type == ItemType.RADIO
			) return true;
			return false;	
		}
		public static unowned string? type_to_string(ItemType type) {
			switch(type) {
				case ItemType.CHECK:
					return "check";
				case ItemType.RADIO:
					return "radio";
				case ItemType.NORMAL:
					return null;
				case ItemType.IMAGE:
					return "image";
				case ItemType.ICON:
					return "icon";
				case ItemType.ARROW:
					return "arrow";
				case ItemType.SEPARATOR:
					return "separator";
			}
			return null;
		}
	}
}
