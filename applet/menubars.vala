using GLib;
using Gtk;
using Gnomenu;

private struct ChildPropBag {
	public bool expand;
}

public class MenuBars: Gtk.Container {
	static const int PROP_EXPAND = 1234;
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
	}
	public PackDirection pack_direction {
		get {
			return _pack_direction;
		}
		set {
			if(_pack_direction == value) return;
			foreach(Gnomenu.MenuBar menubar in children) {
				menubar.pack_direction = value;
				menubar.child_pack_direction = value;
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
	public MenuBars() {
	}
	construct {
		set_flags(WidgetFlags.NO_WINDOW);
		props = new HashTable<weak Widget, ChildPropBag*>.full(direct_hash, direct_equal,
				null, free);
	}

	private Gnomenu.MenuBar _menubar;
	private Gnomenu.MenuBar _selector;

	private HashTable<weak Widget, ChildPropBag*> props;
	private PackDirection _pack_direction;
	private Gnomenu.Gravity _gravity;

	private List<weak Gnomenu.MenuBar> children;

	private override void forall(Gtk.Callback cb, void* data) {
		bool include_internal;

		if(include_internal) {

		}
		foreach(Gnomenu.MenuBar menubar in children) {
			cb(menubar);
		}
	}
	public override void add(Widget child) {
		if(child is Gnomenu.MenuBar) {
			children.append(child as Gnomenu.MenuBar);
			child.set_parent(this);
			props.insert(child, (ChildPropBag*)malloc(sizeof(ChildPropBag)));
			(child as Gnomenu.MenuBar).pack_direction = pack_direction;
			(child as Gnomenu.MenuBar).gravity = gravity;
			child_set(child, "expand", false, null);
			child.ref();
		}
	}
	public override void remove(Widget child) {
		if(child is Gnomenu.MenuBar) {
			children.remove_all(child as Gnomenu.MenuBar);
			child.unparent();
			props.remove(child);
			child.unref();
		}
	}
	private override void size_request(out Requisition r) {
		r.width = 0;
		r.height = 0;
		Requisition cr;
		switch(pack_direction) {
			case PackDirection.LTR:
			case PackDirection.RTL:
				foreach(Gnomenu.MenuBar menubar in children) {
					menubar.size_request(out cr);
					r.height = r.height>cr.height?r.height:cr.height;
					r.width += cr.width;
				}
			break;
			case PackDirection.BTT:
			case PackDirection.TTB:
				foreach(Gnomenu.MenuBar menubar in children) {
					menubar.size_request(out cr);
					r.width = r.width>cr.width?r.width:cr.width;
					r.height += cr.height;
				}
			break;
		}
	}
	private override void map() {
		base.map();
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

		int num_of_expands = 0;
		int non_expand_a = 0;

		foreach(Gnomenu.MenuBar menubar in children) {
			bool expand;
			child_get(menubar, "expand", &expand, null);
			if(expand) num_of_expands++;
			else {
				menubar.get_child_requisition(out cr);
				switch(pack_direction) {
					case PackDirection.LTR:
					case PackDirection.RTL:
						non_expand_a += cr.width;
					break;
					case PackDirection.BTT:
					case PackDirection.TTB:
						non_expand_a += cr.height;
					break;
				}
			}
		}
		foreach(Gnomenu.MenuBar menubar in children) {
			bool expand;
			menubar.get_child_requisition(out cr);
			child_get(menubar, "expand", &expand, null);
			switch(pack_direction) {
				case PackDirection.LTR:
					if(expand) {
						ca.width = (a.width - non_expand_a)/num_of_expands;
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
					if(expand) {
						ca.width = (a.width - non_expand_a)/num_of_expands;
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
				case PackDirection.BTT:
					ca.width = a.width;
					if(expand) {
						ca.height = (a.height - non_expand_a)/num_of_expands;
						if(ca.height < 0) ca.height = 0;
					} else {
						ca.height = cr.height;
					}
					ca.x = x;
					ca.y = rev_y - ca.height;
					rev_y -= ca.height;
					y += ca.height;
				break;
				case PackDirection.TTB:
					ca.width = a.width;
					if(expand) {
						ca.height = (a.height - non_expand_a)/num_of_expands;
						if(ca.height < 0) ca.height = 0;
					} else {
						ca.height = cr.height;
					}
					ca.x = x;
					ca.y = y;
					y += ca.height;
				break;
			}
			menubar.size_allocate((Gdk.Rectangle)ca);
		}
		base.size_allocate(a);
	}
	private override void get_child_property(Gtk.Widget child, uint id,
			Value value, ParamSpec pspec) {
		switch(id) {
			case PROP_EXPAND:
				ChildPropBag* prop = props.lookup(child);
				value.set_boolean(prop->expand);
			break;
		}
	}
	private override void set_child_property(Gtk.Widget child, uint id,
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
		}
	
	}

}
