using Gtk;

namespace Gnomenu {

public class MenuBarBox: Gtk.Container {
	private struct ChildPropBag {
		public bool expand;
		public bool shrink;
	}
	static const int PROP_EXPAND = 1234;
	static const int PROP_SHRINK = 1235;
	static construct {
		install_child_property(PROP_EXPAND,
				new ParamSpecBoolean(
					"expand",
					"Expand",
					"the child will expand if set to true",
					false,
					ParamFlags.READABLE |
					ParamFlags.WRITABLE
					));
		install_child_property(PROP_SHRINK,
				new ParamSpecBoolean(
					"shrink",
					"shrink",
					"the child will shrink if set to true",
					false,
					ParamFlags.READABLE |
					ParamFlags.WRITABLE
					));
	}
	public PackDirection pack_direction {
		/* The child menubar's child pack direction is the same
		 * as this menubox's pack direction,
		 * whilst the child menubar's pack direction is the same
		 * as this menubox's child pack direction.
		 *
		 * This is to provide a sane layout for the vertical left
		 * panel*/
		get {
			return _pack_direction;
		}
		set {
			foreach(Gnomenu.MenuBar menubar in children) {
				menubar.child_pack_direction = value;
			}
			if(_pack_direction == value) return;
			_pack_direction = value;
			queue_resize();
		}
	}
	public PackDirection child_pack_direction {
		get {
			return _child_pack_direction;
		}
		set {
			foreach(Gnomenu.MenuBar menubar in children) {
				menubar.pack_direction = value;
			}
			if(_child_pack_direction == value) return;
			_child_pack_direction = value;
			queue_resize();
		}
	}

	public Gnomenu.Gravity gravity {
		get {
			return _gravity;
		}
		set {
			if(_gravity == value) return;
			_gravity = value;
			foreach(Gnomenu.MenuBar menubar in children) {
				menubar.gravity = value;
			}
			queue_draw();
		}
	}
	public Background background {
		set {
			Background bg = value.clone();
			foreach(Gnomenu.MenuBar menubar in children) {
				bg.offset_x = menubar.allocation.x - allocation.x;
				bg.offset_y = menubar.allocation.y - allocation.y;
				menubar.background = bg;
			}
		}
	}
	public MenuBarBox() {
	}
	construct {
		set_flags(WidgetFlags.NO_WINDOW);
		props = new HashTable<weak Widget, ChildPropBag*>.full(direct_hash, direct_equal,
				null, free);
	}

	private HashTable<weak Widget, ChildPropBag*> props;
	private PackDirection _pack_direction;
	private PackDirection _child_pack_direction;
	private Gnomenu.Gravity _gravity;

	private List<weak Gnomenu.MenuBar> children;

	private int[] size_hints;
	
	public unowned int[] get_size_hints() {
		return size_hints;	
	}
	public override void forall(bool include_internals, Gtk.Callback callback) {
		if(include_internals) {

		}
		weak List<weak Gnomenu.MenuBar> iter = children;
		while(iter!=null) {
			weak Widget child = iter.data;
			iter = iter.next;
			callback(child);
		}
	}
	public override void add(Widget child) {
		if(child is Gnomenu.MenuBar) {
			children.append(child as Gnomenu.MenuBar);
			child.set_parent(this);
			props.insert(child, (ChildPropBag*)malloc0(sizeof(ChildPropBag)));
			(child as Gnomenu.MenuBar).pack_direction = pack_direction;
			(child as Gnomenu.MenuBar).gravity = gravity;
		}
	}
	public override void remove(Widget child) {
		if(child is Gnomenu.MenuBar) {
			children.remove_all(child as Gnomenu.MenuBar);
			child.unparent();
			props.remove(child);
		}
	}
	public override void size_request(out Requisition r) {
		r.width = 0;
		r.height = 0;
		Requisition cr;
		List<int> hints = null;
		foreach(Gnomenu.MenuBar menubar in children) {
			if(!menubar.visible) continue;
			menubar.size_request(out cr);
			bool shrink = false;
			child_get(menubar, "shrink", &shrink);
			switch(pack_direction) {
				case PackDirection.LTR:
				case PackDirection.RTL:
						r.height = r.height>cr.height?r.height:cr.height;
						hints.prepend(r.width + 2);
						r.width += cr.width;
						hints.prepend(r.width + 1);
				break;
				case PackDirection.BTT:
				case PackDirection.TTB:
						r.width = r.width>cr.width?r.width:cr.width;
						hints.prepend(r.height + 2);
						r.height += cr.height;
						hints.prepend(r.height + 1);
				break;
			}
		}
		size_hints = new int[hints.length()];
		int i = 0;
		foreach(int val in hints) {
			size_hints[i] = val;
			i++;
		}
	}

	public override void size_allocate(Gdk.Rectangle a) {
		allocation = (Allocation) a;
		Requisition cr;
		Allocation ca = {0, 0, 0, 0};
 		int x = 0;
		int y = 0;
		int rev_x = a.width;
		int rev_y = a.height;

		int num_of_shrinks = 0;
		int non_shrink_a = 0;

		foreach(Gnomenu.MenuBar menubar in children) {
			if(!menubar.visible) continue;
			bool shrink = false;
			child_get(menubar, "shrink", &shrink, null);
			if(shrink) num_of_shrinks++;
			else {
				menubar.get_child_requisition(out cr);
				switch(pack_direction) {
					case PackDirection.LTR:
					case PackDirection.RTL:
						non_shrink_a += cr.width;
					break;
					case PackDirection.BTT:
					case PackDirection.TTB:
						non_shrink_a += cr.height;
					break;
				}
			}
		}
		foreach(Gnomenu.MenuBar menubar in children) {
			bool shrink = false;
			if(!menubar.visible) continue;
			menubar.get_child_requisition(out cr);
			child_get(menubar, "shrink", &shrink, null);
			switch(pack_direction) {
				case PackDirection.LTR:
					if(shrink) {
						ca.width = (a.width - non_shrink_a)/num_of_shrinks;
						if(ca.width < 0) ca.width = 0;
					} else {
						ca.width = cr.width;
					}
					ca.height = a.height;
					ca.x = x;
					ca.y = y;
					x += ca.width;
				break;
				case PackDirection.RTL:
					if(shrink) {
						ca.width = (a.width - non_shrink_a)/num_of_shrinks;
						if(ca.width < 0) ca.width = 0;
					} else {
						ca.width = cr.width;
					}
					ca.x = rev_x - ca.width;
					ca.y = y;
					ca.height = a.height;
					rev_x -= ca.width;
					x += ca.width;
				break;
				case PackDirection.TTB:
					ca.width = a.width;
					if(shrink) {
						ca.height = (a.height - non_shrink_a)/num_of_shrinks;
						if(ca.height < 0) ca.height = 0;
					} else {
						ca.height = cr.height;
					}
					ca.x = x;
					ca.y = y;
					y += ca.height;
				break;
				case PackDirection.BTT:
					ca.width = a.width;
					if(shrink) {
						ca.height = (a.height - non_shrink_a)/num_of_shrinks;
						if(ca.height < 0) ca.height = 0;
					} else {
						ca.height = cr.height;
					}
					ca.x = x;
					ca.y = rev_y - ca.height;
					rev_y -= ca.height;
					y += ca.height;
				break;
			}
			menubar.size_allocate((Gdk.Rectangle)ca);
		}
		base.size_allocate(a);
	}
	public override void get_child_property(Gtk.Widget child, uint id,
			Value value, ParamSpec pspec) {
		switch(id) {
			case PROP_EXPAND:
				ChildPropBag* prop = props.lookup(child);
				value.set_boolean(prop->expand);
			break;
			case PROP_SHRINK:
				ChildPropBag* prop = props.lookup(child);
				value.set_boolean(prop->shrink);
			break;
		}
	}
	public override void set_child_property(Gtk.Widget child, uint id,
			Value value, ParamSpec pspec) {
		switch(id) {
			case PROP_EXPAND:
				bool expand = value.get_boolean();
				ChildPropBag* prop = props.lookup(child);
				if(prop->expand != expand) {
					prop->expand = expand;
					queue_resize();
				}
			break;
			case PROP_SHRINK:
				bool shrink = value.get_boolean();
				ChildPropBag* prop = props.lookup(child);
				if(prop->shrink != shrink ) {
					prop->shrink = shrink;
					queue_resize();
				}
			break;
		}
	
	}

}
}
