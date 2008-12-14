using Gtk;
using GnomenuGtk;

public class GlobalMenuModule {
	private static bool verbose = false;
	private static bool disabled = false;
	private static string log_file_name;
	private static GLib.OutputStream log_stream;

	private static const OptionEntry [] options = {
		{"verbose", 'v', 0, OptionArg.NONE, ref verbose, "Be verbose", null},
		{"disable", 'd', 0, OptionArg.NONE, ref disabled, "Disable the Plugin", null},
		{"log-file", 'l', 0, OptionArg.FILENAME, ref log_file_name, "File to save the log, default to stderr", null}
	};

	[CCode (cname = "dyn_patch_init")]
	private static extern void dyn_patch_init();

	[CCode (cname="gtk_module_init")]
	public static void gtk_module_init([CCode (array_length_pos = 0.9)] ref weak string[] args) {
	
		string command_line = "globalmenu-gnome " 
			+ Environment.get_variable("GLOBALMENU_GNOME_ARGS");
		string [] real_args;
		Shell.parse_argv(command_line, out real_args);
		init(ref real_args);
	}

	private static void init (ref string [] args) {
		if(is_quirky_app()) return;
		OptionContext context = new OptionContext("- Global Menu plugin Module for GTK");
		context.set_description(
"""These parameters should be supplied in environment GLOBALMENU_ARGS instead of the command line.
NOTE: Environment GTK_MENUBAR_NO_MAC contains the applications to be ignored
by the plugin."""
		);
		context.set_ignore_unknown_options(false);
		context.add_main_entries(options, null);
		try {
			context.parse(ref args);
		} catch (Error e) {
			warning("%s", e.message);
			message("%s", context.get_help(false, null));
		}



		if(disabled) {
			message("GlobalMenu is disabled");
			return;
		} else {
			message("GlobalMenu is enabled");
		}

		prepare_log_file();

		if(!verbose) {
			Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_MESSAGE, empty_log_handler);
			Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_DEBUG, empty_log_handler);
			Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_INFO, empty_log_handler);
		}

		dyn_patch_init();

		add_emission_hooks();


	
	}
	private static void prepare_log_file() {
		if(log_file_name != null) {
			try {
				GLib.File file = GLib.File.new_for_path(log_file_name);
				log_stream = file.append_to(FileCreateFlags.NONE, null);
			} catch (GLib.Error e) {
				warning("Log file %s is not accessible. Fallback to stderr: %s", log_file_name, e.message);
			}	
		}
		if(log_stream == null) log_stream = new GLib.UnixOutputStream(2, false);
		Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_MASK, default_log_handler);
	}


	private static void default_log_handler(string? domain, LogLevelFlags level, string message) {
		TimeVal time;
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

		switch(Environment.get_prgname()) {
			case "gnome-panel":
			case "GlobalMenu.PanelApplet":
			case "gdm-user-switch-applet":
			message("GlobalMenu is disabled for several programs");
			return true;
			break;
			default:
				if((disabled_application_names!=null) 
					&& disabled_application_names.str(Environment.get_prgname())!=null){
					message("GlobalMenu is disabled in GTK_MENUBAR_NO_MAC list");
					return true;
				}
			break;
		}
		return false;
	}
}
