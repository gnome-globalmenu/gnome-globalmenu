namespace Gnomenu {
public class Application{
	public string readable_name;
	public string exec_path;
	public string icon_name;

	public Wnck.Application wnck_application;

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
	private static void init(){
		GMenu.TreeDirectory node = GMenu.Tree.lookup("applications.menu", GMenu.TreeFlags.INCLUDE_EXCLUDED).get_root_directory();
		append_node_r(node);
		initialized = true;
	}

	private Application() { }

	public static unowned Application lookup(string key) {
		if(!initialized) init();
		return dict.lookup(key);
	}
	public static unowned Application lookup_from_wnck(Wnck.Application wapp) {
		if(!initialized) init();
		string key = generate_key_from_wnck(wapp);
		weak Application rt = dict.lookup(key);
		if(rt == null) {
			Application app = new Application();
			app.readable_name = wapp.get_name();
			app.exec_path = null;
			app.icon_name = wapp.get_icon_name();
			dict.insert(key, app);
			rt = app;
			applications.prepend(#app);
		}
		/*FIXME: listen to wnckscreen.application_open /wnckscreen.application_close!*/
		rt.wnck_application = wapp;
		return rt;
	}
	private static void append_node_r(GMenu.TreeDirectory node) {
		foreach (GMenu.TreeItem item in node.get_contents()) {
			switch(item.get_type()) {
				case GMenu.TreeItemType.ENTRY:
					GMenu.TreeEntry entry = (GMenu.TreeEntry)item;
					string key = generate_key(entry);
					Application app = new Application();
					app.readable_name = entry.get_name();
					app.exec_path = entry.get_exec();
					app.icon_name = entry.get_icon();
					dict.insert(key, app);
					applications.prepend(#app);
				break;
				case GMenu.TreeItemType.DIRECTORY:
					append_node_r((GMenu.TreeDirectory)item);
				break;
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
		name_widget.label = readable_name;
		status_widget.label = "unknown yet";
		icon_widget.icon_name = icon_name;
	}

	[CCode (cname = "get_task_name_by_pid")]
	private static extern string get_task_name_by_pid(int pid);
	private static string generate_key_from_wnck(Wnck.Application app) {
		string process_name = get_process_name(app);
		if ((process_name=="") && (process_name==null)) process_name = app.get_name();

		if(process_name.has_suffix(".real")) {
			return process_name.substring(0, - 5);
		}
		return process_name;
	}

	private static string get_process_name(Wnck.Application app) {
		string txt = get_task_name_by_pid(app.get_pid());
		if ((txt==null) || (txt=="")) return "";
		string ret = txt.chomp();

		if (ret.has_suffix(".exe"))
			return ret; // is a wine program
		
		/* First, remove the parameters */
		ret = ret.split(" ")[0];
		/* Second, remove the path */
		weak string path_stripped = ret.rchr(-1, '/');
		switch(path_stripped) {
		case "mono":
		case "python":
		case "python2.5":
		case "vmplayer":
			string[] buf = txt.chomp().split(" ");
			if (buf.length<2)
				return ret; 
			else
				return buf[1].rchr(-1, '/');
			break;
		case "wine":
			return app.get_name();
			break;
		}
		return ret;
	}

/* FIXME: 
 * The following functions are 
 * to be replaced by a manually written normalizer with StringBuilder*/
	private static string generate_key(GMenu.TreeEntry entry ) {
		string txt = entry.get_exec();
		txt = adjust_spaces(txt);
		
		string[] buf = txt.split(" ");
		int cc=0;
		while(buf[cc]=="env") cc+=2;
		while(buf[cc]=="wine") cc++;
		txt = buf[cc];
		
		long co = txt.length-1;
		while ((co>=0) && (txt.substring(co, 1)!="/")) {
			co--;
		}
		txt = txt.substring(co+1,(txt.length-co-1));
		txt = replace(txt, "&nbsp;", " ");
		txt = replace(txt, "\"", "");
		return txt;
	}
	private static string replace(string source, string find, string replacement) {
		/* replaces the string.replace method which depends on GLib.RegEx >= 2.12 */
		string[] buf = source.split(find);
		return join(buf, replacement);
	}
	private static string join(string[] buf, string separator) {
		string ret = "";
		for (int co=0; co<buf.length; co++) {
			ret+=buf[co];
			if (co!=(buf.length-1)) ret+=separator;
		}
		return ret;
	}
	private static string adjust_spaces(string source) {
		string ret = "";
		bool quoted = false;
		for (int co=0; co<source.length; co++) {
			if (source[co]=='"') quoted = !quoted;
			if ((source[co]==' ') && quoted)
				ret += "&nbsp;"; else
				ret += source.substring(co, 1);
		}
		return ret;
	}
}
}
