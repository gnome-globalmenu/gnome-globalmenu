[Compact]
public class Locator {
	Locator() { }	
	public static Gtk.MenuItem? locate (Gtk.MenuBar menubar, string path) {
		string[] tokens = path.split_set("/", -1);
		tokens.length = (int) strv_length(tokens);
		Gtk.MenuShell shell = menubar;
		/*
		weak string rev = tokens[0];
		FIXME: implement the revision */
		for(int i = 1; i < tokens.length; i++) {
			weak string token = tokens[i];
			Gtk.MenuItem item = null;
			List<weak Gtk.Widget> children = shell.get_children();
			if(token.has_prefix("W")) {
				ulong pointer = token.offset(1).to_ulong();
				/*Widget Pointer*/
				foreach(var child in children) {
					if((ulong) child == pointer) {
						item = child as Gtk.MenuItem;
						break;
					}
				}
			} else {
				/*assume it is an id*/
				int position = token.to_int();
				foreach(var child in children) {
					if(child is Gtk.TearoffMenuItem) continue;
					if(position == 0) {
						item = child as Gtk.MenuItem;
						break;
					}
					position--;
				}
			}
			if( i == tokens.length - 1 /*last token in the path*/) return item;
			if(item == null) return null;
			shell = item.submenu;
			if(shell == null) return null;
		}
		return null;
	}
}

