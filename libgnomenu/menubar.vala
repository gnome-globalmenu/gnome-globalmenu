using Gtk;

namespace Gnomenu {
	public class Background {
		public BackgroundType type;
		public Gdk.Pixmap pixmap;
		public Gdk.Color color;
		public int offset_x;
		public int offset_y;
		public Background clone() {
			Background rt = new Background();
			rt.type = type;
			rt.pixmap = pixmap;
			rt.color = color;
			rt.offset_x = offset_x;
			rt.offset_y = offset_y;
			return rt;
		}
	}
	public class MenuBar : Gtk.MenuBar {
		public MenuBar() {}
		construct {
			_background = new Background();
			overflow_button = new ToggleButton();
			overflow_button.set_parent(this);
		}
		public signal void activate(MenuItem item);

		public Background background {
			get {
				return _background;
			}
			set {
				BackgroundType old_type = _background.type;
				Gdk.Color old_color = _background.color;
				_background.type = value.type;
				_background.pixmap = value.pixmap;
				_background.color = value.color;
				_background.offset_x = value.offset_x;
				_background.offset_y = value.offset_y;
				switch(_background.type) {
					case BackgroundType.NONE:
						if(old_type != _background.type) {
							style = null;
							RcStyle rc_style = new RcStyle();
							modify_style(rc_style);
						}
					break;
					case BackgroundType.COLOR:
						if(old_type != _background.type
						|| (old_type == _background.type
						&& old_color.to_string() !=
							_background.color.to_string())) {
							modify_bg(StateType.NORMAL, _background.color);
						}
					break;
					case BackgroundType.PIXMAP:
						reset_bg_pixmap();
						queue_draw();
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
		public bool overflow {
			get { return _overflow; }
			set { _overflow = value; }
		}
		private Background _background;
		private Gravity _gravity;
		private bool _overflow;
		private ToggleButton overflow_button;
		private Requisition internal_requisition;

		private void reset_bg_pixmap() {
			if(background.type != BackgroundType.PIXMAP) return;
			if(0 != (get_flags() & WidgetFlags.REALIZED)) {
				Gdk.Pixmap pixmap = new Gdk.Pixmap(window, allocation.width, allocation.height, -1);
				assert(window is Gdk.Drawable);
				assert(pixmap is Gdk.Drawable);
				Cairo.Context cairo = Gdk.cairo_create(pixmap);
				assert(cairo != null);
				assert(_background.pixmap is Gdk.Drawable);
				Gdk.cairo_set_source_pixmap(cairo, _background.pixmap, -_background.offset_x, -background.offset_y);
				cairo.rectangle (0, 0, allocation.width, allocation.height);
				cairo.fill();
				/*style = */style.copy();
				style.bg_pixmap[(int)StateType.NORMAL] = pixmap;
				style.attach(window);
				style.set_background(window, StateType.NORMAL);
			}
		}
		private override void realize() {
			base.realize();
			reset_bg_pixmap();
		}
		private override void size_allocate(Gdk.Rectangle a) {
			bool need_reset_bg_pixmap = false;
			int delta_x = allocation.x - a.x;
			int delta_y = allocation.y - a.y;
			Gdk.Rectangle oba;

			Requisition obr;
			overflow_button.get_child_requisition(out obr);

			if(delta_x != 0 || delta_y != 0
					|| a.width != allocation.width
					|| a.height != allocation.height)
				need_reset_bg_pixmap = true;
			
			background.offset_x += delta_x;
			background.offset_y += delta_y;
			switch(pack_direction) {
				case PackDirection.TTB:
					if(a.height < internal_requisition.height) {
						a.height -= obr.height;
						oba.width = a.width;
						oba.height = obr.height;
						oba.y = a.height;
						oba.x = 0;
						overflow_button.visible = true;
					} else {
						overflow_button.visible = false;
					}
				break;
				case PackDirection.BTT:
					if(a.height < internal_requisition.height) {
						a.y += obr.height;
						a.height -= obr.height;
						oba.width = a.width;
						oba.height = obr.height;
						oba.x = 0;
						oba.y = 0;
						overflow_button.visible = true;
					} else {
						overflow_button.visible = false;
					}
				break;
				case PackDirection.LTR:
					if(a.width < internal_requisition.width) {
						a.width -= obr.width;
						oba.height = a.height;
						oba.width = obr.width;
						oba.x = a.width;
						oba.y = 0;
						overflow_button.visible = true;
					} else {
						overflow_button.visible = false;
					}
				break;
				case PackDirection.RTL:
					if(a.width < internal_requisition.width) {
						a.x += obr.width;
						a.width -= obr.width;
						oba.height = a.height;
						oba.width = obr.width;
						oba.x = 0;
						oba.y = 0;
						overflow_button.visible = true;
					} else {
						overflow_button.visible = false;
					}
				break;
			}
			base.size_allocate(a);
			overflow_button.size_allocate(oba);
			if(need_reset_bg_pixmap) {
				reset_bg_pixmap();
			}
		}
		private override void size_request(out Requisition req) {
			base.size_request(out internal_requisition);
			if(overflow) {
				overflow_button.size_request(out req);
			} else {
				req = internal_requisition;
			}
		}
		private override void insert(Widget child, int position) {
			base.insert(child, position);
			(child as MenuItem).gravity = gravity;
		}
		public void remove_all() {
			((MenuShellHelper)this).truncate(0);
		}	
		private override void forall(Gtk.Callback callback, void* data) {
			/* NOTE: this function is patched. see patch.sh*/
			bool include_internals = false;
			if(include_internals) {
				callback(overflow_button);
			}
			base.forall(callback, data);
		}
	}
}
