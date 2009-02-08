namespace Gnomenu {
	public interface Shell: GLib.Object {
		public abstract MenuItem? owner {get; set;}
		public abstract MenuItem? get_item(int position);
		public abstract void truncate(int length);
		public abstract int length {get;}
		public abstract MenuItem? get_item_by_id(string id);
		public abstract void insert_item(MenuItem item, int pos);
		public void append_item(MenuItem item) {
			insert_item(item, -1);
		}
		public abstract void remove_item(MenuItem item);
	}
}
