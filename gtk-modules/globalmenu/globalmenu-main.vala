using Gtk;
using GtkAQD;
namespace GnomenuGtk {
	[CCode (cname = "dyn_patch_init")]
	protected extern void dyn_patch_init();
	[CCode (cname="gtk_module_init")]
	public void module_init([CCode (array_length_pos = 0.9)] ref weak string[] args) {
		string disabled_application_names = Environment.get_variable("GTK_MENUBAR_NO_MAC");
		disabled = (Environment.get_variable("GNOMENU_DISABLED")!=null);
		verbose = (Environment.get_variable("GNOMENU_VERBOSE")!=null);
		application_name = Environment.get_prgname();
	
		prepare_log_file();

		if(disabled) {
			message("GTK_MENUBAR_NO_MAC or GNOMENU_DISABLED is set. GlobalMenu is disabled");
			return;
		}
		if(!verbose) {
			/*TODO: disable verbose output*/
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

		dyn_patch_init();

		add_emission_hooks();

		debug("GlobalMenu is enabled");
	}


	private void prepare_log_file() {
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
		Log.set_handler ("GlobalMenuModule", LogLevelFlags.LEVEL_MASK, default_log_handler);
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
}

