using Gtk;

namespace Gnomenu {
	/**
	 *
	 * 	This structure is used to represent a 'background' passed from
	 * 	the panel library.
	 * 	
	 */
	public class Background {
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
	 * explicityly acts as the toplevel menubar of
	 * its logical descents.
	 *
	 * It supports changing the background,
	 * getting an xml representation of the overflown items,
	 * changing the text gravity,
	 * looking up item (and subitems) by path.
	 */
	public class MenuBar : Gtk.MenuBar, Shell {
		public MenuBar() {}
		const uint PROP_IMPORTANT = 1;
	static const string OVERFLOWER_TEMPLATE =
"""
<menu>
	<item type="a" id="_arrow_">
	%s
	</item>
</menu>
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
			disposed = false;
			_background = new Background();
			/*This is quirky. min_length should be 
			 * a 'constrcut set' property, and this 
			 * value should be set in CreateMethod,
			 * however then test-menubar will
			 * fail to build due to a vala bug in 0.5.1*/
			_min_length = -1;
			setup_overflown_item();
		}
		public override void dispose() {
			if(!disposed) {
				disposed = true;
			}
			base.dispose();
		}
		/**
		 * This signal is emitted when a child item is activated
		 */
		public signal void activate(MenuItem item);
		public virtual void emit_activate(MenuItem item) {
			if(item == _overflown_item) {
				rebuild_overflown_menu();
				return;
			} 
			if(item.is_child_of(_overflown_item)) {
				string path = overflown_path_to_path(item.item_path);
				debug("real_item is %s", path);
				MenuItem real_item = get(path);
				real_item.activate();
				return;
			}
			debug("item %s activated", item.item_path);
			activate(item);
		}
		public signal void select(MenuItem item);
		public virtual void emit_select(MenuItem item) {
			if(item == _overflown_item) {
				return;
			}
			if(item.is_child_of(_overflown_item)) {
				string path = overflown_path_to_path(item.item_path);
				debug("real_item is %s", path);
				MenuItem real_item = get(path);
				select(real_item);
				return;
			}
			debug("item %s selected", item.item_path);
			select(item);
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
							RcStyle rc_style = new RcStyle();
							modify_style(rc_style);
						}
					break;
					case BackgroundType.COLOR:
						if(old_type != _background.type
						|| (old_type == _background.type
						&& !old_color.equal(_background.color))) {
							modify_bg(StateType.NORMAL, _background.color);
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
				foreach(weak Widget child in get_children()) {
					(child as MenuItem).gravity = value;
				}
				_overflown_item.gravity = value;
			}
		}
		/**
		 * return true if the menubar has overflown items.
		 */
		public bool overflown {
			get { 
				switch(pack_direction) {
					case PackDirection.TTB:
					case PackDirection.BTT:
						return allocation.height < real_requisition.height;
					case PackDirection.LTR:
					case PackDirection.RTL:
					default:
						return allocation.width < real_requisition.width;
				}
			}
		}
		/* minimal length of the menubar.
		 * if >= 0,
		 * the menu bar will report this length to 
		 * either width or height in ::request
		 *
		 * Notice that initally this value is <0,
		 * to avoid a recursion of new MenuBar();
		 * */
		public int min_length {
			get { return _min_length;}
		   	set {
				_min_length = value;
				queue_resize();
			}
		}
		
		/**
		 * Look up a child item from a path.
		 * The path is a string constructed by two parts:
		 *
		 * [rev:]/id/id/id
		 *
		 * where rev: is an integer stamp, and id can be either
		 * the id property or the position of the menu item.
		 *
		 * return a strong reference of the menu item if found;
		 * null if not.
		 */
		public new MenuItem? get(string path) {
			string[] tokens = path.split_set("/", -1);
			tokens.length = (int) strv_length(tokens);
			Shell shell = this;
			/*
			weak string rev = tokens[0];
			FIXME: check rev */
			for(int i = 1; i < tokens.length; i++) {
				weak string token = tokens[i];
				List<weak Widget> children = (shell as Gtk.MenuShell).get_children();
				MenuItem item = null;
				foreach(weak Widget child in children) {
					MenuItem child_item = child as MenuItem;
					if(child_item != null) {
						if(child_item.item_id == token
						|| (child_item.item_id == null && 
							child_item.item_position.to_string() == token)) {
							item = child_item;
							break;
						}
					}
				}
				if(i == tokens.length - 1 /*last token, maybe found*/) return item;	
				if(item == null /*intermediate item is not found*/) return null; 
				shell = item.submenu as Shell;
				if(shell == null /*intermediate menu is not found*/) return null;
			}
			return null;
		}

/* Private variables */
		/**
		 * Holding the background object
		 */
		private Background _background;
		/**
		 * Storing the text gravity
		 */
		private Gravity _gravity;

		private MenuItem _overflown_item = new Gnomenu.MenuItem();

		private int _min_length;

		private Requisition real_requisition;

		private bool disposed;

		public override void style_set(Style? old_style) {
			base.style_set(old_style);
			_overflown_item.style = this.style;
			reset_bg_pixmap();
		}

		private void setup_overflown_item() {
			_overflown_item.set_parent(this);
			_overflown_item.style = style;
			_overflown_item.has_sub_shell = true;
			_overflown_item.visible = true;
			_overflown_item.item_id = "_arrow_";
			_overflown_item.item_type = ItemType.ARROW;
			Gnomenu.Shell shell = _overflown_item.sub_shell;
			try {
				Parser.parse(shell ,OVERFLOWER_TEMPLATE.printf("<menu/>"));
			} catch(GLib.Error e) {
				warning("%s", e.message);
			}
		}
		private void rebuild_overflown_menu() {
			debug("rebuild_overflown_menu");
			Gnomenu.Shell shell = _overflown_item.sub_shell;
			StringBuilder sb = new StringBuilder("");
			sb.append(Serializer.to_string(this));
			try {
				Parser.parse(shell, sb.str);
			} catch(GLib.Error e) {
				warning("%s", e.message);
			}

			for(int i = 0; i < this.length; i++) {
				Item item = this.get_item(i);
				Item proxy_item = shell.get_item(i);

				Widget child = item as Widget;
				proxy_item.item_visible = item.item_visible && child_need_overflown_item(child);
			}
		}
		private bool child_need_overflown_item(Gtk.Widget child) {
			int lhs = 0;
			int rhs = 0;
			Allocation a = child.allocation;
			Allocation oa = _overflown_item.allocation;
			switch(pack_direction) {
				case PackDirection.LTR:
					lhs = a.x + a.width;
					rhs = allocation.width - oa.width;

				break;
				case PackDirection.RTL:
					lhs = 0 + oa.width;
					rhs = a.x;
				break;
				case PackDirection.BTT:
					lhs = 0 + oa.height;
					rhs = a.y;
				break;
				case PackDirection.TTB:
					lhs = a.y + a.height;
					rhs = allocation.height - oa.height;
				break;
			}
			return lhs > rhs;
		}
		private void reset_bg_pixmap() {
			if(background.type != BackgroundType.PIXMAP) return;
			if(0 != (get_flags() & WidgetFlags.REALIZED)) {
				Gdk.Pixmap pixmap = new Gdk.Pixmap(window, allocation.width, allocation.height, -1);
				assert(window is Gdk.Drawable);
				assert(pixmap is Gdk.Drawable);
				Cairo.Context cairo = Gdk.cairo_create(pixmap);
				assert(cairo != null);
				assert(_background.pixmap is Gdk.Drawable);
				Gdk.cairo_set_source_pixmap(cairo, _background.pixmap, 
						-(_background.offset_x), 
						-(_background.offset_y));
				weak Cairo.Pattern pattern = cairo.get_source();
				pattern.set_extend(Cairo.Extend.REPEAT);
				cairo.rectangle (0, 0, allocation.width, allocation.height);
				cairo.fill();
				style.bg_pixmap[(int)StateType.NORMAL] = pixmap;
				style.attach(window);
				style.set_background(window, StateType.NORMAL);
				queue_draw();
			}
		}
		public override bool move_selected(int distance) {
			if(active_menu_item == _overflown_item) {
				if(distance == 1) {
					select_first(true);
				} else {
					for(int i = this.length - 1; i >= 0; i--) {
						Item item = this.get_item(i);

						Widget child = item as Widget;
						if(!child_need_overflown_item(child)) {
							select_item(child);
						}
					}
				}
				return true;
			} else {

				/*FIXME:
				 * Move from the last visible item to _overflown_item
				 * or from the first visible item to _overflown_item
				 */

				return base.move_selected(distance);
			}
		}
		public override void forall(bool include_internals, Gtk.Callback callback) {
			if(include_internals) {
				callback(_overflown_item);
			}
			base.forall(include_internals, callback);
		}
		public override void realize() {
			base.realize();
			reset_bg_pixmap();
		}
		public override void map() {
			base.map();
		}
		public override void size_allocate(Gdk.Rectangle a) {
			bool need_reset_bg_pixmap = false;
			int delta_x = a.x - allocation.x;
			int delta_y = a.y - allocation.y;
			if(delta_x != 0 || delta_y != 0
					|| a.width != allocation.width
					|| a.height != allocation.height)
				need_reset_bg_pixmap = true;
			
			background.offset_x += delta_x; 
			background.offset_y += delta_y;
			
			allocation = (Allocation) a; /*To make 'overflown' happy*/

			Gdk.Rectangle oa = {0, 0, 0, 0};
			Requisition or;
			base.size_allocate(a);

			_overflown_item.get_child_requisition(out or);
			switch(pack_direction) {
				case PackDirection.TTB:
					oa.height = or.height;
					oa.width = a.width;
					oa.x = 0;
					oa.y = a.height - oa.height;
					break;
				case PackDirection.BTT:
					oa.height = or.height;
					oa.width = a.width;
					oa.x = 0; 
					oa.y = 0;
					break;
				case PackDirection.RTL:
					oa.height = a.height;
					oa.width = or.width;
					oa.x = 0;
					oa.y = 0;
					break;
				default:
				case PackDirection.LTR:
					oa.width = or.width;
					oa.height = a.height;
					oa.x = a.width - oa.width;
					oa.y = 0;
					break;
			}
			_overflown_item.size_allocate(oa);

			if(overflown) {
				_overflown_item.set_child_visible(true);
				for(int i = 0; i < this.length; i++) {
					Item item = this.get_item(i);

					Widget child = item as Widget;
					child.set_child_visible(!child_need_overflown_item(child));
				}
			} else {
				_overflown_item.set_child_visible(false);
				for(int i = 0; i < this.length; i++) {
					Item item = this.get_item(i);

					Widget child = item as Widget;
					child.set_child_visible(true);
				}
			}
			if(need_reset_bg_pixmap) {
				reset_bg_pixmap();
			}
		}
		public override bool expose_event(Gdk.EventExpose event) {
			if((get_flags() & WidgetFlags.HAS_FOCUS) != 0) {
				Gtk.paint_focus(style,
						window,
						(Gtk.StateType)state,
						null,
						this,
						"menubar-applet",
						0, 0, -1, -1);
			}
			foreach(weak Widget child in get_children()) {
				propagate_expose(child, event);
			}
			propagate_expose(_overflown_item, event);
			return false;
		}
		public override bool focus_out_event (Gdk.EventFocus event) {
			queue_draw();
			return false;
		}
		public override bool focus_in_event (Gdk.EventFocus event) {
			queue_draw();
			return false;
		}
		public override void size_request(out Requisition req) {
			base.size_request(out real_requisition);
			req = real_requisition;
			if(min_length >= 0) {
				Requisition r = {0, 0};
				if(_overflown_item!= null) {
					_overflown_item.size_request(out r);
				}
				switch(pack_direction) {
					case PackDirection.TTB:
					case PackDirection.BTT:
						req.height = min_length>r.height? min_length: r.height;
					break;
					case PackDirection.LTR:
					case PackDirection.RTL:
					default:
						req.width = min_length>r.width?min_length:r.width;
					break;
				}
			}
		}
		public override void insert(Widget child, int position) {
			base.insert(child, position);
			(child as MenuItem).gravity = gravity;
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
			foreach(weak Widget child in get_children()) {
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
				return gtk_menu_shell_length_without_truncated(this);
			}
			set {
				gtk_menu_shell_truncate(this, value);
			}	
		}
	}
}
