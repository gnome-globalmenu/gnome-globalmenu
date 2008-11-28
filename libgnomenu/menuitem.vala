
using Gtk;

namespace Gnomenu {
	public class MenuItem : Gtk.MenuItem {
		public MenuItem() {}
		public void set_label(string? text) {
			weak Widget bin_child = get_child();
			weak Label label_widget;
			if(!(bin_child is Gtk.Label)) {
				if(bin_child != null)
					remove(bin_child);
				add(new Label(""));
				bin_child = get_child();
			}

			label_widget = bin_child as Label;
			if(text == null) {
				text = "";
			}
				label_widget.label = text;
		}
		public weak string? get_label() {
			weak Widget bin_child = get_child();
			if(!(bin_child is Label)) return null;
			weak string text = (bin_child as Label).label;
			if(text == "") return null;
			return text;
		}
		/* return: beginning with "/" if a menu bar is found. Not if not found.*/
		public string get_path() {
			string path = "";
			MenuItem item = this;
			MenuShell parent = item.parent as MenuShell;
			path = get_position().to_string();

			while(!(parent is MenuBar)) {
				item = (parent as Menu).get_attach_widget() as MenuItem;
				if(item == null) break;
				path = item.get_position().to_string() + "/" + path;
				parent = item.parent as MenuShell;
				if(parent == null) break;
			}
			if(parent is MenuBar) {
				path = "/" + path;
			}
			return path;
		}

		public void set_position(int position) {
			set_data("position", (void*)position);
		}
		public int get_position() {
			return (int)get_data("position");
		}
	}
}
