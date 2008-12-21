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
		private Gdk.Pixbuf _icon;
		private Wnck.Window _window;
		
		private const string TEMPLATE = """<menu><item label="%s" font="bold"/></menu>""";
		private GLib.HashTable<string,string> program_list;
		
		public Switcher() {
			Parser.parse(this, TEMPLATE.replace("%s","Global Menu Bar"));
			program_list = GnomeMenuHelper.get_flat_list();
			if (program_list.lookup("nautilus")==null)
				program_list.insert("nautilus", "File Manager");
		}
		private string remove_path(string txt, string separator) {
			long co = txt.length-1;
			while ((co>=0) && (txt.substring(co, 1)!=separator)) {
				co--;
			}
			string ret = txt.substring(co+1,(txt.length-co-1));
			return ret;
		}
		private void app_selected(Gtk.ImageMenuItem? item) {
			Wnck.Window window = item.user_data as Wnck.Window;
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
		private void do_menu(Gnomenu.MenuItem? mi_parent, Wnck.Window window) {
			Gtk.Menu menu = null;
			
			if ((window.get_window_type()!=Wnck.WindowType.DESKTOP) && (_show_window_actions))
				menu = new Wnck.ActionMenu(window); else
				if (_show_window_list) menu = new Gtk.Menu();
				/* FIXME: Since it is a Gtk.Menu, some rgba tricks has to be added */	
			if (_show_window_list) {
				menu.insert(new Gtk.SeparatorMenuItem(), 0);
				weak GLib.List<Wnck.Window> windows = Wnck.Screen.get_default().get_windows();
				foreach(weak Wnck.Window window in windows) {
					if (!window.is_skip_pager()) {
						Gtk.ImageMenuItem mi;
						string txt = window.get_name();
						if ((txt.length>max_size) && (max_size>3)) txt = txt.substring(0, (max_size-3)) + "...";
						mi = new Gtk.ImageMenuItem.with_label(txt);
						if (window.is_active())
							(mi.child as Gtk.Label).set_markup_with_mnemonic("<b>" + txt + "</b>");
						
						mi.set_image(new Gtk.Image.from_pixbuf(window.get_mini_icon()));
						mi.user_data = window;
						mi.activate += app_selected;
						menu.insert(mi, 0);
					}
				}
			}
			
			if (menu!=null) menu.show_all();
			mi_parent.submenu = menu;
		}
		private string get_process_name(Wnck.Window window) {
			string txt = __get_task_name_by_pid(window.get_application().get_pid());
			if ((txt==null) || (txt=="")) return window.get_application().get_name();
			string ret = txt.chomp();
			if (ret.substring(ret.length-4,4)==".exe") return remove_path(ret, "\\"); // is a wine program

			ret = remove_path(ret.split(" ")[0], "/");
				
			switch(ret) {
			case "mono":
			case "python":
			case "python2.5":
			case "vmplayer":
				return remove_path(txt.chomp().split(" ")[1], "/");
				break;
			case "wine":
				return window.get_application().get_name();
				break;
			}
			return ret;
		}
		private string get_application_name(Wnck.Window window) {
			if (window.get_window_type() == Wnck.WindowType.DESKTOP)
				return "Desktop";
				
			string process_name = get_process_name(window);
			if (process_name==null) return window.get_name();
			
			string ret;
			if (program_list.lookup(process_name)!=null)
				ret = program_list.lookup(process_name); else
				ret = window.get_name();
			return ret; 
		}
		private string cut_string(string txt, int max) {
			if (max<1) return txt;
			if (max<=3) return "...";
			if (txt.length>max) return txt.substring(0, (max-3)) + "...";
			return txt;
		}
		private void update() {

			this.visible = (_show_icon | _show_label);
			if (!this.visible) return;
			
			if(current_window == null) return;
			_label = get_application_name(current_window);
			if (_show_label) 
				Parser.parse(this, TEMPLATE.replace("%s", cut_string(_label, _max_size))); else
				Parser.parse(this, TEMPLATE.replace("%s", ""));
				
			_icon = current_window.get_mini_icon();
			if (_show_icon)
				this.get("/0").icon_pixbuf  = _icon; else
				this.get("/0").item_type = "normal";
				
			do_menu(this.get("/0"), current_window);
		}
		public Wnck.Window? current_window {
			get {
				return _window ;
			}
			set {
				_window = value;
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
