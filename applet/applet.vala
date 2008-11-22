using GLib;
using Gtk;
using Gdk;
using Gnomenu;
using Wnck;
using WnckCompat;
using Panel;
using PanelCompat;

public extern GLib.Object gnome_program_init_easy(string name, string version,
		string[] args, GLib.OptionContext #context);
private class Applet : PanelCompat.Applet {
static const string FACTORY_IID = "OAFIID:GlobalMenu_PanelApplet_Factory";
static const string APPLET_IID = "OAFIID:GlobalMenu_PanelApplet";

	private Wnck.Screen screen;
	private Gnomenu.MenuBar menubar;
	private Gtk.Box box;

	private Gtk.Label label;
	private Gdk.Pixbuf icon;
	private Gtk.EventBox eb;
	static const string GCONF_ROOT_KEY = "/apps/gnome-globalmenu-applet";

	public Applet() {
		int i = 0;
	}

	public static void message(string msg) {
		Gtk.MessageDialog m = new Gtk.MessageDialog(null,
							    Gtk.DialogFlags.MODAL,
							    Gtk.MessageType.INFO,
							    Gtk.ButtonsType.OK,
							    msg);
		m.run();
		m.destroy();
	}
	private void update_by_gconf() {

	}

	private void app_selected(Gtk.ImageMenuItem item) {
		if (((item.user_data as Wnck.Window).is_active()) && ((item.user_data as Wnck.Window).is_visible_on_workspace((item.user_data as Wnck.Window).get_workspace()))) {
			(item.user_data as Wnck.Window).minimize();
			return;
		}

		// Ensure viewport visibility
		int current_workspace_x = (item.user_data as Wnck.Window).get_workspace().get_viewport_x();
		int current_workspace_y = (item.user_data as Wnck.Window).get_workspace().get_viewport_y();
		int x,y,w,h;
		(item.user_data as WnckCompat.Window).get_geometry(out x, out y, out w, out h);
		(item.user_data as Wnck.Window).get_screen().move_viewport(current_workspace_x + x, current_workspace_y + y);
		
		(item.user_data as Wnck.Window).activate(Gtk.get_current_event_time());
		(item.user_data as Wnck.Window).get_workspace().activate(Gtk.get_current_event_time());
		(item.user_data as Wnck.Window).unminimize(Gtk.get_current_event_time());
		
		// ensure is on top
		(item.user_data as Wnck.Window).make_above();
		(item.user_data as Wnck.Window).unmake_above();

		//TOFIX: if the window is on another workspace and it is minimized, it doesn't unminimize automatically.
	}
	private bool app_name_pressed(Gtk.EventBox eventbox, Gdk.EventButton event) {
		if (event.button!=1) return false;
		Gtk.Menu menu = new Gtk.Menu();
		weak GLib.List<Wnck.Window> windows = screen.get_windows();
		foreach(weak Wnck.Window window in windows) {
			if (!window.is_skip_pager()) {
				Gtk.ImageMenuItem mi;
				mi = new Gtk.ImageMenuItem.with_label(window.get_name());
				if (window.is_active()) (mi.child as Gtk.Label).set_markup_with_mnemonic("<b>" + (mi.child as Gtk.Label).text + "</b>");			
				mi.set_image(new Gtk.Image.from_pixbuf(window.get_mini_icon()));
				mi.user_data = window;
				mi.activate += app_selected;
				mi.show_all();
				menu.insert(mi, 0);
			}
		}
		if (menu.get_children().length()==0) return true;
		menu.show_all();
		menu.popup(null, null, null, event.button, event.time);
		return true;
	}
	construct {
		this.set_name("GlobalMenuPanelApplet");
		menubar = new Gnomenu.MenuBar();
		menubar.set_name("PanelMenuBar");
		box = new Gtk.HBox(false, 0);
		menubar.show_tabs = false;

		string menu_definition = 
		    "<popup name=\"button3\">" +
		        "<menuitem debuname=\"About\" verb=\"About\" _label=\"_About...\" pixtype=\"stock\" pixname=\"gnome-stock-about\"/>" +
		    "</popup>";

		label = new Gtk.Label("<b>GlobalMenu</b>");
		label.use_markup = true;
		label.visible = true;
			
		eb = new EventBox();
		eb.set_visible_window(true);
		eb.set_size_request(label.width_request,label.height_request);
		eb.add(label);
		eb.button_press_event += app_name_pressed;
		box.pack_start(eb, false, true, 0);

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

				string aname = "Desktop";
				if (xid!=this.menubar.find_default()) aname = window.get_application().get_name();
				label.set_markup ("<b>" + aname + " </b>");
			}
		};
		this.set_flags(Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.HAS_HANDLE | Panel.AppletFlags.EXPAND_MAJOR );
		(this as PanelCompat.Applet).change_background += (applet, bgtype, color, pixmap) => {
			Gtk.Style style = (Gtk.rc_get_style(this.menubar) as GtkCompat.Style).copy();
			switch(bgtype){
				case Panel.AppletBackgroundType.NO_BACKGROUND:
					//Gtk.Style def_style = Gtk.rc_get_style(this.menubar);
					Gtk.Style def_style = Gtk.rc_get_style(this);
					this.menubar.set_style(def_style);
					this.menubar.queue_draw();

					this.eb.set_style(def_style);
					this.eb.queue_draw();

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

			this.eb.set_style(style);
			this.eb.queue_draw();
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


