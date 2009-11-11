/**
 *
 * 	This structure is used to represent a 'background' passed from
 * 	the panel library.
 * 	
 */
public class Gnomenu.Background {
	public BackgroundType type;
	public Gdk.Pixmap pixmap;
	public Gdk.Color color;
	/**
	 * the offset in x direction for clipping
	 * the pixmap background
	 */
	public int offset_x;
	/**
	 * the offset in y direction for clipping
	 * the pixmap background
	 */
	public int offset_y;
	/**
	 * @return a new backround which is identical to this one.
	 */
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

/**
 * A fancy menubar used by GlobalMenu.PanelApplet;
 * Not the same as Gtk.MenuBar, Gnomenu.MenuBar also
 * explicityly acts as the topmost menubar of
 * its logical descents.
 *
 * It supports changing the background,
 * getting an xml representation of the overflown items,
 * changing the text gravity,
 * looking up item (and subitems) by path.
 */
public class Gnomenu.MenuBar : Gtk.MenuBar, Shell {
	public MenuBar() {}
	const uint PROP_IMPORTANT = 1;
static const string EMPTY_OVERFLOWN_MENU =
"""
<menu/>
""";
	static construct {
		/*FIXME: this is not awared yet
		  override set_child_property
		  get_child_property,
		  gonna be messy
		install_child_property (
				PROP_IMPORTANT,
				new ParamSpecBoolean("important", 
					"Imporant",
					"Whether this item should not be overflown if possible",
					false,
					ParamFlags.READABLE | ParamFlags.WRITABLE)
				);
		*/
	
	}
	construct {
		setup_overflown_arrow();
	}

	public override void dispose() {
		if(!disposed) {
			disposed = true;
			gtk_menu_shell_remove_all(this);
		}
		base.dispose();
	}

	private MenuItem resolve_item_maybe_from_overflown(MenuItem item) {
		if(item.is_child_of(_overflown_arrow)) {
			string path = overflown_path_to_path(item.item_path);
			debug("real_item is %s", path);
			MenuItem real_item = get(path);
			return real_item;
		}
		return item;
	}
	/**
	 * emit an activate signal if needed.
	 */
	internal void emit_activate(MenuItem item) {
		if(item == _overflown_arrow) {
			rebuild_overflown_menu();
			return;
		}
		debug("item %s activated", item.item_path);
		activate(resolve_item_maybe_from_overflown(item));
	}

	internal void emit_select(MenuItem item) {
		if(item == _overflown_arrow) {
			return;
		}
		debug("item %s selected", item.item_path);
		select(resolve_item_maybe_from_overflown(item));
	}
	internal void emit_deselect(MenuItem item) {
		if(item == _overflown_arrow) {
			return;
		}
		deselect(resolve_item_maybe_from_overflown(item));
	}

	private string? overflown_path_to_path(string path) {
		int slashes = 0;
		StringBuilder sb = new StringBuilder("");
		/***
		 * path = "00001234:/0/1/234/512";
		 * sb =   "00001234:  /1/234/512";
		 */
		for(int i = 0; i < path.length; i++) {
			if( path[i] == '/') {
				slashes ++;
			}
			if(slashes != 1) 
				sb.append_unichar(path[i]);
		}
		if(slashes > 1) 
			return sb.str;
		return null;
	}

	/**
	 * To change the background of the menubar,
	 * set this property.
	 *
	 * Notice that this property doesn't increase
	 * the reference of the passed background object.
	 * rather, it sets up the menubar background
	 * according the passed value.
	 *
	 * therefore this property is a bad property.
	 * It should actually be replace by
	 * set_background and get_background.
	 */
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
						Gtk.RcStyle rc_style = new Gtk.RcStyle();
						modify_style(rc_style);
					}
				break;
				case BackgroundType.COLOR:
					if(old_type != _background.type
					|| (old_type == _background.type
					&& !old_color.equal(_background.color))) {
						modify_bg(Gtk.StateType.NORMAL, _background.color);
					}
				break;
				case BackgroundType.PIXMAP:
					reset_bg_pixmap();
				break;
			}
		}
	}
	/**
	 * The text gravity of the menubar.
	 * It also affects the text gravity of
	 * the menu items in this menubar.
	 */
	public Gravity gravity {
		get { return _gravity; }
		set {
			if(_gravity == value) return;
			_gravity = value;
			foreach(var child in get_children()) {
				(child as MenuItem).gravity = value;
			}
			_overflown_arrow.gravity = value;
		}
	}
	/**
	 * return true if the menubar has overflown items.
	 */
	public bool overflown {
		get { 
			switch(pack_direction) {
				case Gtk.PackDirection.TTB:
				case Gtk.PackDirection.BTT:
					return allocation.height < requisition.height;
				case Gtk.PackDirection.LTR:
				case Gtk.PackDirection.RTL:
				default:
					return allocation.width < requisition.width;
			}
		}
	}
	
	public new MenuItem? get(string path) {
		return this.get_item_by_path(path) as MenuItem;
	}

/* Private variables */
	/**
	 * Holding the background object
	 */
	private Background _background = new Background();
	/**
	 * Storing the text gravity
	 */
	private Gravity _gravity;

	private MenuItem _overflown_arrow = new Gnomenu.MenuItem();

	private bool disposed = false;

	private void setup_overflown_arrow() {
		/* this function is invoked by construct */
		_overflown_arrow.set_parent(this);
		_overflown_arrow.style = style;
		_overflown_arrow.has_sub_shell = true;
		_overflown_arrow.visible = true;
		_overflown_arrow.item_id = "_arrow_";
		_overflown_arrow.item_type = ItemType.ARROW;
		Gnomenu.Shell shell = _overflown_arrow.sub_shell;
		try {
			Parser.parse(shell , EMPTY_OVERFLOWN_MENU);
		} catch(GLib.Error e) {
			warning("%s", e.message);
		}
	}
	private void rebuild_overflown_menu() {
		/* this is essentially copying the menubar to
		 * the submenu of the overflown arrow */
		debug("rebuild_overflown_menu");

		/* we first serialize this menu to the global menu xml */
		var xml = Serializer.to_string(this);

		Gnomenu.Shell shell = _overflown_arrow.sub_shell;
		try {
			Parser.parse(shell, xml);
		} catch(GLib.Error e) {
			warning("%s", e.message);
		}

		/* set the visibility of each item in overflown menu */
		for(int i = 0; i < this.length; i++) {
			var item = this.get_item(i);
			var proxy_item = shell.get_item(i);

			var child = item as Gtk.Widget;
			proxy_item.item_visible = item.item_visible && child_need_overflown_arrow(child);
		}
	}

	private bool child_need_overflown_arrow(Gtk.Widget child) {
		int lhs = 0;
		int rhs = 0;
		var a = child.allocation;
		var oa = _overflown_arrow.allocation;
		switch(pack_direction) {
			case Gtk.PackDirection.LTR:
				lhs = a.x + a.width;
				rhs = allocation.width - oa.width;
			break;
			case Gtk.PackDirection.RTL:
				lhs = 0 + oa.width;
				rhs = a.x;
			break;
			case Gtk.PackDirection.BTT:
				lhs = 0 + oa.height;
				rhs = a.y;
			break;
			case Gtk.PackDirection.TTB:
				lhs = a.y + a.height;
				rhs = allocation.height - oa.height;
			break;
		}
		return lhs > rhs;
	}

	private void reset_bg_pixmap() {
		if(background.type != BackgroundType.PIXMAP) return;
		if(!this.is_realized()) return;

		assert(window is Gdk.Drawable);
		assert(_background.pixmap is Gdk.Drawable);

		/* Create the target pixmap */
		Gdk.Pixmap pixmap = new Gdk.Pixmap(window, allocation.width, allocation.height, -1);
		assert(pixmap is Gdk.Drawable);
		/* and the cairo context */
		Cairo.Context cairo = Gdk.cairo_create(pixmap);
		assert(cairo != null);
		/* copy the pixmap from background to the target pixmap
		 * notice the offsets are negative, clipping the unwanted
		 * area */
		Gdk.cairo_set_source_pixmap(cairo, _background.pixmap,
		    - _background.offset_x,
		    - _background.offset_y);
		weak Cairo.Pattern pattern = cairo.get_source();
		pattern.set_extend(Cairo.Extend.REPEAT);
		cairo.rectangle (0, 0, allocation.width, allocation.height);
		cairo.fill();

		Gtk.Style style = this.get_style();
		/* put the new pixmap into the widget style */
		style.bg_pixmap[(int)Gtk.StateType.NORMAL] = pixmap;

		style.set_background(window, Gtk.StateType.NORMAL);
		/* we need this redraw to repaint the background */
		this.queue_draw();
	}
	public override bool move_selected(int distance) {
		if(active_menu_item == _overflown_arrow) {
			/* from overflown arrow */
			if(distance == 1) {
				select_first(true);
			} else {
				/* the last non overflown item */
				for(int i = this.length - 1; i >= 0; i--) {
					var item = this.get_item(i);

					var child = item as Gtk.Widget;
					if(!child_need_overflown_arrow(child)) {
						select_item(child);
					}
				}
			}
			return true;
		} else {

			/*FIXME:
			 * Move from the last visible item to _overflown_arrow
			 * or from the first visible item to _overflown_arrow
			 */

			return base.move_selected(distance);
		}
	}
	public override void forall_internal(bool include_internals, Gtk.Callback callback) {
		if(include_internals) {
			callback(_overflown_arrow);
		}
		base.forall_internal(include_internals, callback);
	}

	public override void realize() {
		base.realize();
		reset_bg_pixmap();
	}

	public override void size_allocate(Gdk.Rectangle a) {
		bool need_reset_bg_pixmap = false;

		/* Shift the background image */
		int delta_x = a.x - allocation.x;
		int delta_y = a.y - allocation.y;
		if(  delta_x != 0 || delta_y != 0
		  || a.width != allocation.width
		  || a.height != allocation.height) {
			need_reset_bg_pixmap = true;
		}
		background.offset_x += delta_x;
		background.offset_y += delta_y;
		
		/* size_allocate will set allocation to a */
		base.size_allocate(a);

		/* calculate the allocation of the overflown arrow */
		Gdk.Rectangle oa = {0, 0, 0, 0};
		Gtk.Requisition or;
		_overflown_arrow.get_child_requisition(out or);
		switch(pack_direction) {
			case Gtk.PackDirection.TTB:
				oa.height = or.height;
				oa.width = a.width;
				oa.x = 0;
				oa.y = a.height - oa.height;
				break;
			case Gtk.PackDirection.BTT:
				oa.height = or.height;
				oa.width = a.width;
				oa.x = 0; 
				oa.y = 0;
				break;
			case Gtk.PackDirection.RTL:
				oa.height = a.height;
				oa.width = or.width;
				oa.x = 0;
				oa.y = 0;
				break;
			default:
			case Gtk.PackDirection.LTR:
				oa.width = or.width;
				oa.height = a.height;
				oa.x = a.width - oa.width;
				oa.y = 0;
				break;
		}
		_overflown_arrow.size_allocate(oa);

		/*
		 * decide showing the overflown array or not.
		 * at this stage we already know the allocation
		 * of every item, because base.allocate has been called.
		 * */
		if(overflown) {
			_overflown_arrow.set_child_visible(true);
			for(int i = 0; i < this.length; i++) {
				var item = this.get_item(i);
				var child = item as Gtk.Widget;
				child.set_child_visible(!child_need_overflown_arrow(child));
			}
		} else {
			_overflown_arrow.set_child_visible(false);
			for(int i = 0; i < this.length; i++) {
				var item = this.get_item(i);
				var child = item as Gtk.Widget;
				child.set_child_visible(true);
			}
		}
		/* reset the bg pixmap at the very end of the process,
		 * using the new allocation. */
		if(need_reset_bg_pixmap) {
			reset_bg_pixmap();
		}
	}
	public override bool expose_event(Gdk.EventExpose event) {
		foreach(var child in get_children()) {
			propagate_expose(child, event);
		}
		propagate_expose(_overflown_arrow, event);
		return false;
	}

	public override void size_request(out Gtk.Requisition req) {
		Gtk.Requisition r = {0, 0};
		base.size_request(out req);
		_overflown_arrow.size_request(out r);
		/* the minimal requisition is the size request of the
		 * overflown arrow */
		if(r.width > req.width) req.width = r.width;
		if(r.height > req.height) req.height = r.height;
	}

	public override void insert(Gtk.Widget child, int position) {
		base.insert(child, position);
		if(child is Gnomenu.MenuItem)
		(child as Gnomenu.MenuItem).gravity = gravity;
	}
	
	/******
	 * Gnomenu.Shell interface
	 ********* */
	public Item? owner {
		get {
			return null;
		}
	}
	public Item? get_item(int position) {
		return gtk_menu_shell_get_item(this, position) as Item;
	}
	public Item? get_item_by_id(string id) {
		foreach(var child in get_children()) {
			Item item = child as Item;
			if(item == null) continue;
			if(item.item_id == id) return item;
		}
		return null;
	}
	public int get_item_position(Item item) {
		return gtk_menu_shell_get_item_position(this, item as MenuItem);
	}
	public int length {
		get {
			return gtk_menu_shell_get_length(this);
		}
		set {
			gtk_menu_shell_set_length(this, value);
		}	
	}
}
