
using Gtk;

namespace Gnomenu {
	public class MenuItem : Gtk.MenuItem {
		public MenuItem() {}
		public string? label {
			get {
				weak Widget bin_child = get_child();
				if(!(bin_child is Label)) return null;
				weak string text = (bin_child as Label).label;
				if(text == "") return null;
				return text;
			}
			set {
				weak Widget bin_child = get_child();
				weak Label label_widget;
				if(!(bin_child is Gtk.Label)) {
					if(bin_child != null)
						remove(bin_child);
					add(new Label(""));
					bin_child = get_child();
					bin_child.visible = true;
				}
				label_widget = bin_child as Label;
				if(value == null) {
					value = "";
				}
				label_widget.label = value;
			}
		}
		public string path {
			get {
				/* return: beginning with "/" if a menu bar is found. Not if not found.*/
				MenuItem item = this;
				MenuShell parent = item.parent as MenuShell;
				_path = position.to_string();

				while(!(parent is MenuBar)) {
					item = (parent as Menu).get_attach_widget() as MenuItem;
					if(item == null) break;
					_path = item.position.to_string() + "/" + _path;
					parent = item.parent as MenuShell;
					if(parent == null) break;
				}
				if(parent is MenuBar) {
					_path = "/" + _path;
				}
				return _path;
			}
		}
		public int position {
			set {
				int new_pos = value;
				set_data("position", (void*)new_pos);
			}
			get {
				return (int)get_data("position");
			}
		}
		private string _path; /*merely a buffer*/
	}
}
