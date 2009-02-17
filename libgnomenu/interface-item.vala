namespace Gnomenu {
	public interface Item: GLib.Object {
		[CCode (cname = "GnomenuItemType")]
		public enum Type {
			NORMAL = 0,
			CHECK = 1,
			RADIO = 2,
			IMAGE = 3,
			SEPARATOR = 4,
			ARROW = 5,
			ICON = 6,
		}
		[CCode (cname = "GnomenuItemState")]
		public enum State {
			UNTOGGLED = 0,
			TOGGLED = 1,
			TRISTATE = 2,
		}
		public abstract Shell shell {get;}
		public abstract Shell sub_shell {get;}
		public abstract bool has_sub_shell {get; set;}

		public Shell? toplevel_shell {get {
			return shell.toplevel_shell;
		} }
		public int item_position {get {
			return shell.get_item_position(this);
		}}
		public abstract string? item_id {get; set;}

		public abstract Item.Type item_type {get; set;}
		public abstract bool item_use_underline {get; set;}
		public abstract bool item_sensitive{get; set;}
		public abstract bool item_visible{get; set;}
		public abstract Item.State item_state {get; set;}
		public abstract string? item_label {get; set;}
		public abstract string? item_icon {get; set;}
		public abstract string? item_accel_text {get; set;}
		public abstract string? item_font {get; set;}


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
		 * menu hierarch until reaching the toplevel menu bar.
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
		 * 0/New/Message (If the toplevel menu bar is not found).
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

		public static Item.State state_from_string(string? str) {
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
		public static weak string? state_to_string(Item.State state) {
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
		public static Item.Type type_from_string(string? str) {
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
		public static bool type_has_label(Item.Type type) {
			if(type == Item.Type.NORMAL
			|| type == Item.Type.IMAGE
			|| type == Item.Type.CHECK
			|| type == Item.Type.RADIO
			) return true;
			return false;	
		}
		public static weak string? type_to_string(Item.Type type) {
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
}
