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
		
		private static const string ICON_CLOSE = """pixbuf:R2RrUAAAAL0CAQACAAAAQAAAABAAAAAQswAAAACCAAAA/4YAAAAAggAAAP+GAAAAAIMAAAD/hAAAAACDAAAA/4cAAAAAgwAAAP+CAAAAAIMAAAD/iQAAAACGAAAA/4sAAAAAhAAAAP+MAAAAAIQAAAD/iwAAAACGAAAA/4kAAAAAgwAAAP+CAAAAAIMAAAD/hwAAAACDAAAA/4QAAAAAgwAAAP+GAAAAAIIAAAD/hgAAAACCAAAA/7MAAAAA""";
		private static const string ICON_MAXIMIZE = """pixbuf:R2RrUAAAAL0CAQACAAAAQAAAABAAAAAQswAAAACKAAAA/4YAAAAAigAAAP+GAAAAAIoAAAD/hgAAAAABAAAA/4ji4t4AAQAAAP+GAAAAAAEAAAD/iOLi3gABAAAA/4YAAAAAAQAAAP+I4uLeAAEAAAD/hgAAAAABAAAA/4ji4t4AAQAAAP+GAAAAAAEAAAD/iOLi3gABAAAA/4YAAAAAAQAAAP+I4uLeAAEAAAD/hgAAAACKAAAA/7MAAAAA""";
		private static const string ICON_MINIMIZE = """pixbuf:R2RrUAAAAEACAQACAAAAQAAAABAAAAAQ/wAAAACkAAAAAIoAAAD/hgAAAACKAAAA/4YAAAAAigAAAP+zAAAAAA==""";
		
		private const string MENU_TEMPLATE = """<menu>
	<item label="%label%" font="bold" type="%type%" icon="%icon%">
		<menu>
			%sub_menu%
		</menu>
	</item>
</menu>""";
		private const string ITEM_TEMPLATE = """<item type="image" id="_%id%" label="%label%" font="%font%" icon="pixbuf:%pixdata%" sensitive="true"/>""";
		private string NOWIN_TEMPLATE = """<item label="%no_windows%" type="image" icon="theme:gtk-about" sensitive="false" font="italic"/>""";
		private string NOACT_TEMPLATE = """<item label="%no_actions%" type="image" icon="theme:gtk-about" sensitive="false" font="italic"/>""";
		private GLib.HashTable<string,string> program_list;
		
		private bool disposed = false;

		public Switcher() {
		}

		construct {
			try {
				string s = replace(MENU_TEMPLATE, "%label%","Global Menu Bar");
				s = replace(s, "%sub-menu%", "");
				Parser.parse(this, s);
				this.get("/0").activate += on_menu_activated;
			} catch (GLib.Error e) {
				warning("%s", e.message);
			}
			program_list = GnomeMenuHelper.get_flat_list();
			if (program_list.lookup("nautilus")==null)
				program_list.insert("nautilus", _("File Manager"));
				
			NOWIN_TEMPLATE = replace(NOWIN_TEMPLATE, "%no_windows%", _("no windows"));
			NOACT_TEMPLATE = replace(NOACT_TEMPLATE, "%no_actions%", _("no actions"));
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
			string ret = "";
			for (int co=0; co<buf.length; co++) {
				ret+=buf[co];
				if (co!=(buf.length-1)) ret+=replacement;
			}
			return ret;
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
		private void on_menu_activated(Gnomenu.MenuItem? item) {
			if (item!=this.get("/0")) return;
			update(true);
		}
		private void on_item_activated(Gnomenu.MenuItem? item) {
			if (item.submenu!=null) return;
			if (!(item.user_data is Wnck.Window)) return;	
			Wnck.Window window = item.user_data as Wnck.Window;
			
			switch(item.id) {
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
					foreach(weak Wnck.Window window in windows) {
						if (is_in_sight(window))
							window.minimize();
					}
					break;
				default:
					set_iconify_destination(window); /* make sure that it goes to the switcher */
					
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
					(item.user_data as WnckCompat.Window).get_geometry(out x, out y, out w, out h);

					screen.move_viewport(current_workspace_x + x, current_workspace_y + y);
					
					window.activate(Gtk.get_current_event_time());
					workspace.activate(Gtk.get_current_event_time());
					window.unminimize(Gtk.get_current_event_time());
					
					// ensure is on top
					window.make_above();
					window.unmake_above();

					//TOFIX: if the window is on another workspace and it is minimized, it doesn't unminimize automatically.
					break;
			}
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
					string item = replace(ITEM_TEMPLATE, 
										  "%label%",
										  cut_string(window.get_name(), _max_size));
					if (window.is_active()) {
						item = replace(item, "%font%", "bold");
						item = replace(item, "/>","><menu>" + do_action_menu(window) + "</menu></item>"); 
					} else {
						if (window.is_minimized())
							item = replace(item, "%font%", "italic"); else
							item = replace(item, "%font%", "");
					}
					
					item = replace(item,
								   "%pixdata%",
								   pixbuf_encode_b64(window.get_mini_icon()));
					item = replace(item, "%id%", window.get_xid().to_string());
					items+=item;
				}
			}
			if (items=="") {
				items = replace(ITEM_TEMPLATE, "%label%", " " + _("no windows") + " ");
				items = replace(items, "%font%", "");
				items = replace(items, "pixbuf:%pixdata%", "theme:gtk-about");
				items = replace(items, "sensitive=\"true\"", "sensitive=\"false\"");
			} else {
				if (_current_window.get_window_type() != Wnck.WindowType.DESKTOP) {
					items += "<item id=\"separator3\" type=\"separator\"/>";
					items += "<item id=\"show_desktop\" type=\"image\" icon=\"pixbuf:" + pixbuf_encode_b64(find_desktop().get_mini_icon()) + "\" label=\"" + _("Show _Desktop") + "\"/>";
				}
			}
			return items;
		}
		private string do_action_menu(Wnck.Window? window) {
			if (window.get_window_type() == Wnck.WindowType.DESKTOP) return NOACT_TEMPLATE;
			
			string ret = "";
			if ((window.get_actions() & Wnck.WindowActions.MINIMIZE)!=0) 
				ret += "<item id=\"minimize\" type=\"image\" icon=\"" + ICON_MINIMIZE + "\" label=\"" + _("Mi_nimize") + "\"/>";
			if ((window.get_actions() & Wnck.WindowActions.MAXIMIZE)!=0) 
				if (!window.is_maximized())
					ret += "<item id=\"maximize\" type=\"image\" icon=\"" + ICON_MAXIMIZE + "\" label=\"" + _("Ma_ximize") + "\"/>"; else
					ret += "<item id=\"unmaximize\" type=\"normal\" label=\"" + _("Unma_ximize") + "\"/>";
			if ((window.get_actions() & Wnck.WindowActions.MOVE)!=0)
				ret += "<item id=\"move\" type=\"normal\" label=\"" + _("_Move") + "\"/>";
			if ((window.get_actions() & Wnck.WindowActions.RESIZE)!=0)
				ret += "<item id=\"resize\" type=\"normal\" label=\"" + _("_Resize") + "\"/>";
			if ((window.get_actions() & Wnck.WindowActions.ABOVE)!=0) {
				ret += "<item id=\"separator1\" type=\"separator\"/>";
				if (window.is_above())
					ret += "<item id=\"untopmost\" type=\"check\" state=\"toggled\" label=\"" + _("Always on _Top") + "\"/>"; else
					ret += "<item id=\"topmost\" type=\"check\" state=\"untoggled\" label=\"" + _("Always on _Top") + "\"/>";
			}
			if (window.is_sticky())
				ret += "<item id=\"unstick\" type=\"check\" state=\"toggled\" label=\"" + _("_Always on visible Workspace") + "\"/>"; else
				ret += "<item id=\"stick\" type=\"check\" state=\"untoggled\" label=\"" + _("_Always on visible Workspace") + "\"/>";
				
			if ((window.get_actions() & Wnck.WindowActions.CLOSE)!=0) {
				ret += "<item id=\"separator2\" type=\"separator\"/>";
				ret += "<item id=\"close\" type=\"image\" icon=\"" + ICON_CLOSE + "\" label=\"" + _("_Close") + "\"/>";
			}
			if (ret=="")
				ret = NOACT_TEMPLATE;
			return ret;
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
		private Gdk.Pixbuf rescale_pixbuf(Gdk.Pixbuf icon, int scaled_size) {
			icon = current_window.get_icon();

			int size = icon.height;
			if(icon.get_width() > size)
				size = icon.width;

			double ratio = (double)scaled_size/(double)size;
			Gdk.Pixbuf scaled_icon = new Gdk.Pixbuf(
				Gdk.Colorspace.RGB,
				icon.has_alpha, 8,
				(int) (icon.width * ratio),
				(int) (icon.height * ratio)
				);

			icon.scale(scaled_icon,
					0, 0, scaled_icon.width, scaled_icon.height,
					0, 0, ratio, ratio, Gdk.InterpType.BILINEAR);
			/* can't use scale_simple because the vala binding is wrong*/
			return scaled_icon;
		}
		private void update(bool include_menu = false) {
			Gnomenu.MenuItem item = this.get("/0");

			/* prevent the menu to be updated while visible so causing the applet to block */
			if(item.submenu != null)
				item.submenu.popdown();	
			
			this.visible = (_show_icon | _show_label);
			if (!this.visible) return;
			
			if(current_window == null) return;
			
			Wnck.WindowType wt = current_window.get_window_type();
			if (wt==Wnck.WindowType.DOCK) 
				_label = ""; else
				_label = cut_string(get_program_name(current_window), _max_size);
			
			string s = MENU_TEMPLATE;
			s = replace(s, "%label%", _label);
			if (_show_icon) {
				if (_show_label)
					s = replace(s, "%type%", "image"); else
					s = replace(s, "%type%", "icon");
				
				int scaled_size = allocation.height;
				if(allocation.width < scaled_size)
					scaled_size = allocation.width;
				s = replace(s, "%icon%", "pixbuf:" + 
					pixbuf_encode_b64(rescale_pixbuf(current_window.get_mini_icon(), scaled_size - 2)));
					
			} else {
				s = replace(s, "%type%", "normal");
				s = replace(s, "icon=\"%icon%\"", "");
			}
			
			if (include_menu) {
				remove_all();
				if (_show_window_list) {
					s = replace(s, "%sub_menu%", do_xml_menu());
					Parser.parse(this, s);
					
					Gnomenu.MenuItem misd = this.get("/0/show_desktop");
					if (misd!=null) {
						misd.user_data = find_desktop();
						misd.activate += on_item_activated;
					}
					
					weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
					foreach(weak Wnck.Window window in windows) {
						Gnomenu.MenuItem mi = this.get("/0/_" + window.get_xid().to_string());
						if (mi != null) {
							mi.user_data = window;
							mi.activate += on_item_activated;
						}
						if (window.is_active())
							setup_window_actions_menu("/0/" + mi.id + "/", window);
					}	
				} else {
					if (_show_window_actions) {
						s = replace(s, "%sub_menu%", do_action_menu(_current_window));
						Parser.parse(this, s);
						setup_window_actions_menu("/0/", _current_window);
					}
				}
			} else {
				s = replace(s, "%sub_menu%", "");
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
								"close"};
			for (int co=0; co<actions.length; co++) {
				try {
					Gnomenu.MenuItem si = this.get(prefix +	actions[co]);
					si.activate += on_item_activated;
					si.user_data = window;
				} catch (Error e) {}
			}
		}
		public Wnck.Window? current_window {
			get {
				return _current_window ;
			}
			set {
				_current_window = value;
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
