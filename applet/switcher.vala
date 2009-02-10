using GLib;
using Gnomenu;
using Gtk;
using GnomeMenuHelper;
public extern string* __get_task_name_by_pid(int pid);
	
	public class Switcher : Gnomenu.MenuBar {
		private string _label;
		private int _max_size = 30;
		private bool _show_icon = false;
		private bool _show_label = true;
		private bool _show_window_list = true;
		private bool _show_window_actions = true;
		private Wnck.Window _current_window;

		private const string MENU_TEMPLATE = """<?xml version="1.0" encoding="UTF-8"?>
<menu>
	<item id="switcher" label="%label%" font="bold" type="%type%" icon="%icon%">
			%submenu%
	</item>
</menu>""";
		private const string ITEM_TEMPLATE = """<item type="image" id="XID%id%" label="%label%" font="%font%" icon="pixbuf:%pixdata%" sensitive="true">%submenu%</item>""";
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

		private GLib.HashTable<string,string> program_list;
		
		private bool disposed = false;

		public Switcher() {
		}

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
				this.activate += on_activate;
			} catch (GLib.Error e) {
				warning("%s", e.message);
			}
			program_list = GnomeMenuHelper.get_flat_list();
			if (program_list.lookup("nautilus")==null)
				program_list.insert("nautilus", _("File Manager"));
				
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
		private void on_deactivate() {
			update(false);
		}
		private void on_activate(Gnomenu.MenuBar _this, Gnomenu.MenuItem item) {
			if(item.item_path == "/switcher") {		
				update(true);
				return;
			}

			/** Then handle the window actions */
			Wnck.Window window = item_to_window(item);
			if (window == null) return;
			
			switch(item.item_id) {
				case "minimize":
					window.minimize(); break;
				case "maximize":
					window.maximize(); break;
				case "unmaximize":
					window.unmaximize(); break;
				case "move":
					window.keyboard_move(); break;
				case "resize":
					window.keyboard_size(); break;
				case "topmost":
					window.make_above(); break;
				case "untopmost":
					window.unmake_above(); break;
				case "stick":
					window.stick(); break;
				case "unstick":
					window.unstick(); break;
				case "close":
					window.close(Gtk.get_current_event_time()); break;
					break;
				case "show_desktop":
					weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
					foreach(weak Wnck.Window w in windows) {
						if (is_in_sight(w))
							w.minimize();
					}
					break;
				case "workspaces":
					WorkspaceSelector ws = new WorkspaceSelector(_current_window);
					ws.show();
					break;
				default:
					/** dirty trick to ignore the activate
					 * signal on the item with the wnck action menu */
					if (item.submenu != null) return;
					perhaps_minimize_window(window);
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
			(window as WnckCompat.Window).get_geometry(out x, out y, out w, out h);

			screen.move_viewport(current_workspace_x + x, current_workspace_y + y);
			
			window.activate(Gtk.get_current_event_time());
			workspace.activate(Gtk.get_current_event_time());
			window.unminimize(Gtk.get_current_event_time());
			
			// ensure is on top
			window.make_above();
			window.unmake_above();

			//TOFIX: if the window is on another workspace and it is minimized, it doesn't unminimize automatically.
		
		
		}
		private string remove_path(string txt, string separator) {
			long co = txt.length-1;
			while ((co>=0) && (txt.substring(co, 1)!=separator)) {
				co--;
			}
			string ret = txt.substring(co+1,(txt.length-co-1));
			return ret;
		}
		private void set_iconify_destination(Wnck.Window window) {
			int ax = 0;
			int ay = 0;
			this.window.get_origin(out ax, out ay);
			window.set_icon_geometry(ax,
									 ay,
						 			 allocation.width,
									 allocation.height);
		}

		private bool is_in_sight(Wnck.Window window) {
			return (window.is_in_viewport(_current_window.get_workspace()) &&
					!window.is_minimized() &&
					!window.is_skip_pager());
		}
		private string pixbuf_encode_b64(Gdk.Pixbuf pixbuf) {
			Gdk.Pixdata pixdata = {0};
			pixdata.from_pixbuf(pixbuf, true);
			return Base64.encode(pixdata.serialize());
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
						"%label%", Markup.escape_text(replace(cut_string(window.get_name(), _max_size), "_", "__")),
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
		private Wnck.Window find_desktop() {
			weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
			foreach(weak Wnck.Window window in windows)
				if (window.get_window_type() == Wnck.WindowType.DESKTOP) return window;
			return null;
		}
		private string get_process_name(Wnck.Window window) {
			string txt = __get_task_name_by_pid(window.get_pid());
			if ((txt==null) || (txt=="")) return "";
			string ret = txt.chomp();

			if (ret.substring(ret.length-4,4)==".exe")
				return ret; // is a wine program
			
			ret = remove_path(ret.split(" ")[0], "/");
			switch(ret) {
			case "mono":
			case "python":
			case "python2.5":
			case "vmplayer":
				string[] buf = txt.chomp().split(" ");
				if (buf.length<2)
					return ret; else
					return remove_path(buf[1], "/");
				break;
			case "wine":
				return window.get_application().get_name();
				break;
			}
			return ret;
		}
		private string get_program_name(Wnck.Window window) {
			if (window.get_window_type() == Wnck.WindowType.DESKTOP)
				return _("Desktop");
			
			string process_name = get_process_name(window);
			if ((process_name=="") && (process_name==null)) process_name = window.get_application().get_name();

			string ret = program_list.lookup(process_name);
			if (ret == null) {
				/* try by removing .real (i.e. Skype bug) */
				ret = program_list.lookup(replace(process_name, ".real", ""));
			}
			if (ret == null) {
				ret = window.get_name();
				switch (process_name) {
					/* fixes a problem with some apps like Archive Manager
					 * Add any other affected application in cases below here */
					case "file-roller":
					case "evince":
					case "eog":
					case "soffice.bin":
						if (program_list.lookup(process_name)==null)
							program_list.insert(process_name, ret);	
						break;
				}
			}
			return ret; 
		}
		private string cut_string(string txt, int max) {
			if (max<1) return txt;
			if (max<=3) return "...";
			if (txt.length>max) return txt.substring(0, (max-3)) + "...";
			return txt;
		}
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
		private void update(bool include_menu = false) {
			/* root and menu are then used for the window list */
			Gnomenu.MenuItem root = this.get("/switcher");
			Gtk.Menu menu = root.submenu;
			/* prevent the menu to be updated while visible so causing the applet to block */
			if(menu != null)
				menu.popdown();
			
			this.visible = (_show_icon | _show_label);
			if (!this.visible) return;
			
			if(current_window == null) return;
			
			Wnck.WindowType wt = current_window.get_window_type();
			if (wt==Wnck.WindowType.DOCK) 
				_label = ""; else
				_label = cut_string(get_program_name(current_window), _max_size);
			
			string s = MENU_TEMPLATE;
			s = replace(s, "%label%", Markup.escape_text(replace(_label, "_", "__")));
			if (_show_icon) {
				if (_show_label)
					s = replace(s, "%type%", "image"); else
					s = replace(s, "%type%", "icon");
				
				int width = allocation.width - 2;
				int height = allocation.height - 2;
				if(width <= 0) width = -1;
				if(height <= 0) height = -1;

				s = replace(s, "%icon%", "pixbuf:" + 
						pixbuf_encode_b64(guess_icon(current_window, width, height)));
					
			} else {
				s = replace(s, "%type%", "normal");
				s = replace(s, "icon=\"%icon%\"", "");
			}
			
			if (include_menu) {
				if (_show_window_list) {
					s = replace(s, "%submenu%", do_xml_menu());
					Parser.parse(this, s);
					
					Gnomenu.MenuItem misd = this.get("/switcher/show_desktop");
					if (misd!=null) {
						override_item_window(misd, find_desktop());
					}
					
					foreach(Gtk.Widget widget in menu.get_children()) {
						Gnomenu.MenuItem item = widget as Gnomenu.MenuItem;
						Wnck.Window window = item_to_window(item);
						if(window == null) continue;
						if (window.is_active()) {
							setup_window_actions_menu(
									"/switcher/" + item.item_id + "/", 
									window);
						}
					}
				} else {
					if (_show_window_actions) {
						s = replace(s, "%submenu%", do_action_menu(_current_window));
						Parser.parse(this, s);
						setup_window_actions_menu("/switcher/", _current_window);
					}
				}
			} else {
				s = replace(s, "%submenu%", "<menu/>");
				Parser.parse(this, s);
			}
		}
		private void setup_window_actions_menu(string prefix, Wnck.Window window) {
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
		}
		private bool guess_dock_is_around() {
			weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
			foreach(weak Wnck.Window window in windows)
				if (window.get_window_type() == Wnck.WindowType.DOCK) {
					switch(window.get_application().get_name()) {
						case "cairo-dock":
						case "avant-window-navigator":
							/* add any other known dock having a task bar */
							return true;
							break;
					}
				}
			return false;
		}
		/**
		 * The following 3 functions map item to window and verse vesa
		 *
		 */
		private Gnomenu.MenuItem? window_to_item(Wnck.Window window) {
			return this.get("/switcher/XID" + window.get_xid().to_string());
		}
		private void override_item_window(Gnomenu.MenuItem item, Wnck.Window? window) {
			item.user_data = window;
		}
		private Wnck.Window? item_to_window(Gnomenu.MenuItem item) {
			string id = item.item_id;
			if(item.user_data != null) return item.user_data as Wnck.Window;
			if(id.has_prefix("XID")) {
				return Wnck.Window.get(id.offset(3).to_ulong());
			}
			return null;
		}
		public Wnck.Window? current_window {
			get {
				return _current_window ;
			}
			set {
				_current_window = value;
				if (!guess_dock_is_around())
					set_iconify_destination(_current_window);
				/* always refresh !*/
				update();
			}
		}
		public int max_size {
			get { return _max_size; }
			set {
				if(_max_size == value) return;
				_max_size = value;
				update();
			}
		}
		public bool show_icon {
			get { return _show_icon; }
			set {
				if(_show_icon == value) return;
				_show_icon = value;
				update();
			}
		}
		public bool show_label {
			get { return _show_label; }
			set {
				if(_show_label == value) return;
				_show_label = value;
				update();
			}
		}
		public bool show_window_list {
			get { return _show_window_list; }
			set {
				if(_show_window_list == value) return;
				_show_window_list = value;
				update();
			}
		}
		public bool show_window_actions {
			get { return _show_window_actions; }
			set {
				if(_show_window_actions == value) return;
				_show_window_actions = value;
				update();
			}
		}
	}
