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
public extern string* __get_task_name_by_pid(int pid);

private class Applet : PanelCompat.Applet {
	
	static const string FACTORY_IID = "OAFIID:GlobalMenu_PanelApplet_Factory";
	static const string APPLET_IID = "OAFIID:GlobalMenu_PanelApplet";
	static const string APPLET_NAME = "Global Menu Panel Applet";
	static const string APPLET_VERSION = "0.6";
	static const string APPLET_ICON = "gnome-fs-home";
	static const string[] APPLET_AUTHORS = {"Coding:",
						"Yu Feng <rainwoodman@gmail.com>",
						"Mingxi Wu <fengshenx@gmail.com>",
						"bis0n.lives <bis0n.lives@gmail.com>",
						"Luca Viggiani <lviggiani@gmail.com>",
						"",
						"Packaging:",
						"sstasyuk <sstasyuk@gmail.com>",
						"David Watson <dwatson031@gmail.com>",
						"Valiant Wing <Valiant.Wing@gmail.com>"};
	static const string[] APPLET_ADOCUMENTERS = {"Pierre Slamich <pierre.slamich@gmail.com>"};
	private Wnck.Screen screen;
	private Gnomenu.MenuBar menubar;
	private Gtk.Box box;

	private PanelExtra.Switcher switcher;
	private GLib.HashTable<string,string> switcher_dictionary;
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
    private static void on_about_clicked (BonoboUI.Component component,
                                          void* user_data, string cname) {
       	var dialog = new Gtk.AboutDialog();
       	dialog.program_name = APPLET_NAME;
		dialog.version = APPLET_VERSION;
		dialog.website = "http://code.google.com/p/gnome2-globalmenu";
		dialog.website_label = "Project Home";
		dialog.wrap_license = false;
		dialog.license = GPL.Licenses.V2;
		dialog.logo_icon_name = APPLET_ICON;
		dialog.authors = APPLET_AUTHORS;
		dialog.documenters = APPLET_ADOCUMENTERS;
       	dialog.run();
       	dialog.destroy();
    }
    private static void on_help_clicked (BonoboUI.Component component,
                                          void* user_data, string cname) {
       	var dialog = new Gtk.AboutDialog();
       	dialog.program_name = APPLET_NAME;
		dialog.version = APPLET_VERSION;
		dialog.website = "http://code.google.com/p/gnome2-globalmenu/w/list";
		dialog.website_label = "On-line help";
		dialog.logo_icon_name = "gtk-help";
       	dialog.run();
       	dialog.destroy();
    }
    private static void on_preferences_clicked (BonoboUI.Component component,
                                          void* user_data, string cname) {
       	message("Not yet available...");
    }
    private string remove_path(string txt, string separator) {
    	long co = txt.length-1;
		while ((co>=0) && (txt.substring(co, 1)!=separator)) {
			co--;
		}
		string ret = txt.substring(co+1,(txt.length-co-1));
		return ret;
	}
    private string get_application_name(Wnck.Window window) {
		string txt = __get_task_name_by_pid(window.get_application().get_pid());
		if ((txt==null) || (txt=="")) return window.get_application().get_name();
		string ret = txt.chomp();
		if (ret.substring(ret.length-4,4)==".exe") return remove_path(ret, "\\"); // is a wine program

		ret = remove_path(ret.split(" ")[0], "/");
			
		switch(ret) {
		case "mono":
		case "python":
		case "python2.5":
		case "vmplayer":
			return remove_path(txt.chomp().split(" ")[1], "/");
			break;
		case "wine":
			return window.get_application().get_name();
			break;
		}
		return ret;
	}

	construct {
		this.set_name("GlobalMenuPanelApplet");
		menubar = new Gnomenu.MenuBar();
		menubar.set_name("PanelMenuBar");
		box = new Gtk.HBox(false, 0);
		menubar.show_tabs = false;

		string menu_definition = 
		    "<popup name=\"button3\">" +
		        "<menuitem debuname=\"Preferences\" verb=\"Preferences\" _label=\"_Preferences\" pixtype=\"stock\" pixname=\"gtk-preferences\"/>" +
		        "<menuitem debuname=\"Help\" verb=\"Help\" _label=\"_Help\" pixtype=\"stock\" pixname=\"gtk-help\"/>" +
		     	"<menuitem debuname=\"About\" verb=\"About\" _label=\"_About...\" pixtype=\"stock\" pixname=\"gtk-about\"/>" +
		    "</popup>";
		    
		var verbPreferences = BonoboUI.Verb ();
		verbPreferences.cname = "Preferences";
		verbPreferences.cb = on_preferences_clicked;
		
		var verbAbout = BonoboUI.Verb ();
		verbAbout.cname = "About";
		verbAbout.cb = on_about_clicked;
		
		var verbHelp = BonoboUI.Verb ();
		verbHelp.cname = "Help";
		verbHelp.cb = on_help_clicked;
		
		var verbs = new BonoboUI.Verb[] { verbAbout, verbHelp, verbPreferences };
		setup_menu (menu_definition, verbs, null);

		switcher = new PanelExtra.Switcher();
		box.pack_start(switcher, false, true, 0);

		switcher_dictionary = GnomeMenuHelper.get_flat_list();
		if (switcher_dictionary.lookup("nautilus")==null)
			switcher_dictionary.insert("nautilus", "File Manager"); // To be removed when gconf will be available
		
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
				if (window.get_xid().to_string()!=this.menubar.find_default()) aname = get_application_name(window);
				if (switcher_dictionary.lookup(aname)!=null) 
					aname = switcher_dictionary.lookup(aname); else
					aname = window.get_name();
				switcher.set_label(aname);
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
				
					this.switcher.set_style(def_style);
					this.switcher.queue_draw();
					
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
					
			this.switcher.set_style(style);
			this.switcher.queue_draw();
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
			APPLET_VERSION, args, #context);
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


