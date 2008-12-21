using Gtk;
Don't use this file.
/*****
 * Deperacated
 * ******/
namespace Gnomenu {
	public class Box: Gtk.Box {
		public Box(){ }
		public PackDirection pack_direction {
			get {
				return _pack_direction;
			}
			set {
				if(_pack_direction == value) return;
				foreach(Widget child in gtk_container_get_children(this)) {
					if(child is MenuBar) {
						(child as MenuBar).pack_direction = value;
						(child as MenuBar).child_pack_direction = value;
					}
				}
				_pack_direction = value;
				queue_resize();
			}
		}
		public Gnomenu.Gravity gravity {
			get {
				return _gravity;
			}
			set {
				_gravity = value;
				foreach(Widget child in gtk_container_get_children(this)) {
					if(child is MenuBar) 
						(child as MenuBar).gravity = value;
				}
				queue_draw();
			}
		}
		public Background background {
			set {
				Background bg = value.clone();
				foreach(Widget child in gtk_container_get_children(this)) {
					if(child is MenuBar) {
						bg.offset_x = child.allocation.x - allocation.x;
						bg.offset_y = child.allocation.y - allocation.y;
						(child as MenuBar).background = bg;
					}
				}
			}
		}
		construct {
			set_flags(WidgetFlags.NO_WINDOW);
		}


		private PackDirection _pack_direction;
		private Gnomenu.Gravity _gravity;

		private override void size_request(out Requisition r) {
			r.width = 0;
			r.height = 0;
			Requisition cr;
			switch(pack_direction) {
				case PackDirection.LTR:
				case PackDirection.RTL:
					foreach(Widget child in gtk_container_get_children(this)) {
						child.size_request(out cr);
						r.width += cr.width;
						r.height = r.height>cr.height?r.height:cr.height;
					}
				break;
				case PackDirection.BTT:
				case PackDirection.TTB:
					foreach(Widget child in gtk_container_get_children(this)) {
						child.size_request(out cr);
						r.height += cr.height;
						r.width = r.width>cr.width?r.width:cr.width;
					}
				break;
			}
		}
		private override void map() {
			base.map();
			foreach(Widget child in gtk_container_get_children(this)) {
				if(child.visible) {
					child.map();
				}
			}
		}
		private override void size_allocate(Gdk.Rectangle a) {
			allocation = (Allocation) a;
			Requisition cr;
			Allocation ca;
			int x;
			int y;
			int rev_x;
			int rev_y;
			x = 0;
			y = 0;
			rev_x = a.width;
			rev_y = a.height;

			foreach(Widget child in gtk_container_get_children(this)) {
				child.get_child_requisition(out cr);
				switch(pack_direction) {
					case PackDirection.LTR:
						ca.width = cr.width;
						ca.height = a.height;
						ca.x = x;
						ca.y = y;
						x += ca.width;
					break;
					case PackDirection.RTL:
						ca.width = cr.width;
						ca.x = rev_x - ca.width;
						ca.y = y;
						ca.height = a.height;
						rev_x -= ca.width;
						x += ca.width;
					break;
					case PackDirection.BTT:
						ca.width = a.width;
						ca.height = cr.height;
						ca.x = x;
						ca.y = rev_y - ca.height;
						rev_y -= ca.height;
						y += ca.height;
					break;
					case PackDirection.TTB:
						ca.width = a.width;
						ca.height = cr.height;
						ca.x = x;
						ca.y = y;
						y += ca.height;
					break;
				}
				child.size_allocate((Gdk.Rectangle)ca);
			}
		}
	}
}
