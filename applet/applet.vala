using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using WnckCompat;
using Panel;
public extern GLib.Object gnome_program_init_easy(string name, string version,
		string[] args, GLib.OptionContext #context);
public class Applet : Panel.Applet{
static const string FACTORY_IID = "OAFIID:GlobalMenu_PanelApplet_Factory";
static const string APPLET_IID = "OAFIID:GlobalMenu_PanelApplet";
	private Wnck.Screen screen;
	private Gnomenu.MenuBar menubar;
	private Gtk.Box box;
	public Applet() {
	}
	construct {
		this.set_name("GlobalMenuPanelApplet");
		menubar = new Gnomenu.MenuBar();
		menubar.set_name("PanelMenuBar");
		box = new Gtk.HBox(false, 0);
		menubar.show_tabs = false;
		box.pack_start(menubar, true, true, 0);
		this.add(box);
		screen = Wnck.Screen.get_default();
		(screen as WnckCompat.Screen).active_window_changed += (screen, previous_window) => {
			weak Wnck.Window window = (screen as Wnck.Screen).get_active_window();
			if((window != previous_window) && (window is Wnck.Window)) {
				weak Wnck.Window transient_for = window.get_transient();
				if(transient_for != null) window = transient_for;
				string xid = window.get_xid().to_string();
				menubar.switch(xid);
			}
		};
		this.set_flags(Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.HAS_HANDLE | Panel.AppletFlags.EXPAND_MAJOR );
		(this as PanelCompat.Applet).change_background += (applet, bgtype, color, pixmap) => {
			Gtk.Style style = (this.menubar.get_style() as GtkCompat.Style).copy();
			switch(bgtype){
				case Panel.AppletBackgroundType.NO_BACKGROUND:
					this.menubar.set_style(null);
					this.menubar.queue_draw();
					return;
				break;
				case Panel.AppletBackgroundType.COLOR_BACKGROUND:
					style.bg_pixmap[(int)StateType.NORMAL] = null;
					style.bg[(int)StateType.NORMAL] = color;
				break;
				case Panel.AppletBackgroundType.PIXMAP_BACKGROUND:
					style.bg_pixmap[(int)StateType.NORMAL] = pixmap;
				break;
			}
			this.menubar.set_style(style);
			this.menubar.queue_draw();
		};
	}
	private static bool verbose = false;
	const OptionEntry[] options = {
		{"verbose", 'v',0, OptionArg.NONE, ref verbose, "Show debug messages from GMarkupDoc and Gnomenu", null},
		{null}
	};
	public static int main(string[] args) {
		GLib.OptionContext context = new GLib.OptionContext("- GlobalMenu.PanelApplet");
		context.set_help_enabled (true);
		context.add_main_entries(options, null);
		GLib.Object program = gnome_program_init_easy(
			"GlobalMenu.PanelApplet",
			"0.6", args, #context);
		if(!verbose) {
			LogFunc handler = (domain, level, message) => { };
			Log.set_handler ("GMarkupDoc", LogLevelFlags.LEVEL_DEBUG, handler);
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
				if(iid == APPLET_IID) {
					applet.show_all();
					return true;
				} else return false;
			}) ;	
		return retval;
	}

}


