using Gtk;
using Config;
const string FACTORY_IID = "OAFIID:GlobalMenu_PanelApplet_Factory";

public bool verbose = false;
private const OptionEntry[] options = {
	{"verbose", 'v',0, OptionArg.NONE, ref verbose, N_("Be verbose"), null},
	{null}
};

public int main(string[] args) {
	Environment.set_variable("GLOBALMENU_GNOME_ARGS", "--disable", true);
	Intl.bindtextdomain (Config.GETTEXT_PACKAGE, Config.LOCALEDIR);
 	Intl.bind_textdomain_codeset (Config.GETTEXT_PACKAGE, "UTF-8");
 	Intl.textdomain (GETTEXT_PACKAGE);
	GLib.OptionContext context = new GLib.OptionContext("- GlobalMenu.PanelApplet");
	context.set_help_enabled (true);
	context.add_main_entries(options, null);
	context.add_group(Gtk.get_option_group(true));
	context.add_group(Bonobo.Activation.get_goption_group());

	try {
	context.parse(ref args);
	} catch(GLib.Error e) {
		error("parsing options failed: %s", e.message);	
	}

	if(!verbose) {
		LogFunc handler = (domain, level, message) => { };
		Log.set_handler ("libgnomenu", LogLevelFlags.LEVEL_DEBUG, handler);
		Log.set_handler (null, LogLevelFlags.LEVEL_DEBUG, handler);
	}

	Gtk.rc_parse_string("""
		style "globalmenu_event_box_style"
		{
			GtkWidget::focus-line-width=0
			GtkWidget::focus-padding=0
		}
		style "globalmenu_menu_bar_style"
		{
			ythickness = 0
			GtkMenuBar::shadow-type = none
			GtkMenuBar::internal-padding = 0
		}
		class "GtkEventBox" style "globalmenu_event_box_style"
		class "GnomenuMenuBar" style:highest "globalmenu_menu_bar_style"
""");

	Gtk.init(ref args);
	if(!Bonobo.init(ref args.length, args)) {
		error("Cannot initialize bonobo.");
	}

	int retval = Panel.Applet.factory_main(FACTORY_IID, typeof(Applet), 
		(applet, iid) => {
			if(iid == Applet.IID) {
				(applet as Applet).init();
				applet.show();
				return true;
			} else return false;
		}) ;	
	return retval;
}
