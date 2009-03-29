using Gtk;

namespace GlobalMenuGTK {
	[Compact]
	public class Locator {
		Locator() { }	
		public static Gtk.MenuItem? locate (MenuBar menubar, string path) {
			string[] tokens = path.split_set("/", -1);
			tokens.length = (int) strv_length(tokens);
			MenuShell shell = menubar;
			/*
			weak string rev = tokens[0];
			FIXME: implement the revision */
			for(int i = 1; i < tokens.length; i++) {
				weak string token = tokens[i];
				MenuItem item = null;
				List<weak Widget> children = shell.get_children();
				if(token.has_prefix("W")) {
					ulong pointer = token.offset(1).to_ulong();
					/*Widget Pointer*/
					foreach(weak Widget child in children) {
						if((ulong) child == pointer) {
							item = child as MenuItem;
							break;
						}
					}
				} else {
					/*assume it is an id*/
					int position = token.to_int();
					foreach(weak Widget child in children) {
						if(child is TearoffMenuItem) continue;
						if(position == 0) {
							item = child as MenuItem;
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


}
