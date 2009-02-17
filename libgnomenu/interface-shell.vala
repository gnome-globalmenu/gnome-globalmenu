namespace Gnomenu {
	public interface Shell: GLib.Object {
		public abstract Item? owner {get;}
		public abstract Item? get_item(int position);
		public abstract int length {get; set;}
		public abstract Item? get_item_by_id(string id);
		public abstract int get_item_position(Item item);
		public Shell? toplevel_shell {get {
			if(owner != null) return owner.toplevel_shell;
			return this;
		}}
	}
}
