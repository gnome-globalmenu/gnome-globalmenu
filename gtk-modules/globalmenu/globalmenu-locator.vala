using Gtk;

namespace GnomenuGtk {
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
				int position = token.to_int();
				List<weak Widget> children = gtk_container_get_children(shell);
				MenuItem item = null;
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
