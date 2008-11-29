
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
					replace(new Label(""));
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
		private Gravity _gravity;
		public Gravity gravity {
			get {
				return _gravity;
			}
			set {
				_gravity = value;
				update_label_gravity();
			}
		}
		private Widget? replace(Widget new_child) {
			Widget old_child = get_child();
			if(old_child != null) {
				remove(old_child);
			}
			add(new_child);
			return old_child;
		}
		private void update_label_gravity() {
			double text_angle = 0;
			switch(gravity) {
				case Gravity.UP:
					text_angle = 180;
				break;
				case Gravity.DOWN:
					text_angle = 0;
				break;
				case Gravity.LEFT:
					text_angle = 270;
				break;
				case Gravity.RIGHT:
					text_angle = 90;
				break;
			}
			Label label = get_child() as Label;
			if(label != null) {
				label.angle = text_angle;
			}
		}
		private override void add(Gtk.Widget widget) {
			base.add(widget);
			if(widget is Label) {
				update_label_gravity();
			}
		
		}
	}
}
