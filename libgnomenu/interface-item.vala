namespace Gnomenu {
	public interface Item: GLib.Object {
		public abstract Shell shell {get;}
		public abstract Shell sub_shell {get;}
		public abstract bool has_sub_shell {get; set;}

		public Shell? toplevel_shell {get {
			weak Item item = this;
			weak Shell parent = shell;
			while(parent != null) {
				item = parent.owner;
				if(item != null) {
					parent = item.shell;
				} else {
					break;
				}
			}
			return parent;
		} }
		public int item_position {get {
			return shell.get_item_position(this);
		}}
		public abstract string? item_id {get; set;}

		public abstract string? item_type {get; set;}
		public abstract bool item_use_underline {get; set;}
		public abstract bool item_sensitive{get; set;}
		public abstract bool item_visible{get; set;}
		public abstract string? item_state {get; set;}
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
	}
}
