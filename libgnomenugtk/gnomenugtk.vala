using GLib;
using Gdk;
using Gtk;
using GtkAQD;
using Gnomenu;
using GMarkup;
using DBus;

namespace GnomenuGtk {
	[CCode (cname = "_patch_menu_bar")]
	protected extern  void patch_menu_bar();
	[CCode (cname = "_patch_menu_shell")]
	protected extern  void patch_menu_shell();
	[CCode (cname = "_patch_menu_item")]
	protected extern  void patch_menu_item();
	[CCode (cname = "gdk_window_get_is_desktop")]
	protected extern bool gdk_window_get_is_desktop (Gdk.Window window);

	private bool hook_func (SignalInvocationHint ihint, [CCode (array_length_pos = 1.9)] Value[] param_values) {
		Gtk.Widget self = param_values[0].get_object() as Gtk.Widget;
		if(self is Gtk.MenuBar) {
			if(ihint.run_type != SignalFlags.RUN_FIRST) return true;
			Gtk.Widget old_toplevel = param_values[1].get_object() as Gtk.Widget;
			Gtk.Widget toplevel = self.get_toplevel();
			if(old_toplevel is Gtk.Widget)
				old_toplevel = old_toplevel.get_ancestor(typeof(Gtk.Window));
			if(toplevel is Gtk.Widget)
				toplevel = toplevel.get_ancestor(typeof(Gtk.Window));
			if(old_toplevel != null) {
				unbind_menu(old_toplevel, self);
			}
			if(toplevel != null && (0 != (toplevel.get_flags() & WidgetFlags.TOPLEVEL))) {
				bind_menu(toplevel, self);
			}
		} 
		return true;
	}
	protected bool verbose = false;
	protected bool disabled = false;
	protected GLib.OutputStream log_stream;
	protected string application_name;
	private void default_log_handler(string? domain, LogLevelFlags level, string message) {
		TimeVal time;
		time.get_current_time();
		string s = "%.10ld | %20s | %10s | %s\n".printf(time.tv_usec, application_name, domain, message);
		log_stream.write(s, s.size(), null);
	}
	private void init_log() {
		string log_file_name = Environment.get_variable("GNOMENU_LOG_FILE");
		if(log_file_name != null) {
			try {
				GLib.File file = GLib.File.new_for_path(log_file_name);
				log_stream = file.append_to(FileCreateFlags.NONE, null);
			} catch (GLib.Error e) {
				warning("Log file %s is not accessible. Fallback to stderr: %s", log_file_name, e.message);
			}	
		}
		if(log_stream == null) log_stream = new GLib.UnixOutputStream(2, false);
		Log.set_handler ("GnomenuGTK", LogLevelFlags.LEVEL_MASK, default_log_handler);
	}
	[CCode (cname="gtk_module_init")]
	public void init([CCode (array_length_pos = 0.9)] ref weak string[] args) {
		string disabled_application_names = Environment.get_variable("GTK_MENUBAR_NO_MAC");
		disabled = (Environment.get_variable("GNOMENU_DISABLED")!=null);
		verbose = (Environment.get_variable("GNOMENU_VERBOSE")!=null);
		application_name = Environment.get_prgname();
	
		init_log();

		if(disabled) {
			message("GTK_MENUBAR_NO_MAC or GNOMENU_DISABLED is set. GlobalMenu is disabled");
			return;
		}
		if(!verbose) {
			LogFunc handler = (domain, level, message) => { };
			Log.set_handler ("GMarkup", LogLevelFlags.LEVEL_DEBUG, handler);
			Log.set_handler ("Gnomenu", LogLevelFlags.LEVEL_DEBUG, handler);
			Log.set_handler ("GnomenuGTK", LogLevelFlags.LEVEL_DEBUG, handler);
		}
		DBus.Connection conn;
		try {
			conn = Bus.get(DBus.BusType.SESSION);
		} catch (GLib.Error e) {
			warning("%s", e.message);
			conn = null;
		}
		if(conn == null) {
			message("DBus is unavailable. GlobalMenu is disabled.");
			return;
		}
		switch(Environment.get_prgname()) {
			case "gnome-panel":
			case "GlobalMenu.PanelApplet":
			case "gdm-user-switch-applet":
			message("GlobalMenu is disabled for several programs");
			return;
			break;
			default:
				if((disabled_application_names!=null) 
					&& disabled_application_names.str(application_name)!=null){
					message("GlobalMenu is disabled in GTK_MENUBAR_NO_MAC list");
					return;
				}
			break;
		}
		typeof(Gtk.Widget).class_ref();
		uint signal_id = Signal.lookup("hierarchy-changed", typeof(Gtk.Widget));
		Signal.add_emission_hook (signal_id, 0, hook_func, null);
		patch_menu_item();
		patch_menu_shell();
		patch_menu_bar();
		debug("GlobalMenu is enabled");
		Log.set_handler ("GMarkup", LogLevelFlags.LEVEL_MASK, default_log_handler);
		Log.set_handler ("Gnomenu", LogLevelFlags.LEVEL_MASK, default_log_handler);
	}
	private weak Client client() {
		return Client.instance();
	}
	private weak Document document() {
		return Client.instance().document as Document;
	}
	protected weak string translate_gtk_type(Gtk.Widget widget) {
		weak string type;
		type = "widget";
		if(widget is Gtk.Window)
			type = "window";
		if(widget is Gtk.MenuBar)
			type = "menubar";
		if(widget is Gtk.Menu)
			type = "menu";
		if(widget is Gtk.MenuItem)
			type = "item";
		if(widget is Gtk.CheckMenuItem)
			type = "check";
		if(widget is Gtk.ImageMenuItem)
			type = "imageitem";
		if(widget is Gtk.TearoffMenuItem)
			type = "tearoff";
		return type;
	}
	private void transverse(Gtk.Widget head, Document.Widget rel_root, int pos = -1) {
		weak Gtk.Widget gtk = head;
		assert(gtk is Gtk.Widget);
		assert(rel_root is Document.Widget);
		weak Document.Widget node = document().wrap(gtk);
		if(gtk is Gtk.MenuShell) {
			rel_root.insert(node, pos);
			foreach(weak Gtk.Widget child in (gtk as Gtk.Container).get_children()) {
				transverse(child, node);
			}
			(gtk as GtkAQD.MenuShell).insert += child_insert;
			(gtk as GtkAQD.MenuShell).remove += child_remove;
		}
		if(gtk is Gtk.MenuItem) {
			rel_root.insert(node, pos);
			weak Gtk.Menu submenu = (gtk as Gtk.MenuItem).submenu;
			if(submenu != null) {
				transverse(submenu, node);
			}
			gtk.notify["submenu"] += submenu_notify;
		}
	}
	private void submenu_notify(Gtk.Widget widget, ParamSpec pspec) {
		weak Gtk.Menu submenu = (widget as Gtk.MenuItem).submenu;
		weak Document.Widget node = document().wrap(widget);
		List<weak Document.Widget> list = node.children.copy();
		foreach(weak Document.Widget child in list) {
			node.remove(child);
		}
		if(submenu != null) {
			transverse(submenu, node);
		}
	}
	private void child_remove(Gtk.Widget widget, Gtk.Widget child) {
		weak Document.Widget node = document().wrap(widget);
		weak Document.Widget child_node = document().wrap(child);
		node.remove(child_node);
	}
	private void child_insert(Gtk.Widget widget, Gtk.Widget child, int pos) {
		weak Document.Widget node = document().wrap(widget);
		transverse(child, node, pos);
	}
	private void do_type_hint(Gtk.Widget window) {
		weak Document.Widget node = document().wrap(window);
		if((window as Gtk.Window).type_hint == Gdk.WindowTypeHint.DESKTOP) {
			client().set_default(node.name);
		}
	}
	private void do_realize(Gtk.Widget window) {
		weak Document.Widget node = document().wrap(window);
		client().register_window(node.name, XWINDOW(window.window).to_string());
		if(gdk_window_get_is_desktop(window.window)) { 
			/*workaround nautilus which doesn't use GDK to set the hint*/
			client().set_default(node.name);
		}
	}
	public void bind_window(Gtk.Widget window) {
		weak Document.Widget node = document().wrap(window);
		if(document().root.index(node) < 0) {
			document().root.append(node);
			if(0 != (window.get_flags() & WidgetFlags.REALIZED))
				do_realize(window);
			do_type_hint(window);
			window.notify["type-hint"] += do_type_hint;
			window.realize += do_realize;
			window.unrealize += (window) => {
				weak Document.Widget node = document().wrap(window);
				client().unregister_window(node.name);
			};
		}
	}
	public void bind_menu(Gtk.Widget window, Gtk.Widget menu) {
		bind_window(window);
		weak Document.Widget node = document().wrap(window);
		weak Document.Widget menu_node = document().wrap(menu);
		debug("binding menu %s to %s", menu_node.name, node.name);
		/*TODO: transverse menu_node, adding children*/
		transverse(menu, node);
	}
	public void unbind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak Document.Widget node = document().wrap(window);
		weak Document.Widget menu_node = document().wrap(menu);
		debug("unbinding menu %s to %s", menu_node.name, node.name);
		node.remove(menu_node);
	}
}
