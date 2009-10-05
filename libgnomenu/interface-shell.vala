public interface Gnomenu.Shell: GLib.Object {
	public abstract Item? owner {get;}
	public abstract Item? get_item(int position);
	public abstract int length {get; set;}
	public abstract Item? get_item_by_id(string id);
	public abstract int get_item_position(Item item);
	public Shell? topmost_shell {
		get {
			if(owner != null) return owner.topmost_shell;
			return this;
		}
	}
	/**
	 * Look up a child item from a path.
	 * The path is a string constructed by two parts:
	 *
	 * [rev:]/id/id/id
	 *
	 * where rev: is an integer stamp, and id can be either
	 * the id property or the position of the menu item.
	 *
	 * return a strong reference of the menu item if found;
	 * null if not.
	 */
	public new Item? get_item_by_path(string path) {
		string[] tokens = path.split_set("/", -1);
		tokens.length = (int) strv_length(tokens);
		Shell shell = this;
		/*
		weak string rev = tokens[0];
		FIXME: check rev */
		for(int i = 1; i < tokens.length; i++) {
			weak string token = tokens[i];
			Item item = null;
			item = shell.get_item_by_id(token);
			if(item == null) {
				weak string endptr = null;
				int pos = (int) token.to_int64(out endptr);
				if(endptr.get_char() == 0) {
					item = shell.get_item(pos);
				}
			}
			if(i == tokens.length - 1 /*last token, maybe found*/) return item;	
			if(item == null /*intermediate item is not found*/) return null; 
			shell = item.sub_shell;
			if(shell == null /*intermediate menu is not found*/) return null;
		}
		return null;
	}
	/**
	 * This signal is emitted when a child item is activated
	 */
	public signal void activate(Item item);
	public signal void select(Item item);
	public signal void deselect(Item item);
}
