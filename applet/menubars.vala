using GLib;
using Gtk;
using Gnomenu;

public class MenuBars: Gtk.Container {
	static const string OVERFLOWER_TEMPLATE =
"""
<menu>
	<item type="a" id="_arrow_">
	%s
	</item>
</menu>
""";
	public PackDirection pack_direction {
		get {
			return _pack_direction;
		}
		set {
			foreach(Gnomenu.MenuBar menubar in internal_children) {
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
			_gravity = value;
			foreach(Gnomenu.MenuBar menubar in internal_children) {
				menubar.gravity = value;
			}
			queue_draw();
		}
	}
	public Background background {
		set {
			Background bg = value.clone();
			foreach(Gnomenu.MenuBar menubar in internal_children) {
				bg.offset_x = menubar.allocation.x - allocation.x;
				bg.offset_y = menubar.allocation.y - allocation.y;
				menubar.background = bg;
			}
		}
	}
	public Gnomenu.MenuBar selector {
		get {
			return _selector;
		}
	}
	public Gnomenu.MenuBar menubar {
		get {
			return _menubar;
		}	
	}
	public MenuBars() {
	}
	construct {
		set_flags(WidgetFlags.NO_WINDOW);
		_selector = add_menubar();
		_menubar = add_menubar();
		_overflower = add_menubar();
		Parser.parse(_overflower, OVERFLOWER_TEMPLATE.printf("<menu/>"));
		_overflower.activate += (menubar, item) => {
			string path = item.path;
			if(item.id == "_arrow_") {
				string overflown_menu = _menubar.create_overflown_menu();
				if(overflown_menu == null) {
					overflown_menu = "<menu/>";
				}
				string overflower_context = OVERFLOWER_TEMPLATE.printf(overflown_menu);
				Parser.parse(_overflower, overflower_context);
				return;
			}
			int slashes = 0;
			StringBuilder sb = new StringBuilder("");
			/***
			 * path = "00001234:/0/1/234/512";
			 * sb =   "00001234:  /1/234/512";
			 */
			bool skip = false;
			for(int i = 0; i < path.length; i++) {
				if( path[i] == '/') {
					slashes ++;
				}
				if(slashes != 1) 
					sb.append_unichar(path[i]);
			}
			if(slashes > 1) {
				Gnomenu.MenuItem item = _menubar.get(sb.str);
				if(item != null)
					_menubar.activate(item);
				else {
					warning("MenuItem %s not found in the main menubar!", sb.str);
				}
			}
		};

	}

	private Gnomenu.MenuBar _menubar;
	private Gnomenu.MenuBar _overflower;
	private Gnomenu.MenuBar _selector;


	private PackDirection _pack_direction;
	private Gnomenu.Gravity _gravity;

	private List<weak Gnomenu.MenuBar> internal_children;

	private override void forall(Gtk.Callback cb, void* data) {
		bool include_internal;

		if(include_internal) {
			foreach(Gnomenu.MenuBar menubar in internal_children) {
				cb(menubar);
			}
		}
	}
	private override void size_request(out Requisition r) {
		r.width = 0;
		r.height = 0;
		Requisition cr;
		switch(pack_direction) {
			case PackDirection.LTR:
			case PackDirection.RTL:
				foreach(Gnomenu.MenuBar menubar in internal_children) {
					menubar.size_request(out cr);
					if(menubar != _menubar) {
						r.width += cr.width;
					}
					r.height = r.height>cr.height?r.height:cr.height;
				}
			break;
			case PackDirection.BTT:
			case PackDirection.TTB:
				foreach(Gnomenu.MenuBar menubar in internal_children) {
					menubar.size_request(out cr);
					if(menubar != _menubar) {
						r.height += cr.height;
					}
					r.width = r.width>cr.width?r.width:cr.width;
				}
			break;
		}
	}
	private override void map() {
		base.map();
		foreach(Gnomenu.MenuBar menubar in internal_children) {
			if(menubar.visible) {
				menubar.map();
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

		foreach(Gnomenu.MenuBar menubar in internal_children) {
			menubar.get_child_requisition(out cr);
			switch(pack_direction) {
				case PackDirection.LTR:
					if(menubar == _menubar) {
						ca.width = a.width - requisition.width;
					} else {
						ca.width = cr.width;
					}
					ca.height = a.height;
					ca.x = x;
					ca.y = y;
					x += ca.width;
				break;
				case PackDirection.RTL:
					if(menubar == _menubar) {
						ca.width = a.width - requisition.width;
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
					if(menubar == _menubar) {
						ca.height = a.height - requisition.height;
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
					if(menubar == _menubar) {
						ca.height = a.height - requisition.height;
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
		if(_menubar.overflown) {
			_overflower.visible = true;
		} else {
			_overflower.visible = false;
		}
		base.size_allocate(a);
	}
	private Gnomenu.MenuBar add_menubar() {
		Gnomenu.MenuBar menubar = new Gnomenu.MenuBar();
		menubar.visible = true;
		menubar.set_parent(this);

		internal_children.append(menubar);
		return menubar;
	}


}
