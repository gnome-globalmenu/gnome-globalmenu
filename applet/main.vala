using Gtk;
using Config;
const string APPLET_STANDARD_PROPERTIES = ""; /* then replaced by patch.sh to GNOME_STANDARD_PROPERTies*/
const string FACTORY_IID = "OAFIID:GlobalMenu_PanelApplet_Factory";

public bool verbose = false;
const OptionEntry[] options = {
	{"verbose", 'v',0, OptionArg.NONE, ref verbose, N_("Show debug messages from GMarkupDoc and Gnomenu"), null},
	{null}
};

public int main(string[] args) {
	GLib.OptionContext context = new GLib.OptionContext("- GlobalMenu.PanelApplet");
	context.set_help_enabled (true);
	context.add_main_entries(options, null);
	Gnome.Program program = Gnome.Program.init (
			"GlobalMenu.PanelApplet", Config.VERSION, 
			Gnome.libgnomeui_module, 
			args, 
			Gnome.PARAM_GOPTION_CONTEXT, context,
			Gnome.CLIENT_PARAM_SM_CONNECT, false,
			APPLET_STANDARD_PROPERTIES,
			null);

	if(!verbose) {
		LogFunc handler = (domain, level, message) => { };
		Log.set_handler ("Gnomenu", LogLevelFlags.LEVEL_DEBUG, handler);
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

	int retval = Panel.Applet.factory_main(FACTORY_IID, typeof(Applet), 
		(applet, iid) => {
			if(iid == Applet.IID) {
				applet.show();
				return true;
			} else return false;
		}) ;	
	return retval;
}
