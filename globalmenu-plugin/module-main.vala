
private bool verbose = false;
private bool disabled = false;
private bool initialized = false;
private Patcher patcher;
private string log_file_name = null;
private FileStream log_stream;
private Quark domain;

[CCode (cname="gtk_module_init")]
public void gtk_entry([CCode (array_length_pos = 0.9)] ref string[] args) {
	if(disabled) return;
	Gdk.threads_add_idle_full(Priority.HIGH, 
		() => {
			if(initialized) return false;

			initialized = true;
			patcher = new Patcher();
			MenuBarAgentFactory.init();
			MenuBarAgentFactory.get().prepare_attached_menubars();
			return false;
		}
	);
}

[CCode (cname="g_module_check_init")]
public string? glib_entry(Module module) {
	domain = Quark.from_string("GlobalMenu:Plugin");

	if(is_quirky_app()) disabled = true;

	parse_args();

/* Do not write any log messages before we prepare the log file.*/
	if(verbose) {
		log_stream = FileStream.open(log_file_name, "a+");
		Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_DEBUG, write_log);
	} else {
		Log.set_handler (domain.to_string(), LogLevelFlags.LEVEL_DEBUG, suppress_log);
	}

	debug("Global Menu Version: %s", Config.VERSION);

	if(disabled) {
		return "Global Menu is disabled";
	}

	debug("Global Menu is enabled");

	/* So that the module is never unloaded once.
	 * unloading the module causes problem with GType info
	 * after 0.7.9.
	 * */
	module.make_resident();
	return null;

}

private static const OptionEntry [] options = {
	{"verbose", 'v', 0, OptionArg.NONE, ref verbose, N_("Be verbose"), null},
	{"disable", 'd', 0, OptionArg.NONE, ref disabled, N_("Disable the Plugin"), null},
	{"log-file", 'l', 0, OptionArg.FILENAME, ref log_file_name, N_("File to save the log, default to ~/.gnomenu.log"), null},
	{null}
};

private static bool parse_args() {
	string [] args = null;
	string env = Environment.get_variable("GLOBALMENU_GNOME_ARGS");
	if(env == null) return true;

	string command_line = "globalmenu-gnome " + env;
	/* set default log file name */
	log_file_name = Environment.get_home_dir() + "/.gnomenu.log";
	try {
		Shell.parse_argv(command_line, out args);
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
		context.parse(ref args);
	} catch(GLib.Error e) {
		return false;
	}
	return true;
}

private static void write_log(string? domain, LogLevelFlags level, string message) {
	TimeVal time = {0};
	time.get_current_time();
	string s = "%.10ld | %20s | %10s | %s\n".printf(time.tv_usec, Environment.get_prgname(), domain, message);
	log_stream.puts(s);
	log_stream.flush();
}
private static void suppress_log(string? domain, LogLevelFlags level, string message) {}

private static bool is_quirky_app() {
	string list = 
		Environment.get_variable("GTK_MENUBAR_NO_MAC");

	string app_name = Environment.get_prgname();
	/* Don't use switch case because vala will create
	 * static quarks which cause core dumps when 
	 * the module is unloaded */

	if((list != null) && list.str(app_name)!=null)
		return true;

	return false;
}
