using Gtk;
using GnomenuGtk;

[Compact]
public class GlobalMenuModule {
	private static bool verbose = false;
	private static bool disabled = false;
	private static bool initialized = false;

	private static string log_file_name;
	private static GLib.OutputStream log_stream;
	private static Quark domain;

	[CCode (cname = "dyn_patch_init")]
	public static extern void dyn_patch_init();

	[CCode (cname = "dyn_patch_uninit")]
	public static extern void dyn_patch_uninit();

	[CCode (cname="gtk_module_init")]
	public static void gtk_module_init([CCode (array_length_pos = 0.9)] ref weak string[] args) {
		Gdk.threads_enter();
		if(!initialized) {
			initialized = true;
			if(!disabled) {	
				dyn_patch_init();
				add_emission_hooks();
			}
		}
		Gdk.threads_leave();
	}

	[CCode (cname="g_module_check_init")]
	public static string? g_module_load(Module module) {
		Gdk.threads_enter();
		debug(_("Global Menu plugin module is loaded"));
		domain = Quark.from_string("GlobalMenu");

		if(is_quirky_app()) disabled = true;

		parse_args();

		if(!disabled) {
			debug(_("Global Menu is enabled"));

			prepare_log_file();

			if(!verbose) {
//				Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_MESSAGE, empty_log_handler);
				Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_DEBUG, empty_log_handler);
				Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_INFO, empty_log_handler);
			}
		}
		/** make the module resident to avoid the rapid reloading issue
		 * before we figure out why.
		 *  Therefore after the gconf key for gtk-modules is changed,
		 *  the user needs to relogin to clear global menu

		module.make_resident();
		*/
		Gdk.threads_leave();
		return null;
	}

	[CCode (cname="g_module_unload")]
	public static void g_module_unload(Module module) {
		Gdk.threads_enter();
		if(!disabled) {
		remove_emission_hooks();
		dyn_patch_uninit();

		Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_MASK, g_log_default_handler);
		log_stream = null;
		}
		debug(_("Global Menu plugin module is unloaded"));
		Gdk.threads_leave();
	}
	
	[CCode (cname="MY_")]
	private static weak string _(string s) {
		return dgettext(Config.GETTEXT_PACKAGE, s);
	}

	private static const OptionEntry [] options = {
		{"verbose", 'v', 0, OptionArg.NONE, ref verbose, N_("Be verbose"), null},
		{"disable", 'd', 0, OptionArg.NONE, ref disabled, N_("Disable the Plugin"), null},
		{"log-file", 'l', 0, OptionArg.FILENAME, ref log_file_name, N_("File to save the log, default to stderr"), null},
		{null}
	};

	private static void parse_args() {
		string [] args;
		string env = Environment.get_variable("GLOBALMENU_GNOME_ARGS");
		
		if(env == null) return;
		string command_line = "globalmenu-gnome " + env;
		Shell.parse_argv(command_line, out args);

		OptionContext context = new OptionContext(
				_("- Global Menu plugin Module for GTK"));
		context.set_description(
_("""These parameters should be supplied in environment GLOBALMENU_GNOME_ARGS instead of the command line.
NOTE: Environment GTK_MENUBAR_NO_MAC contains the applications to be ignored by the plugin.
""")
		);
		context.set_ignore_unknown_options(true);
		context.add_main_entries(options, Config.GETTEXT_PACKAGE);
		try {
			context.parse(ref args);
		} catch (Error e) {
			warning("%s", e.message);
			print("%s", context.get_help(false, null));
		}
	}

	private static void prepare_log_file() {
		if(log_file_name != null) {
			try {
				GLib.File file = GLib.File.new_for_path(log_file_name);
				log_stream = file.append_to(FileCreateFlags.NONE, null);
			} catch (GLib.Error e) {
				warning(_("Log file %s is not accessible. Fallback to stderr. %s"), log_file_name, e.message);
			}	
		}
		if(log_stream == null) log_stream = new GLib.UnixOutputStream(2, false);
		Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_MASK, default_log_handler);
	}


	private static void default_log_handler(string? domain, LogLevelFlags level, string message) {
		TimeVal time = {0};
		time.get_current_time();
		string s = "%.10ld | %20s | %10s | %s\n".printf(time.tv_usec, Environment.get_prgname(), domain, message);
		log_stream.write(s, s.size(), null);
	}
	private static void empty_log_handler(string? domain, LogLevelFlags level,
			string message) {
		/*do nothing*/
	}
	private static bool is_quirky_app() {
		string disabled_application_names = 
			Environment.get_variable("GTK_MENUBAR_NO_MAC");

		string app_name = Environment.get_prgname();
		/* Don't use switch case because vala will create
		 * static quarks which cause core dumps when 
		 * the module is unloaded */

		/* Try to figure this out by filtering out 
		 * menubars in 'PanelApplet'
		 * and sub classes of 'PanelMenuBar'
		 * in globalmenu.vala
		if(app_name == "gnome-panel"
		|| app_name == "gdm-user-switch-applet")
			return true;
		*/

		if((disabled_application_names!=null) 
		&& disabled_application_names.str(app_name)!=null)
			return true;

		return false;
	}
}
