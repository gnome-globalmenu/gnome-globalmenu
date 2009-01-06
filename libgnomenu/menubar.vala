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
	public class MenuBar : Gtk.MenuBar {
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
		}
		public override void dispose() {
			if(!disposed) {
				disposed = true;
				if(_overflown_menubar != null) {
					_overflown_menubar.unparent();
					_overflown_menubar = null;
				}
			}
			base.dispose();
		}
		/**
		 * This signal is emitted when a child item is activated
		 */
		public signal void activate(MenuItem item);

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
							if(_overflown_menubar != null) {
								_overflown_menubar.background = value;
							}
						}
					break;
					case BackgroundType.COLOR:
						if(old_type != _background.type
						|| (old_type == _background.type
						&& old_color.to_string() !=
							_background.color.to_string())) {
							modify_bg(StateType.NORMAL, _background.color);
							if(_overflown_menubar != null) {
								_overflown_menubar.background = value;
							}
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
				foreach(weak Widget child in gtk_container_get_children(this)) {
					(child as MenuItem).gravity = value;
				}
				if(_overflown_menubar != null) {
					_overflown_menubar.gravity = value;
				}
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
				if(value >= 0 && _overflown_menubar == null) {
					_overflown_menubar = create_overflown_menubar();
					if((get_flags() & WidgetFlags.REALIZED) != 0) {
						unrealize();
						realize();
					}
				}
				if(value < 0 && _overflown_menubar != null) {
					_overflown_menubar.unparent();
					_overflown_menubar = null;
				}
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
		public MenuItem? get(string path) {
			string[] tokens = path.split_set("/", -1);
			tokens.length = (int) strv_length(tokens);
			MenuShell shell = this;
			/*
			weak string rev = tokens[0];
			FIXME: check rev */
			for(int i = 1; i < tokens.length; i++) {
				weak string token = tokens[i];
				List<weak Widget> children = gtk_container_get_children(shell);
				MenuItem item = null;
				foreach(weak Widget child in children) {
					MenuItem child_item = child as MenuItem;
					if(child_item != null) {
						if(child_item.id == token
						|| (child_item.id == null && 
							child_item.position.to_string() == token)) {
							item = child_item;
							break;
						}
					}
				}
				if(i == tokens.length - 1 /*last token, maybe found*/) return item;	
				if(item == null /*intermediate item is not found*/) return null; 
				shell = item.submenu;
				if(shell == null /*intermediate menu is not found*/) return null;
			}
			return null;
		}


		/**
		 * returns an xml representation of the overflown menubar.
		 * The difference between the returned string and the
		 * ordinary xml representation is that 
		 * only overflown items are visible.
		 */
		public string? create_overflown_menu() {
			if(!overflown) return null;
			StringBuilder sb = new StringBuilder("");
			sb.append("<menu>");
			List<weak Widget> children = gtk_container_get_children(this);
			foreach(weak Widget child in children) {
				bool need_overflown_item = false;
				Allocation a = child.allocation;
				switch(pack_direction) {
					case PackDirection.LTR:
						if(a.x + a.width > allocation.width) {
							need_overflown_item = true;
						}
					break;
					case PackDirection.RTL:
						if(a.x < 0 ) {
							need_overflown_item = true;
						}
					break;
					case PackDirection.BTT:
						if(a.y < 0 ) {
							need_overflown_item = true;
						}
					break;
					case PackDirection.TTB:
						if(a.y + a.height > allocation.height) {
							need_overflown_item = true;
						}
					break;
				}
				/* This is quirky. But it works
				 * we first save the visibility flag of
				 * the child,
				 * then change it
				 * our serializer will produce the visible=false
				 * attribute.
				 * then we restore it.
				 *
				 * Not thread safe.
				 * */
				bool vis = child.visible;
				if(need_overflown_item && vis) {
					child.set_flags(WidgetFlags.VISIBLE);
				} else {
					child.unset_flags(WidgetFlags.VISIBLE);
				}
				sb.append(Serializer.to_string(child));
				if(vis) {
					child.set_flags(WidgetFlags.VISIBLE);
				} else {
					child.unset_flags(WidgetFlags.VISIBLE);
				}
			}
			sb.append("</menu>");
			return sb.str;
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

		private MenuBar _overflown_menubar;
		private int _min_length;

		private Requisition real_requisition;

		private Allocation real_allocation; /*minus the overflown*/

		private bool disposed;

		public override void style_set(Style old_style) {
			base.style_set(old_style);
			if(_overflown_menubar != null) {
				_overflown_menubar.style = style;
			}
			reset_bg_pixmap();
		}
		private MenuBar create_overflown_menubar() {
			MenuBar menubar = new MenuBar();
			menubar.set_parent(this);
			menubar.style = style;
			Parser.parse(menubar,OVERFLOWER_TEMPLATE.printf("<menu/>"));
			menubar.activate += (menubar, item) => {
				string path = item.path;
				if(item.id == "_arrow_") {
					rebuild_overflown_menubar();
				} else {
					string stripped_path = overflown_path_to_path(path);
					if(stripped_path != null) {
						Gnomenu.MenuItem item = get(stripped_path);
						if(item != null)
							activate(item);
						else {
							warning("MenuItem %s not found in the main menubar!", stripped_path);
						}
					}
				}
			
			};
			return menubar;
		}
		private void rebuild_overflown_menubar() {
			string overflown_menu = create_overflown_menu();
			if(overflown_menu == null) {
				overflown_menu = "<menu/>";
			}
			string overflower_context = OVERFLOWER_TEMPLATE.printf(overflown_menu);
			Parser.parse(_overflown_menubar, overflower_context);
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
						-(_background.offset_x + real_allocation.x - allocation.x), 
						-(_background.offset_y + real_allocation.y - allocation.y));
				weak Cairo.Pattern pattern = cairo.get_source();
				pattern.set_extend(Cairo.Extend.REPEAT);
				cairo.rectangle (0, 0, allocation.width, allocation.height);
				cairo.fill();
				style.bg_pixmap[(int)StateType.NORMAL] = pixmap;
				style.attach(window);
				style.set_background(window, StateType.NORMAL);
				queue_draw();
			}
			if(_overflown_menubar != null) {
				Background bg = background.clone();
				bg.offset_x += (_overflown_menubar.allocation.x - allocation.x) ;
				bg.offset_y += (_overflown_menubar.allocation.y - allocation.y) ;
				_overflown_menubar.background = bg;
			}
		}
		public override void forall(Gtk.Callback callback, void* data) {
			bool include_internals = false;

			if(include_internals) {
				if(_overflown_menubar != null)
					callback(_overflown_menubar);
			}
			base.forall(callback, data);
		}
		public override void realize() {
			base.realize();
			/* because it is possible that the bg is set before this widget is realized*/
			if(_overflown_menubar != null) {
				_overflown_menubar.set_parent_window(get_parent_window());
				_overflown_menubar.realize();
			}
			reset_bg_pixmap();
		}
		public override void map() {
			base.map();
			if(_overflown_menubar != null && _overflown_menubar.visible) {
				_overflown_menubar.map();
			}
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
			if(_overflown_menubar != null) {
				Gdk.Rectangle oa = {0, 0, 0, 0};
				Gdk.Rectangle ba = a;
				Requisition or;
				_overflown_menubar.get_child_requisition(out or);
				if(!overflown) {
					_overflown_menubar.visible = false;
				} else {
					switch(pack_direction) {
						case PackDirection.TTB:
							oa.height = or.height;
							oa.width = a.width;
							oa.x = a.x;
							oa.y = a.y + a.height - oa.height;
							ba.height -= oa.height;
							ba.y = a.y;
							break;
						case PackDirection.BTT:
							oa.height = or.height;
							oa.width = a.width;
							oa.x = a.x;
							oa.y = a.y;
							ba.height -= oa.height;
							ba.y = a.y + oa.height;
							break;
						case PackDirection.RTL:
							oa.height = a.height;
							oa.width = or.width;
							oa.x = a.x;
							oa.y = a.y;
							ba.width -= oa.width;
							ba.x = oa.width;
							break;
						default:
						case PackDirection.LTR:
							oa.width = or.width;
							oa.height = a.height;
							oa.x = a.x + a.width - oa.width;
							oa.y = a.y;
							ba.width -= oa.width;
							break;
					}
				}
				base.size_allocate(ba);
				real_allocation = (Allocation)ba;
				allocation = (Allocation) a;
				if(overflown) {
					_overflown_menubar.size_allocate(oa);
					_overflown_menubar.visible = true;
				}
			} else  {
				base.size_allocate(a);
				real_allocation = (Allocation)a;
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
			foreach(weak Widget child in gtk_container_get_children(this)) {
				propagate_expose(child, event);
			}
			if(_overflown_menubar != null) {
			//	propagate_expose(_overflown_menubar, event);
			}
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
				if(_overflown_menubar != null) {
					_overflown_menubar.size_request(out r);
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
		public void remove_all() {
			gtk_menu_shell_truncate(this, 0);
		}	
	}
}
