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
		
		private const string MENU_TEMPLATE = """<menu>
	<item label="%label%" font="bold">%sub-menu%</item>
</menu>""";
		private const string ITEM_TEMPLATE = """<item type="image" id="_%id%" label="%label%" font="%font%" icon="pixbuf:%pixdata%"/>""";
		
		private GLib.HashTable<string,string> program_list;
		
		public Switcher() {
		}

		construct {
			try {
				Parser.parse(this, MENU_TEMPLATE.replace("%label%","Global Menu Bar").replace("%sub-menu%", ""));
			} catch (GLib.Error e) {
				warning("%s", e.message);
			}
			program_list = GnomeMenuHelper.get_flat_list();
			if (program_list.lookup("nautilus")==null)
				program_list.insert("nautilus", _("File Manager"));
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
		private void app_selected(Gtk.MenuItem? item) {
			Wnck.Window window = item.user_data as Wnck.Window;
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
		}
		string pixbuf_encode_b64(Gdk.Pixbuf pixbuf) {
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
					string item = ITEM_TEMPLATE.replace("%label%", cut_string(window.get_name(), _max_size));
					if (window.is_active())
						item = item.replace("%font%", "bold"); else
						if (window.is_minimized())
							item = item.replace("%font%", "italic"); else
							item = item.replace("%font%", "");
					
					item = item.replace("%pixdata%",
										pixbuf_encode_b64(window.get_mini_icon()));
					item = item.replace("%id%", window.get_xid().to_string());
					items+=item;
				}
			}
			if (items=="")
				items = ITEM_TEMPLATE.replace("%label%", " " + _("no windows") + " ").replace("%font%", "");
			return "<menu>" + items + "</menu>";
		}
		private Gtk.Menu do_action_menu(Wnck.Window? window) {
			Wnck.ActionMenu ret = new Wnck.ActionMenu(window); 
			return ret;
		}
		private string get_process_name(Wnck.Window window) {
			string txt = __get_task_name_by_pid(window.get_pid());
			if ((txt==null) || (txt=="")) return "";
			string ret = txt.chomp();
			if (ret.substring(ret.length-4,4)==".exe") return remove_path(ret, "\\"); // is a wine program

			ret = remove_path(ret.split(" ")[0], "/");
				
			switch(ret) {
			case "mono":
			case "python":
			case "python2.5":
			case "vmplayer":
				return remove_path(txt.chomp().split(" ")[1], "/");
			case "wine":
				return window.get_application().get_name();
			}
			return ret;
		}
		private string get_application_name(Wnck.Window window) {
			if (window.get_window_type() == Wnck.WindowType.DESKTOP)
				return _("Desktop");
				
			string process_name = get_process_name(window);
			if (process_name=="") process_name = window.get_name();

			string ret = program_list.lookup(process_name);
			if (ret == null) {
				/* try by removing .real (i.e. Skype bug) */
				ret = program_list.lookup(process_name.replace(".real", ""));
			}
			if (ret == null) {
				ret = window.get_name();
				program_list.insert(process_name, ret);	// fixes a problem with some apps like Archive Manager.
			}
			return ret; 
		}
		private string cut_string(string txt, int max) {
			if (max<1) return txt;
			if (max<=3) return "...";
			if (txt.length>max) return txt.substring(0, (max-3)) + "...";
			return txt;
		}
		private void update() {
			Gnomenu.MenuItem item = this.get("/0");
			item.submenu.popdown();	/* prevent the menu to be updated while visible so causing the applet to block */
			
			this.visible = (_show_icon | _show_label);
			if (!this.visible) return;
			
			if(current_window == null) return;
			
			Wnck.WindowType wt = current_window.get_window_type();
			if (wt==Wnck.WindowType.DOCK) 
				_label = ""; else
				_label = get_application_name(current_window);
				
			try {
				if (_show_label) 
					Parser.parse(this, 
						MENU_TEMPLATE.replace("%label%", cut_string(_label, _max_size)).replace("%sub-menu%",do_xml_menu()));
				else
					Parser.parse(this, MENU_TEMPLATE.replace("%label%","").replace("%sub-menu%", ""));
			} catch (GLib.Error e) {
				warning("%s", e.message);
			}
				
			item.use_underline = false;
			if (!_show_icon) {
				item.item_type = "normal";
			} else {
				item.icon = "custom:";
				if (_show_label) {
					item.item_type = "image";
				} else {
					item.item_type = "icon";
				}
				Gdk.Pixbuf icon = current_window.get_icon();

				int size = icon.height;
				if(icon.get_width() > size)
					size = icon.width;

				int scaled_size = allocation.height;
				if(allocation.width < scaled_size)
					scaled_size = allocation.width;

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
				item.image.set_from_pixbuf(scaled_icon);
			}

			/* TOFIX: this part should be made within do_xml_menu */
			if (_show_window_list) {
				weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
				foreach(weak Wnck.Window window in windows) {
					Gnomenu.MenuItem mi = this.get("/0/_" + window.get_xid().to_string());
					if (mi != null) {
						if(!(bool) mi.get_data("connected")) {
							mi.set_data("connected", (void*) true);
							mi.activate += (item) => {
								/* Workaround the activate signal for items with a submenu. */
								if(item.submenu == null)
									app_selected(item);
							};
						}
						mi.user_data = window;
						if ((window.get_window_type()!=Wnck.WindowType.DESKTOP) && 
							(_show_window_actions) &&
							(window.is_active())) {
							mi.submenu = do_action_menu(window); 
						} else {
							mi.submenu = null;
						}
					}
				}
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
