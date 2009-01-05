using Gtk;

[Compact]
public class GlobalMenuGNOME {
	private static bool verbose = false;
	private static bool disabled = false;
	private static bool initialized = false;

	private static string log_file_name = null;
	private static GLib.OutputStream log_stream;
	private static Quark domain;

	private static uint deferred_init_id = 0;
	[CCode (cname="gtk_module_init")]
	public static void gtk_module_init([CCode (array_length_pos = 0.9)] ref weak string[] args) {
		if(!disabled) {
			deferred_init_id = Idle.add_full(Priority.HIGH, deferred_init);
		}
	}

	private static bool deferred_init() {
		if(!initialized) {
			initialized = true;
			DynPatch.init();
			GlobalMenuGTK.init();
		}
		deferred_init_id = 0;
		return false;
	}
	[CCode (cname="g_module_check_init")]
	public static string? g_module_load(Module module) {
		domain = Quark.from_string("GlobalMenu");

		if(is_quirky_app()) disabled = true;

		parse_args();

	/* Do not write any log messages before we prepare the log file.*/
		prepare_log_file();
		Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_DEBUG, default_log_handler);

		if(!disabled) {
			debug("Global Menu is enabled");
		} else {
			return "Global Menu is disabled";
		}
		return null;
	}

	[CCode (cname="g_module_unload")]
	public static void g_module_unload(Module module) {
		if(!disabled) {
			if(deferred_init_id != 0) {
				Source.remove(deferred_init_id);
			}
			if(initialized) {
				DynPatch.uninit();
				GlobalMenuGTK.uninit();
			}

			debug("Global Menu plugin module is unloaded");
			Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_MASK, g_log_default_handler);
			log_stream = null;
		}
	}
	
	private static const OptionEntry [] options = {
		{"verbose", 'v', 0, OptionArg.NONE, ref verbose, N_("Be verbose"), null},
		{"disable", 'd', 0, OptionArg.NONE, ref disabled, N_("Disable the Plugin"), null},
		{"log-file", 'l', 0, OptionArg.FILENAME, ref log_file_name, N_("File to save the log, default to ~/.gnomenu.log"), null},
		{null}
	};

	private static bool parse_args() {
		string [] args;
		string env = Environment.get_variable("GLOBALMENU_GNOME_ARGS");
		bool rt = true;	
		if(env != null) {
			string command_line = "globalmenu-gnome " + env;
			try {
				rt = Shell.parse_argv(command_line, out args);
			} catch( GLib.Error e) { }
			if(rt) {
				OptionContext context = new OptionContext(
						_("- Global Menu plugin Module for GTK"));
				context.set_description(
		_("""These parameters should be supplied in environment GLOBALMENU_GNOME_ARGS instead of the command line.
		NOTE: Environment GTK_MENUBAR_NO_MAC contains the applications to be ignored by the plugin.
		""")
				);
				context.set_help_enabled(false);
				context.set_ignore_unknown_options(true);
				context.add_main_entries(options, Config.GETTEXT_PACKAGE);
				try {
					rt = context.parse(ref args);
				} catch(GLib.Error e) { }
			}
		}
		if(log_file_name == null) {
			log_file_name = Environment.get_home_dir() + "/.gnomenu.log";
		}
		return rt;
	}

	private static void prepare_log_file() {
		if(!verbose) return;
		try {
			GLib.File file = GLib.File.new_for_path(log_file_name);
			log_stream = file.append_to(FileCreateFlags.NONE, null);
		} catch (GLib.Error e) {
			/*If can't log just ignore verbose parameter.*/
			verbose = false;
		}	
	}


	private static void default_log_handler(string? domain, LogLevelFlags level, string message) {
		if(!verbose) return;
		TimeVal time = {0};
		time.get_current_time();
		string s = "%.10ld | %20s | %10s | %s\n".printf(time.tv_usec, Environment.get_prgname(), domain, message);
		try {
		log_stream.write(s, s.size(), null);
		} catch (GLib.Error e) { } 
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
