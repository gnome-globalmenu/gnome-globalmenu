using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;

private class Applet : Panel.Applet {
	static const string FACTORY_IID = "OAFIID:GlobalMenu_PanelApplet_Factory";
	static const string APPLET_IID = "OAFIID:GlobalMenu_PanelApplet";
	public Applet() {
		int i = 0;
	}
	construct {
		this.set_name("GlobalMenuPanelApplet");
		menubar = new Gnomenu.MenuBar();
		menubar.set_name("PanelMenuBar");
		box = new Gtk.HBox(false, 0);
		box.pack_start(menubar, true, true, 0);
		Parser.parse(menubar,
"""
<menu>
	<item label="File">
		<menu>
			<item label="New">
				<menu>
					<item label="Zombie"/>
					<item label="Worm"/>
				</menu>
			</item>
		</menu>
	</item>
	<item label="Edit">
		<menu>
			<item label="Shoot"/>
			<item label="Run"/>
		</menu>
	</item>
	<item label="Help">
		<menu>
			<item label="Police"/>
			<item label="Army"/>
		</menu>
	</item>
</menu>
"""
);
		this.add(box);
	
	
		/*init wnck*/
		screen = Wnck.Screen.get_default();
		screen.active_window_changed += (screen, previous_window) => {
			weak Wnck.Window window = screen.get_active_window();
			if((window != previous_window) && (window is Wnck.Window)) {
				weak Wnck.Window transient_for = window.get_transient();
				if(transient_for != null) window = transient_for;
			}
		};

		/*init panel*/
		this.set_flags(Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.HAS_HANDLE | Panel.AppletFlags.EXPAND_MAJOR );
	}
	private override void change_background(AppletBackgroundType type, ref Gdk.Color? color, Gdk.Pixmap? pixmap) {
		switch(type){
			case Panel.AppletBackgroundType.NO_BACKGROUND:
			break;
			case Panel.AppletBackgroundType.COLOR_BACKGROUND:
			break;
			case Panel.AppletBackgroundType.PIXMAP_BACKGROUND:
			break;
		}
	}
	private override void change_orient(AppletOrient orient) {
		switch(orient) {
			case AppletOrient.UP:
				menubar.gravity = Gravity.DOWN;
				menubar.pack_direction = PackDirection.LTR;
				menubar.child_pack_direction = PackDirection.LTR;
			break;
			case AppletOrient.DOWN:
				menubar.gravity = Gravity.DOWN;
				menubar.pack_direction = PackDirection.LTR;
				menubar.child_pack_direction = PackDirection.LTR;
			break;
			case AppletOrient.LEFT:
				menubar.gravity = Gravity.LEFT;
				menubar.pack_direction = PackDirection.TTB;
				menubar.child_pack_direction = PackDirection.TTB;
			break;
			case AppletOrient.RIGHT:
				menubar.gravity = Gravity.RIGHT;
				menubar.pack_direction = PackDirection.BTT;
				menubar.child_pack_direction = PackDirection.BTT;
			break;
		}
	
	}
	private Wnck.Screen screen;
	private Gnomenu.MenuBar menubar;
	private Gtk.Box box;
	private static bool verbose = false;
	const OptionEntry[] options = {
		{"verbose", 'v',0, OptionArg.NONE, ref verbose, "Show debug messages from GMarkupDoc and Gnomenu", null},
		{null}
	};
	static const string STANDARD_PROPERTIES = "";
	public static int main(string[] args) {
		GLib.OptionContext context = new GLib.OptionContext("- GlobalMenu.PanelApplet");
		context.set_help_enabled (true);
		context.add_main_entries(options, null);
		Gnome.Program program = Gnome.Program.init (
				"GlobalMenu.PanelApplet", "0.7", 
				Gnome.libgnomeui_module, 
				args, 
				Gnome.PARAM_GOPTION_CONTEXT, context,
				Gnome.CLIENT_PARAM_SM_CONNECT, false,
				STANDARD_PROPERTIES,
				null);

		if(!verbose) {
			LogFunc handler = (domain, level, message) => { };
			Log.set_handler ("GMarkup", LogLevelFlags.LEVEL_DEBUG, handler);
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


