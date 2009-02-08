namespace Gnomenu {
	public interface Item: GLib.Object {
		public abstract Shell shell {get; set;}
		public abstract Shell sub_shell {get; set;}
		public abstract int position {get; set;}
		public abstract string? id {get; set;}

		public abstract string item_type {get; set;}
		public abstract bool use_underline {get; set;}
		public abstract string? item_state {get; set;}
		public abstract string? label {get; set;}
		public abstract string? icon {get; set;}
		public abstract string? accel_text {get; set;}
		public abstract string? font {get; set;}

		public string path_name {owned get {
			if(id != null)
				return id;
			else
				return position.to_string();
			}
		}
		public string path {owned get {
			StringBuilder sb = new StringBuilder("");
			Item item = this;
			Shell parent = item.shell;
			sb.append(path_name);
			while(parent != null) {
				item = parent.owner as Item;
				if(item == null) break;
				sb.prepend_unichar('/');
				sb.prepend(item.path_name);
				shell = item.shell;
				if(parent == null) break;
			}
			sb.prepend_unichar('/');
			return sb.str;
		}}
	}
}
