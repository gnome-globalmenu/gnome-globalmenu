using Gtk;

namespace Gnomenu {
	public enum BackgroundType {
		NONE,
		COLOR,
		PIXMAP
	}
	public enum Gravity { /*Text Gravity, differ from Gdk.Gravity*/
		DOWN, 
		UP, /*Rarely used: up-side-down!*/
		LEFT,
		RIGHT
	}
	public class Background {
		public BackgroundType type;
		public Gdk.Pixmap pixmap;
		public Gdk.Color color;
		public Background clone() {
			Background rt = new Background();
			rt.type = type;
			rt.pixmap = pixmap;
			rt.color = color;
			return rt;
		}
	}
	public class MenuBar : Gtk.MenuBar {
		public MenuBar() {}
		construct {
			_background = new Background();
		}
		public signal void activate(MenuItem item);

		public Background background {
			get {
				return _background;
			}
			set {
				_background.type = value.type;
				_background.pixmap = value.pixmap;
				_background.color = value.color;
				switch(_background.type) {
					case BackgroundType.NONE:
						style = null;
						RcStyle rc_style = new RcStyle();
						modify_style(rc_style);
					break;
					case BackgroundType.COLOR:
						modify_bg(StateType.NORMAL, _background.color);
					break;
					case BackgroundType.PIXMAP:
					break;
				}
			}
		}
		public Gravity gravity {
			get { return _gravity; }
			set {
				_gravity = value;
				foreach(weak Widget child in get_children()) {
					(child as MenuItem).gravity = value;
				}
			}
		}
		private Background _background;
		private Gravity _gravity;

		private override void insert(Widget child, int position) {
			base.insert(child, position);
			(child as MenuItem).gravity = gravity;
		}
	
	}
}
