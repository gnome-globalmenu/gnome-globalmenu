using Gtk;
using GnomenuGtk;

public class GlobalMenuModule {
	private static bool verbose = false;
	private static bool disabled = false;
	private static string log_file_name;
	private static GLib.OutputStream log_stream;

	private static const OptionEntry [] options = {
		{"verbose", 'v', 0, OptionArg.NONE, ref verbose, N_("Be verbose"), null},
		{"disable", 'd', 0, OptionArg.NONE, ref disabled, N_("Disable the Plugin"), null},
		{"log-file", 'l', 0, OptionArg.FILENAME, ref log_file_name, N_("File to save the log, default to stderr"), null}
	};

	[CCode (cname = "dyn_patch_init")]
	public static extern void dyn_patch_init();

	[CCode (cname="gtk_module_init")]
	public static void gtk_module_init([CCode (array_length_pos = 0.9)] ref weak string[] args) {
	
		dyn_patch_init();

		add_emission_hooks();
	}

	[CCode (cname="g_module_check_init")]
	public static string? g_module_load(Module module) {
		message("Global Menu plugin Module is loaded");
		string [] real_args;
		string command_line = "globalmenu-gnome " 
			+ Environment.get_variable("GLOBALMENU_GNOME_ARGS");
		Shell.parse_argv(command_line, out real_args);

		init(ref real_args);
		return null;
	}

	[CCode (cname="g_module_unload")]
	public static void g_module_unload(Module module) {
		Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_MASK, g_log_default_handler);
		log_stream = null;
		message("Global Menu plugin Module is unloaded");
	}
	
	[CCode (cname="MY_")]
	private static weak string _(string s) {
		return dgettext(Config.GETTEXT_PACKAGE, s);
	}

	private static void init (ref string [] args) {
		if(is_quirky_app()) return;
		
		OptionContext context = new OptionContext(_("- Global Menu plugin Module for GTK"));
		context.set_description(
_("""These parameters should be supplied in environment GLOBALMENU_GNOME_ARGS instead of the command line.
NOTE: Environment GTK_MENUBAR_NO_MAC contains the applications to be ignored
by the plugin.""")
		);
		context.set_ignore_unknown_options(true);
		context.add_main_entries(options, Config.GETTEXT_PACKAGE);
		try {
			context.parse(ref args);
		} catch (Error e) {
			warning("%s", e.message);
			message("%s", context.get_help(false, null));
		}



		if(disabled) {
			message(_("GlobalMenu is disabled"));
			return;
		} else {
			message(_("GlobalMenu is enabled"));
		}

		prepare_log_file();

		if(!verbose) {
			Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_MESSAGE, empty_log_handler);
			Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_DEBUG, empty_log_handler);
			Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_INFO, empty_log_handler);
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
		Log.set_handler ("GlobalMenu", LogLevelFlags.LEVEL_MASK, default_log_handler);
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

		switch(Environment.get_prgname()) {
			case "gnome-panel":
			case "GlobalMenu.PanelApplet":
			case "gdm-user-switch-applet":
			message(_("GlobalMenu is disabled for several programs"));
			return true;
			default:
				if((disabled_application_names!=null) 
					&& disabled_application_names.str(Environment.get_prgname())!=null){
					message(_("GlobalMenu is disabled for applications in GTK_MENUBAR_NO_MAC list"));
					return true;
				}
			break;
		}
		return false;
	}
}
