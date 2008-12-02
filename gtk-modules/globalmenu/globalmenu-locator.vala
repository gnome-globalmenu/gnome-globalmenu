using Gtk;

namespace GnomenuGtk {
	public class Locator {
		Locator() { }	
		public static Gtk.MenuItem? locate (MenuBar menubar, string path) {
			string[] tokens = path.split_set("/", -1);
			tokens.length = (int) strv_length(tokens);
			MenuShell shell = menubar;
			weak string rev = tokens[0];
			/*FIXME: implement the revision */
			for(int i = 1; i < tokens.length; i++) {
				weak string token = tokens[i];
				int position = token.to_int();
				weak List<weak Widget> children = shell.get_children();
				MenuItem item;
				foreach(weak Widget child in children) {
					if(child is TearoffMenuItem) continue;
					if(position == 0) {
						item = child as MenuItem;
						break;
					}
					position--;
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
