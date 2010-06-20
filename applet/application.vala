	/******
	 * The binary executable name is used as the key to link 
	 * Wnck with GMenu.
	 *
	 * If the bin exec starts with wine, mono, python, use
	 * the application's full name. (Although usually app and
	 * dists don't agree on the same name!)
	 *
	 * the policy is sub-opt, because
	 * one executable can actually be multiple applications,
	 *
	 * Example: NXClient Openoffice
	 *
	 * Known problem: Openoffice suite apps always show up 
	 * as one Openoffice app (the last found one in GMenu).
	 * */

extern string get_task_name_by_pid(int pid);

public class Application{
	private string key;

	public string readable_name {
		get;
		private set;
	}
	public string exec_path {
		get;
		private set;
	}
	public string icon_name {
		get;
		private set;
	}
	/* has a priority over icon_name */
	public Gdk.Pixbuf icon_pixbuf {
		get;
		private set;
	}
	public bool not_in_menu {
		get; 
		private set;
	}

	public List<Wnck.Application> wnck_applications;

	private Gtk.EventBox _proxy_item;
	private Gtk.Label name_widget;
	private Gtk.Label status_widget;
	private Gtk.Image icon_widget;

	public Gtk.Widget proxy_item {
		get {
			if(_proxy_item == null) {
				create_proxy_item();
			}
			return _proxy_item;
		}
	}
	public static List<Application> applications;
	public static HashTable<string, unowned Application> dict
		= new HashTable<string, unowned Application>(str_hash, str_equal);

	private static bool initialized = false;
	public static void init(){
		if(initialized) return;
		initialized = true;
		
		GMenu.TreeDirectory node;
		
		node = GMenu.Tree.lookup("applications.menu", GMenu.TreeFlags.INCLUDE_EXCLUDED).get_root_directory();
		append_node_r(node);
		node = GMenu.Tree.lookup("settings.menu", GMenu.TreeFlags.INCLUDE_EXCLUDED).get_root_directory();
		append_node_r(node);
		
		Wnck.Screen screen = Wnck.Screen.get_default();
		weak List<Wnck.Window> list = screen.get_windows();
		foreach(Wnck.Window wwin in list) {
			Wnck.Application wapp = wwin.get_application();
			application_opened(screen, wapp);
		}
		screen.application_opened += application_opened;
		screen.window_opened += window_opened;
		screen.window_closed += window_closed;
		screen.application_closed += application_closed;
		List<AppInfo> appinfos = AppInfo.get_all();
		foreach(var appinfo in appinfos) {
			debug("%s", appinfo.get_executable());
		}
	}

	private Application() { }

	public static unowned Application add_application(string key) {
		Application app = new Application();
		unowned Application rt = app;
		dict.insert(key, app);
		applications.prepend((owned)app);
		return rt;
	}

	public static unowned Application lookup(string key) {
		init();
		return dict.lookup(key);
	}
	public static unowned Application lookup_from_wnck_window(Wnck.Window wwin) {
		init();
		Wnck.Application wapp = wwin.get_application();
		weak Application rt = lookup_from_wnck(wapp);
		string key = generate_key_from_wnck_window(wwin);
		if(rt == null) {
			debug("wnck key = %s", key);
			rt = dict.lookup(key);
		}
		if(rt == null) {
			Application app = new Application();
			app.key = key;
			app.not_in_menu = true;

			app.exec_path = null;
			/* NOTE: get_icon_name is not implement in wnck.
			 * Therefore we set icon_pixbuf, which
			 * indeed has a higher priority in switcher.vala
			 * */
			app.icon_name = wwin.get_icon_name();

			dict.insert(key, app);
			rt = app;
			applications.prepend((owned)app);
		}
	
		switch(wwin.get_window_type()) {
			case Wnck.WindowType.DESKTOP :
				rt.readable_name = _("Desktop");
			break;
			case Wnck.WindowType.DOCK :
			/* We are in good hands if a dock is activated */
			/* FIXME: probably should simply not activate a dock */
				rt.readable_name = "";
			break;
		}
		return rt;
	}
	public static unowned Application lookup_from_wnck(Wnck.Application wapp) {
		init();
		string key = generate_key_from_wnck(wapp);
		debug("wnck key = %s", key);
		weak Application rt = dict.lookup(key);
		if(rt == null) {
			Application app = new Application();
			app.key = key;
			app.not_in_menu = true;
			string name = wapp.get_name();

			/* workaround to java/swt based apps due to a bug in swt */
			if(name == "." || name == "<unknown>") {
				unowned List<Wnck.Window> windows = wapp.get_windows();
				if(windows != null) 
					name = windows.data.get_name();
			} 
			app.readable_name = name;

			app.exec_path = null;
			/* NOTE: get_icon_name is not implement in wnck.
			 * Therefore we set icon_pixbuf, which
			 * indeed has a higher priority in switcher.vala
			 * */
			app.icon_name = wapp.get_icon_name();

			dict.insert(key, app);
			rt = app;
			applications.prepend((owned)app);
		}
		/* Always use the icon_pixbuf obtained from wnck.*/
		rt.icon_pixbuf = wapp.get_mini_icon();

		return rt;
	}
	public void update() {
		if(proxy_item == null) return;
		if(wnck_applications != null) {
			int n = 0;
			foreach(Wnck.Application wapp in wnck_applications) {
				n += wapp.get_n_windows();
			}
			status_widget.label = "%u instances, %d windows"
				.printf(wnck_applications.length(), n);
		} else {
			status_widget.label = "not launched";
		}
		name_widget.label = readable_name;
		icon_widget.icon_name = icon_name;
	}
	private static void append_node_r(GMenu.TreeDirectory node) {
		foreach (GMenu.TreeItem item in node.get_contents()) {
			switch(item.get_type()) {
				case GMenu.TreeItemType.ENTRY:
					GMenu.TreeEntry entry = (GMenu.TreeEntry)item;
					string key = generate_key(entry);
					debug("gmenu key = %s", key);
					Application app = new Application();
					app.key = key;
					app.not_in_menu = false;
					app.readable_name = entry.get_name();
					app.exec_path = entry.get_exec();
					app.icon_name = entry.get_icon();
					dict.insert(key, app);
					applications.prepend((owned)app);
				break;
				case GMenu.TreeItemType.DIRECTORY:
					append_node_r((GMenu.TreeDirectory)item);
				break;
			}
		}	
	}

	private static void window_opened(Wnck.Screen s, Wnck.Window wwin) {
		Application app = lookup_from_wnck(wwin.get_application());
		wwin.set_data("gnomenu-app", app);
		app.update();
		
	}
	private static void window_closed(Wnck.Screen s, Wnck.Window wwin) {
		Application app = wwin.get_data<Application>("gnomenu-app");
		if(app == null)	return;
		app.update();
	}
	private static void application_opened(Wnck.Screen s, Wnck.Application wapp) {
		Application app = lookup_from_wnck(wapp);
		app.wnck_applications.prepend(wapp);
		app.update();
	}
	private static void application_closed(Wnck.Screen s, Wnck.Application wapp) {
		foreach(Application app in applications) {
			/*Don't use lookup_from_wnck, since no pid now?*/
			if(app.wnck_applications.find(wapp) != null) {
				app.wnck_applications.remove(wapp);
				app.update();
			}
			if(app.wnck_applications == null && app.not_in_menu = true) {
				/*TODO: After merging the code to Monitor, fire a signal*/
				dict.remove(app.key);
				applications.remove(app);
			}
		}
	}

	private void create_proxy_item() {
		_proxy_item = new Gtk.EventBox();
		name_widget = new Gtk.Label("");
		status_widget = new Gtk.Label("");
		icon_widget = new Gtk.Image();
		Gtk.HBox hbox = new Gtk.HBox(false, 0);
		Gtk.VBox vbox = new Gtk.VBox(false, 0);
		hbox.pack_start(icon_widget, false, false, 0);
		hbox.pack_start(vbox, true, true, 0);
		vbox.pack_start(name_widget, false, false, 0);
		vbox.pack_start(status_widget, false, false, 0);
		_proxy_item.add(hbox);
		update();
	}

	[CCode (cname = "get_task_name_by_pid")]
	public static extern string get_task_name_by_pid(int pid);
	private static string generate_key_from_wnck(Wnck.Application app) {
		string rt = get_process_name(app.get_pid());
		if(rt == null) return app.get_name();
		return rt;
	}
	private static string generate_key_from_wnck_window(Wnck.Window win) {
		string rt = get_process_name(win.get_pid());
		if(rt == null) return win.get_name();
		return rt;
	}

	private static string? get_process_name(int pid) {
		string cmdline = get_task_name_by_pid(pid);
		string[] args;
		if (cmdline == null || cmdline =="") return null;

		try {
			GLib.Shell.parse_argv(cmdline, out args);

			string basename = Path.get_basename(args[0]);

			switch(basename) {
				case "mono":
				case "python":
				case "python2.5":
				case "wine":
				return null;
				case "swriter.bin":
				return "openoffice.org";
			}
			return basename;
		} catch (GLib.Error e) {
			return null;
		}
	}

/* FIXME: 
 * The following functions are 
 * to be replaced by a manually written normalizer with StringBuilder*/
	private static string generate_key(GMenu.TreeEntry entry ) {
		string[] args;
		StringBuilder sb = new StringBuilder("");
		bool exec = true;
		try {
			GLib.Shell.parse_argv(entry.get_exec(), out args);
			for(int i = 0; i< args.length; i++) {
				if(args[i] == "env") {
					while(args[i+1].chr(-1, '=')!= null) i++;
					continue;
				}
				
				if(exec) {
					string basename = Path.get_basename(args[i]);
					switch(basename) {
						/*For those aliens, use the entry name
						 * and hope the app set the application
						 * name to the same value.*/
						case "mono":
						case "wine":
						case "python":
						case "python2.5":
						return entry.get_name();
					}
					sb.append(basename);
					exec = false;
				} else {
					/*ignore other parameters */
				}
			}
			return sb.str;
		} catch(GLib.Error e) {
			return entry.get_exec();
		}
	}
}
