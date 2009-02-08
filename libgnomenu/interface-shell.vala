namespace Gnomenu {
	public interface Shell: GLib.Object {
		public abstract Item? owner {get;}
		public abstract Item? get_item(int position);
		public abstract void truncate(int length);
		public abstract int length {get;}
		public abstract Item? get_item_by_id(string id);
		public abstract int get_item_position(Item item);
		public abstract void insert_item(Item item, int pos);
		public void append_item(Item item) {
			insert_item(item, -1);
		}
		public abstract void remove_item(Item item);
	}
}
