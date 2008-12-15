using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;
using GConf;
public extern int system(string? cmd);

public class Applet : Panel.Applet {
	public static const string IID = "OAFIID:GlobalMenu_PanelApplet";
	static const string SELECTOR = 
"""
<menu>
	<item label="%s" font="bold">
		<menu>
			<item label="Notepad"/>
			<item label="Terminal"/>
		</menu>
	</item>
</menu>
""";

	public Applet() {
	}
	~Applet() {
	}
	static construct {
		screen = Wnck.Screen.get_default();
		root_window = Gnomenu.Window.new_from_gdk_window(Gdk.get_default_root_window());
	}

	construct {
		set_name("GlobalMenuPanelApplet");
		add_events(Gdk.EventMask.KEY_PRESS_MASK);

		menubars = new MenuBarBox();
		menubars.visible = true;
		add(menubars);

		selector = new Gnomenu.MenuBar();
		/*Put stuff into the selector?*/
		Parser.parse(selector, SELECTOR.printf("Globalmenu"));
		selector.visible = true;
		menubars.add(selector);

		main_menubar = new Gnomenu.MenuBar();
		main_menubar.min_length = 0;  /*Then it will have a overflown item*/
		main_menubar.activate += (menubar, item) => {
			if(current_window != null) {
				current_window.emit_menu_event(item.path);
			}
		};
		menubars.add(main_menubar);
		menubars.child_set(main_menubar, "expand", true, null);	/*Let the main_menubar use the remaining space in the applet.*/

		/*init wnck*/
		init_wnck();

	/* Key grab F10 (gtk-menu-bar-key)*/
		grab_gtk_menu_bar_key();

		/*init panel */
		this.flags = (Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.HAS_HANDLE | Panel.AppletFlags.EXPAND_MAJOR );
		set_background_widget(this);
		/* gconf stuff goes to a ::create method?*/
		Gdk.Color color;
		Gdk.Pixmap pixmap;
		AppletBackgroundType bgtype;
		bgtype = get_background(out color, out pixmap);
		(this as Panel.Applet).change_background(bgtype, color, pixmap);
	}

	private static Wnck.Screen screen;
	private Gnomenu.Window current_window;
	private static Gnomenu.Window root_window;

	private MenuBarBox menubars;

	private Gnomenu.MenuBar main_menubar;
	private Gnomenu.MenuBar selector;

	/* to be removed */
	public static void message(string msg) {
		Gtk.MessageDialog m = new Gtk.MessageDialog(null,
							    Gtk.DialogFlags.MODAL,
							    Gtk.MessageType.INFO,
							    Gtk.ButtonsType.OK,
							    msg);
		m.run();
		m.destroy();
	}
	
	private void init_wnck() {
		screen.window_closed += (screen, window) => {
			ulong xid = window.get_xid();
			if(current_window != null && xid == current_window.xid) {
				/* if the closed window is current window,
				 * fallback to the desktop and
				 * wait for the next active_window_changed event
				 *
				 * To solve the haunting menu bar of closed window
				 * problem.
				 * */
				switch_to(find_desktop(screen)); 
			}
		};
		screen.active_window_changed += (screen, previous_window) => {
			weak Wnck.Window window = screen.get_active_window();
			if((window != previous_window) && (window is Wnck.Window)) {
				weak Wnck.Window transient_for = window.get_transient();
				if(transient_for != null) window = transient_for;
				switch(window.get_window_type()) {
					case Wnck.WindowType.NORMAL:
					case Wnck.WindowType.DESKTOP:
						switch_to(window);
					break;
					default:
						/*Do nothing if it is a toolbox or so*/
					break;
				}
			}
		};
	
	}
	private void switch_to(Wnck.Window? window) {
		Parser.parse(selector, SELECTOR.printf(window.get_application().get_name()));
		if(current_window != null) {
			/* This is a weird way to free a window:
			 * We have two reference counts for current_window
			 * Destroy will release the one held by GTK( including
			 * all circular references),
			 * and the assignment line below will release the one
			 * held by us.
			 * */
			current_window.destroy();
			assert(current_window.ref_count == 1);
			current_window = null;
		}
		if(window != null) {
			current_window = Gnomenu.Window.new_from_native(window.get_xid());
		}
		if(current_window != null) {
			current_window.menu_context_changed += (current_window) => {
				update_menubar();
			};
		}
		update_menubar();
	}
	private Wnck.Window? find_desktop(Wnck.Screen screen) {
		weak List<weak Wnck.Window> windows = screen.get_windows();
		foreach(weak Wnck.Window window in windows) {
			if(window.get_window_type() == Wnck.WindowType.DESKTOP) {
				/*Vala should ref it*/
				return window;
			}
		}
		return null;
	}
	private void grab_gtk_menu_bar_key() {
		/*FIXME: listen to changes in GTK_SETTINGS.*/
		int keyval;
		Gdk.ModifierType mods;
		main_menubar.get_accel_key(out keyval, out mods);
		root_window.grab_key(keyval, mods);
		root_window.key_press_event += (root_window, event) => {
			uint keyval;
			Gdk.ModifierType mods;
			main_menubar.get_accel_key(out keyval, out mods);
			if(event.keyval == keyval &&
				(event.state & Gtk.accelerator_get_default_mod_mask())
				== (mods & Gtk.accelerator_get_default_mod_mask())) {
				/* We chain up to the toplevel key_press_event,
				 * which is listened by all the menubars within
				 * the applet*/
				Gtk.Widget toplevel = get_toplevel();
				if(toplevel != null) 
					toplevel.key_press_event(event);
				return false;
			}
			return true;
		};
	}
	private void update_menubar() {
		if(current_window != null) {
			string context = current_window.menu_context;
			if(context != null) {
				Parser.parse(main_menubar, context);
				main_menubar.show();
				return;
			}
		}
		/* elseever */
		main_menubar.hide();
	}
	private override void change_background(AppletBackgroundType type, Gdk.Color? color, Gdk.Pixmap? pixmap) {
		Background bg = new Background();
		switch(type){
			case Panel.AppletBackgroundType.NO_BACKGROUND:
				bg.type = BackgroundType.NONE;
			break;
			case Panel.AppletBackgroundType.COLOR_BACKGROUND:
				bg.type = BackgroundType.COLOR;
				bg.color = color;
			break;
			case Panel.AppletBackgroundType.PIXMAP_BACKGROUND:
				bg.type = BackgroundType.PIXMAP;
				bg.pixmap = pixmap;
			break;
		}
		menubars.background = bg;
	}
	private override void change_orient(AppletOrient orient) {
		switch(orient) {
			case AppletOrient.UP:
				menubars.gravity = Gravity.DOWN;
				menubars.pack_direction = PackDirection.LTR;
			break;
			case AppletOrient.DOWN:
				menubars.gravity = Gravity.DOWN;
				menubars.pack_direction = PackDirection.LTR;
			break;
			case AppletOrient.LEFT:
				menubars.gravity = Gravity.LEFT;
				menubars.pack_direction = PackDirection.TTB;
			break;
			case AppletOrient.RIGHT:
				menubars.gravity = Gravity.RIGHT;
				menubars.pack_direction = PackDirection.BTT;
			break;
		}
	}
	
	static Applet the_applet;
	static const string APPLET_NAME = _("globalmenu-panel-applet");
	static const string APPLET_ICON = "globalmenu";
	static const string GCONF_SCHEMA_DIR = "/schemas/apps/globalmenu-panel-applet/prefs";
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
		    
	private override void realize() {
		base.realize();
		
		the_applet = this;
		
		/* Connect to gconf */
		this.add_preferences(GCONF_SCHEMA_DIR);
		GConf.Client.get_default().value_changed += (key, value) => {
			this.get_prefs();
		};
		
		string applet_menu_xml = _("""
<popup name="button3">
	<menuitem debuname="Preferences" 
		verb="Preferences" 
		_label="_Preferences" 
		pixtype="stock" 
		pixname="gtk-preferences"/>
	<menuitem debuname="Help" 
		verb="Help" 
		_label="_Help" 
		pixtype="stock" 
		pixname="gtk-help"/>
	<menuitem debuname="About" 
		verb="About" _label="_About..." 
		pixtype="stock" 
		pixname="gtk-about"/>
</popup>
		""");
		    
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
		setup_menu (applet_menu_xml, verbs, null);
	}
	private void get_prefs() {
		/*switcher.show_label = this.gconf_get_bool("show_name");
		switcher.show_icon = this.gconf_get_bool("show_icon");
		switcher.max_size = this.gconf_get_int("title_max_width");
		switcher.show_window_actions = this.gconf_get_bool("show_window_actions");
		switcher.show_window_list = this.gconf_get_bool("show_window_list");
		switcher.refresh();*/
	}
	private static void on_about_clicked (BonoboUI.Component component,
                                          void* user_data, string cname) {
       	var dialog = new Gtk.AboutDialog();
       	dialog.program_name = APPLET_NAME;
		dialog.version = Config.VERSION;
		dialog.website = "http://code.google.com/p/gnome2-globalmenu";
		dialog.website_label = _("Project Home");
		dialog.wrap_license = false;
		dialog.license = Licenses.GPLv2;
		dialog.logo_icon_name = APPLET_ICON;
		dialog.authors = APPLET_AUTHORS;
		dialog.documenters = APPLET_ADOCUMENTERS;
		dialog.set_icon_name("gtk-about");
       	dialog.run();
       	dialog.destroy();
    }
    private static void on_help_clicked (BonoboUI.Component component,
                                          void* user_data, string cname) {
       	var dialog = new Gtk.AboutDialog();
       	dialog.program_name = APPLET_NAME;
		dialog.version = Config.VERSION;
		dialog.website = "http://code.google.com/p/gnome2-globalmenu/w/list";
		dialog.website_label = _("On-line help");
		dialog.logo_icon_name = "gtk-help";
		dialog.set_icon_name("gtk-help");
       	dialog.run();
       	dialog.destroy();
    }
    private static void on_preferences_clicked (BonoboUI.Component component,
                                          void* user_data, string cname) {
       	system("gconf-editor " + the_applet.get_preferences_key() + " &");
		/*GtkExtra.GConfDialog gcd = new GtkExtra.GConfDialog(the_applet.get_preferences_key(), "Applet preferences");
		gcd.run();
		gcd.destroy();*/
    }
}


