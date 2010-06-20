using GLib;
using Gnomenu;
using Gtk;
extern int system(string arg);

	public class Switcher : Gnomenu.MenuBar {
		private string _label;
		private int _max_size = -1;
		private bool _show_icon = false;
		private bool _show_label = true;
		private bool _show_window_list = true;
		private bool _enable_search_box = true;
		private bool _show_window_actions = true;
			/* root and menu are then used for the window list */
		Gnomenu.MenuItem _label_item;
	   
		private Wnck.Window _current_window;
		
		private const string MENU_TEMPLATE = """<?xml version="1.0" encoding="UTF-8"?>
<menu>
	<item id="switcher" underline="false" label="%label%" font="bold" type="%type%" icon="%icon%">
			%submenu%
	</item>
</menu>""";
		private const string ITEM_TEMPLATE = """<item type="image" id="XID%id%" label="%label%" underline="false" font="%font%" icon="pixbuf:%pixdata%" sensitive="true">%submenu%</item>""";
		private string NOWIN_TEMPLATE = """<item label="%no_windows%" type="image" icon="theme:gtk-about" sensitive="false" font="italic"/>""";
		private string NOACT_TEMPLATE = """<item label="%no_actions%" type="image" icon="theme:gtk-about" sensitive="false" font="italic"/>""";
		private string WINACT_TEMPLATE = """<menu>
	<item id="maximize" type="image" icon="wnck-stock-maximize" label="%labelmaximize%" visible="%visiblemaximize%" />
	<item id="minimize" type="image" icon="wnck-stock-minimize" label="%labelminimize%" visible="%visibleminimize%" />
	<item id="unmaximize" type="normal" label="%labelunmaximize%" visible="%visibleunmaximize%" />
	<item id="move" type="normal" label="%labelmove%" visible="%visiblemove%" />
	<item id="resize" type="normal" label="%labelresize%" visible="%visibleresize%" />
	<item id="separator1" type="separator" visible="%visibleseparator1%" />
	<item id="untopmost" type="check" state="toggled" label="%labeluntopmost%" visible="%visibleuntopmost%" />
	<item id="topmost" type="check" state="untoggled" label="%labeltopmost%" visible="%visibletopmost%" />
	<item id="unstick" type="check" state="toggled" label="%labelunstick%" visible="%visibleunstick%" />
	<item id="stick" type="check" state="untoggled" label="%labelstick%" visible="%visiblestick%" />
	<item id="workspaces" type="normal" label="%labelworkspaces%" />
	<item id="separator2" type="separator" visible="%visibleseparator2%" />
	<item id="close" type="image" icon="wnck-stock-delete" label="%labelclose%" visible="%visibleclose%" />
</menu>""";

		private SearchBoxMenuItem search_box  = new SearchBoxMenuItem();
		
		private bool disposed = false;

		public Switcher() { }

		static construct {
			/* To load wnck-stock-delete
			 * wnck-stock-maximize
			 * wnck-stock-minimize
			 * */
			GLib.Type type = typeof(Wnck.ActionMenu);
			type.class_ref(); 
		}
		construct {
			try {
				string s = replace(MENU_TEMPLATE, "%label%","Global Menu Bar");
				s = replace(s, "%sub-menu%", "");
				Parser.parse(this, s);
				_label_item = this.get_item(0) as Gnomenu.MenuItem;
				_label_item._submenu_cache.append(search_box);
				this.activate += on_activate;
			} catch (GLib.Error e) {
				warning("%s", e.message);
			}
				
			NOWIN_TEMPLATE = Template.replace_simple(NOWIN_TEMPLATE, "%no_windows%", _("no windows"));
			NOACT_TEMPLATE = Template.replace_simple(NOACT_TEMPLATE, "%no_actions%", _("no actions"));
			
			string[] substs = {
				"%labelminimize%", _("Mi_nimize"), "%visiblemaximize%", "%visiblemaximize%",
				"%labelmaximize%", _("Ma_ximize"), "%visibleminimize%", "%visibleminimize%",
				"%labelunmaximize%", _("Unma_ximize"), "%visibleunmaximize%", "%visibleunmaximize%",
				"%labelmove%", _("_Move"), "%visiblemove%", "%visiblemove%",
				"%labelresize%", _("_Resize"), "%visibleresize", "%visibleresize%",
				"%visibleseparator1%", "%visibleseparator1%",
				"%labeluntopmost%", _("Always on _Top"), "%visibleuntopmost%", "%visibleuntopmost%",
				"%labeltopmost%", _("Always on _Top"), "%visibletopmost%", "%visibletopmost%",
				"%labelunstick%", _("_Always on visible Workspace"), "%visibleunstick%", "%visibleunstick%",
				"%labelstick%", _("_Always on visible Workspace"), "%visiblestick%", "%visiblestick%",
				"%labelworkspaces%", _("_Workspace..."),
				"%visibleseparator2%", "%visibleseparator2%",
				"%labelclose%", _("_Close"), "%visibleclose%", "%visibleclose%"
			};
			
			WINACT_TEMPLATE = Template.replace(WINACT_TEMPLATE, substs);
			
			/* fixes a rendering issue with Compiz */
			this.deactivate += on_deactivate;
		}

		public override void dispose() {
			if(!disposed) {
				disposed = true;
				current_window = null;
			}
		}
		private void on_deactivate() {
			update(false);
		}
		private void on_activate(Gnomenu.Shell _this, Gnomenu.Item item) {
			if(item == _label_item) {		
				update(true);
				return;
			}

			/** Then handle the window actions */
			Wnck.Window window = item_to_window(item as Gnomenu.MenuItem);
			
			/* if it is an Action Menu item, try to get reference to parent */
			if (window == null) {
				var parent_shell = item.shell;
				var parent_item = parent_shell.owner;
				window = item_to_window(parent_item as Gnomenu.MenuItem);
			}
			

			switch(item.item_id) {
				case "minimize":
					if (window != null) window.minimize(); break;
				case "maximize":
					if (window != null) window.maximize(); break;
				case "unmaximize":
					if (window != null) window.unmaximize(); break;
				case "move":
					if (window != null) window.keyboard_move(); break;
				case "resize":
					if (window != null) window.keyboard_size(); break;
				case "topmost":
					if (window != null) window.make_above(); break;
				case "untopmost":
					if (window != null) window.unmake_above(); break;
				case "stick":
					if (window != null) window.stick(); break;
				case "unstick":
					if (window != null) window.unstick(); break;
				case "close":
					if (window != null) window.close(Gtk.get_current_event_time()); break;
				case "show_desktop":
					weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
					foreach(weak Wnck.Window w in windows) {
						if (is_in_sight(w))
							w.minimize();
					}
					break;
				case "workspaces":
					WorkspaceSelector ws = new WorkspaceSelector(window);
					ws.show();
					break;
				default:
					/** dirty trick to ignore the activate
					 * signal on the item with the wnck action menu */

					if (item.has_sub_shell) return;
					if (window != null) perhaps_minimize_window(window);
					break;
			}
		}

		/** 
		 * Minimize a window if it is not minimized.
		 * restore it if minimized.
		 *
		 */
		private void perhaps_minimize_window(Wnck.Window window) {
			if (!guess_dock_is_around())
					set_iconify_destination(window);
			
			Wnck.Workspace workspace = window.get_workspace();
			if ((window.is_active()) 
			&& (window.is_visible_on_workspace(workspace))) {
				window.minimize();
				return;
			}

			// Ensure viewport visibility
			Wnck.Screen screen = window.get_screen();

			int current_workspace_x = workspace.get_viewport_x();
			int current_workspace_y = workspace.get_viewport_y();
			int x,y,w,h;
			window.get_geometry(out x, out y, out w, out h);

			screen.move_viewport(current_workspace_x + x, current_workspace_y + y);
			system("sleep 0.5");
			
			window.unminimize(Gtk.get_current_event_time());
			system("sleep 0.5");
			
			window.activate(Gtk.get_current_event_time());
			
			// ensure is on top
			//window.make_above();
			//window.unmake_above();	
		}
		private void set_iconify_destination(Wnck.Window window) {
			if(!this.is_realized()) return;
			if (window.get_window_type() != Wnck.WindowType.DESKTOP) {
				int ax = 0;
				int ay = 0;
				this.window.get_origin(out ax, out ay);
				window.set_icon_geometry(ax,
										 ay,
										 8,
										 8);
			}
		}

		private bool is_in_sight(Wnck.Window window) {
			return (window.is_in_viewport(_current_window.get_workspace()) &&
					!window.is_minimized() &&
					!window.is_skip_pager());
		}

		private string do_xml_menu() {
			if (!_show_window_list) return "";
			string items="";
			weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
			foreach(weak Wnck.Window window in windows) {
				if (!window.is_skip_pager()) {
					string font = "";
					string submenu_xml = "";
					if (window.is_active()) {
						font = "bold";
						submenu_xml = do_action_menu(window);
					} else {
						if (window.is_minimized()) {
							font = "italic";
						}
					}

					string[] substs = {
						"%label%", Markup.escape_text(window.get_name()),
						"%font%", font,
						"%submenu%", submenu_xml,
						"%pixdata%", pixbuf_encode_b64(guess_icon(window)),
						"%id%", window.get_xid().to_string()
					};
					
					string item = Template.replace(ITEM_TEMPLATE, substs);
					items+=item;
				}
			}
			if (items=="") {
				items = NOWIN_TEMPLATE;
			} else {
				if (_current_window.get_window_type() != Wnck.WindowType.DESKTOP) {
					items += "<item id=\"separator3\" type=\"separator\"/>";
					items += "<item id=\"show_desktop\" type=\"image\" icon=\"pixbuf:" + pixbuf_encode_b64(guess_icon(find_desktop())) + "\" label=\"" + _("Show _Desktop") + "\"/>";
				}
			}
			return "<menu>" + items + "</menu>";
		}
		private string do_action_menu(Wnck.Window? window) {
			if (window.get_window_type() ==  Wnck.WindowType.DESKTOP)
				return "<menu>" + NOACT_TEMPLATE + "</menu>";
				
			string[] substs = {
				"%visibleminimize%", ((window.get_actions() & Wnck.WindowActions.MINIMIZE)!=0).to_string(),
				"%visiblemaximize%", (((window.get_actions() & Wnck.WindowActions.MAXIMIZE)!=0) && !window.is_maximized()).to_string(),
				"%visibleunmaximize%", (((window.get_actions() & Wnck.WindowActions.MAXIMIZE)!=0) && window.is_maximized()).to_string(),
				"%visiblemove%", ((window.get_actions() & Wnck.WindowActions.MOVE)!=0).to_string(),
				"%visibleresize", ((window.get_actions() & Wnck.WindowActions.RESIZE)!=0).to_string(),
				"%visibleseparator1%", ((window.get_actions() & Wnck.WindowActions.ABOVE)!=0).to_string(),
				"%visibleuntopmost%", (((window.get_actions() & Wnck.WindowActions.ABOVE)!=0) && window.is_above()).to_string(),
				"%visibletopmost%", (((window.get_actions() & Wnck.WindowActions.ABOVE)!=0) && !window.is_above()).to_string(),
				"%visibleunstick%", (window.is_sticky()).to_string(),
				"%visiblestick%", (!window.is_sticky()).to_string(),
				"%visibleseparator2%", ((window.get_actions() & Wnck.WindowActions.CLOSE)!=0).to_string(),
				"%visibleclose%", ((window.get_actions() & Wnck.WindowActions.CLOSE)!=0).to_string()
			};
			return Template.replace(WINACT_TEMPLATE, substs);
		}

		private void update(bool include_menu) {

			search_box.text = "";
			
			this.visible = (_show_icon | _show_label);
			if (!this.visible) return;
			
			if(current_window == null) return;

			if (!guess_dock_is_around())
				set_iconify_destination(_current_window);
			
			Application app = Application.lookup_from_wnck_window(current_window);

			_label = app.readable_name; 
			string s = MENU_TEMPLATE;

			s = replace(s, "%label%", Markup.escape_text(_label));
			if (_show_icon) {
				if (_show_label)
					s = replace(s, "%type%", "image"); else
					s = replace(s, "%type%", "icon");
				
				int width = allocation.width - 2;
				int height = allocation.height - 2;
				if(width <= 0) width = -1;
				if(height <= 0) height = -1;

				debug ("iconname = %s no_in_menu = %s", 
						app.icon_name, app.not_in_menu.to_string()
				);
				if(app.icon_pixbuf != null) {
					s = replace(s, "%icon%", "pixbuf:" + pixbuf_encode_b64(app.icon_pixbuf));
				} else {
					s = replace(s, "%icon%", "theme:"+app.icon_name);
				}
					
			} else {
				s = replace(s, "%type%", "normal");
				s = replace(s, "icon=\"%icon%\"", "");
			}
			
			if (include_menu) {
				if (_show_window_list) {
					s = replace(s, "%submenu%", do_xml_menu());
					try {
					Parser.parse(this, s);
					} catch (GLib.Error e) {}
					
					Gtk.Menu menu = _label_item.submenu;
					
					foreach(Gtk.Widget widget in menu.get_children()) {
						Gnomenu.MenuItem item = widget as Gnomenu.MenuItem;
						if(item == null) continue;
						item.max_width_chars = 30; /* Some sane value?
													libgnomenu's default
													is 30*/
						Wnck.Window window = item_to_window(item);
						if(window == null) continue;
						/*if (window.is_active()) {
							setup_window_actions_menu(
									"/switcher/" + item.item_id + "/", 
									window);
						}*/
					}
				} else {
					if (_show_window_actions) {
						s = replace(s, "%submenu%", do_action_menu(_current_window));
						try {
						Parser.parse(this, s);
						} catch (GLib.Error e) {}
						/*setup_window_actions_menu("/switcher/", _current_window);*/
					}
				}
			} else {
				s = replace(s, "%submenu%", "<menu/>");
				try {
				Parser.parse(this, s);
				} catch (GLib.Error e) {}
			}
		}
		/*private void setup_window_actions_menu(string prefix, Wnck.Window window) {
			string[] actions = {"minimize",
								"maximize",
								"unmaximize",
								"move",
								"resize",
								"topmost",
								"untopmost",
								"stick",
								"unstick",
								"workspaces",
								"close"};
			for (int co=0; co<actions.length; co++) {
				Gnomenu.MenuItem si = this.get(prefix +	actions[co]);
				if(si != null) {
					override_item_window(si, window);
				}
			}
		}*/
		/**
		 * The following 3 functions map item to window and verse vesa
		 *
		 */
		private Gnomenu.MenuItem? window_to_item(Wnck.Window window) {
			return this.get("/switcher/XID" + window.get_xid().to_string());
		}
		private Wnck.Window? item_to_window(Gnomenu.MenuItem item) {
			string id = item.item_id;
			//if(item.user_data != null) return item.user_data as Wnck.Window;
			if(id.has_prefix("XID")) {
				return Wnck.Window.get(id.offset(3).to_ulong());
			}
			return null;
		}
		public Wnck.Window? current_window {
			get {
				return _current_window;
			}
			set {
				_current_window = value;
				/* always refresh !*/
				update(true);
			}
		}
		public override void size_request(out Requisition req) {
			base.size_request(out req);
			if(max_size > 0 && req.width > max_size) {
				/* if max_size == -1, we don't truncate it*/
				
				req.width = max_size;
			}
		}
		public int max_size {
			get { return _max_size; }
			set {
				if(_max_size == value) return;
				_max_size = value;
				if(_max_size < 40) _max_size = -1;
				update(true);
			}
		}
		public bool show_icon {
			get { return _show_icon; }
			set {
				if(_show_icon == value) return;
				_show_icon = value;
				update(true);
			}
		}
		public bool show_label {
			get { return _show_label; }
			set {
				if(_show_label == value) return;
				_show_label = value;
				update(true);
			}
		}
		public bool show_window_list {
			get { return _show_window_list; }
			set {
				if(_show_window_list == value) return;
				_show_window_list = value;
				update(true);
			}
		}
		public bool enable_search_box{
			get { return _enable_search_box; }
			set {
				if(_enable_search_box == value) return;
				_enable_search_box = value;
				update(true);
				search_box.visible = _enable_search_box;
			}
		}
		public bool show_window_actions {
			get { return _show_window_actions; }
			set {
				if(_show_window_actions == value) return;
				_show_window_actions = value;
				update(true);
			}
		}
	}

private class SearchBoxMenuItem : Gtk.ImageMenuItem {
	private Gtk.Entry textbox = new Gtk.Entry();
	public string text {
		get { return textbox.text;}
		set { textbox.text = value; }
	}
	public SearchBoxMenuItem() {
	}
	construct {
		this.image = new Gtk.Image.from_icon_name("search", Gtk.IconSize.MENU);
		textbox.editable = true;
		textbox.sensitive = true;
		textbox.can_focus = true;
		textbox.text = "Type here...";
		textbox.is_focus = true;
		
		this.add(textbox);
		this.show_all();
		this.set_focus_child(textbox);
		
		textbox.state = Gtk.StateType.NORMAL;
		textbox.state_changed += (box, previous_state) => {
			textbox.state = Gtk.StateType.NORMAL;
		};
		textbox.changed += (box) => {
			Gnomenu.Menu shell = parent as Gnomenu.Menu;
			Gnomenu.MenuItem item = find_item();
			if(item != null)
				shell.select_item(item);
		};

		textbox.activate += (box) => {
			Gnomenu.Menu shell = parent as Gnomenu.Menu;
			Gnomenu.MenuItem item = find_item();
			if(item != null)
				shell.activate_item(item, true);
		};
		this.state_changed += (_this, previous_state) => {
			this.state = Gtk.StateType.NORMAL;
		};
	}
	
	public Gnomenu.MenuItem? find_item() {
		Gnomenu.Shell shell = parent as Gnomenu.Shell;
		for(int i = 0; i < shell.length; i++) {
			Gnomenu.MenuItem item = shell.get_item(i) as Gnomenu.MenuItem;
			if(item.item_label != null &&
				item.item_label.casefold()
				.has_prefix(textbox.text.casefold())) {
				return item as Gnomenu.MenuItem;
			}
		}
		return null;	
	}

	public override void parent_set(Gtk.Widget? previous_parent) {
		if(previous_parent!=null) {
			previous_parent.key_press_event -= steal_key_press_event;
		}
		base.parent_set(previous_parent);
		if(parent != null) {
			parent.key_press_event += steal_key_press_event;
		}
	}
	private bool steal_key_press_event(Gtk.Widget widget, 
			Gdk.EventKey event) {
		return textbox.key_press_event(event);
	}
	public override void activate_item() {
	}
	public override void activate() {
	}
}


/**
 * Toolhelp routines
 *
 * */
private Gdk.Pixbuf guess_icon(Wnck.Window window, int width = -1, int height = -1) {
	Gdk.Pixbuf[] icons = {
		window.get_mini_icon(),
		window.get_icon()};
	return guess_icon_array(icons, width, height);	
}
private Gdk.Pixbuf guess_icon_array(Gdk.Pixbuf[] icons, int width, int height) {
	Gdk.Pixbuf icon = null;
	int min_dist = 99999;
	int best_size = 16;
	if(height == -1 || width == -1) {
		Gtk.icon_size_lookup(Gtk.IconSize.MENU, out width, out height);
	}
	int scaled_size = height;
	if(width < scaled_size)
		scaled_size = width;

	foreach(Gdk.Pixbuf i in icons) {
		int size = i.height;
		if(i.get_width() > size)
			size = i.width;
		int dist = size - scaled_size;
		if(dist < 0) dist = -dist;
		if(dist < min_dist) {
			min_dist = dist;
			icon = i;
			best_size = size;
		}
	}

	/* This should never happen. In case it happens
	 * return something to make sure it doesn't crash*/
	if(icon == null) return icons[0];
	double ratio = (double)scaled_size/(double)best_size;
	if(ratio < 0) ratio = 1.0;
	int scaled_width = (int) (icon.width * ratio);
	int scaled_height = (int) (icon.height * ratio);

	Gdk.Pixbuf scaled_icon = new Gdk.Pixbuf(
		Gdk.Colorspace.RGB,
		icon.has_alpha, 8,
		scaled_width,
		scaled_height
		);

	icon.scale(scaled_icon,
			0, 0, scaled_icon.width, scaled_icon.height,
			0, 0, ratio, ratio, Gdk.InterpType.BILINEAR);
	/* can't use scale_simple because the vala binding is wrong*/
	return scaled_icon;
}

private bool guess_dock_is_around() {
	weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
	foreach(weak Wnck.Window window in windows)
		if (window.get_window_type() == Wnck.WindowType.DOCK) {
			switch(window.get_application().get_name()) {
				case "cairo-dock":
				case "avant-window-navigator":
				case "Do":
					/* add any other known dock having a task bar */
					return true;
			}
		}
	return false;
}

private Wnck.Window? find_desktop() {
	weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
	foreach(weak Wnck.Window window in windows)
		if (window.get_window_type() == Wnck.WindowType.DESKTOP) return window;
	return null;
}

private string replace(string source, string find, string replacement) {
	/* replaces the string.replace method which depends on GLib.RegEx >= 2.12 */
	string[] buf = source.split(find);
	StringBuilder sb = new StringBuilder("");
	for (int co=0; co<buf.length; co++) {
		sb.append(buf[co]);
		if (co!=(buf.length-1)) sb.append(replacement);
	}
	return sb.str;
}


private string pixbuf_encode_b64(Gdk.Pixbuf pixbuf) {
	Gdk.Pixdata pixdata = {0};
	pixdata.from_pixbuf(pixbuf, true);
	return Base64.encode(pixdata.serialize());
}
