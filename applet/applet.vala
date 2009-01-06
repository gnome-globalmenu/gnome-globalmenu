using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;
using GConf;

public class Applet : Panel.Applet {
	public static const string IID = "OAFIID:GlobalMenu_PanelApplet";

	public Applet() {
	}
	public override void dispose() {
		if(!disposed) {
			disposed = true;
			set_background_widget(null);
			if(current_window != null) {
				current_window.destroy();
				current_window = null;
			}
		}
		base.dispose();	
	}
	~Applet() {
	}

	private void ensure_monitor() {
		/* this is to workaround the issue that
		 * screen_changed is invoked by GTK before
		 * the instance constructor is invoked */
		if(monitor == null) {
			monitor = new Monitor();
			monitor.window_changed += on_window_changed;
		}
	}
	construct {
		disposed = false;
		set_name("GlobalMenuPanelApplet");
		add_events(Gdk.EventMask.KEY_PRESS_MASK);

		ensure_monitor();
		menubars = new MenuBarBox();
		menubars.visible = true;
		add(menubars);

		selector = new Switcher();
		selector.visible = true;
		menubars.add(selector);
		setup_popup_menu(selector);

		main_menubar = new Gnomenu.MenuBar();
		main_menubar.min_length = 0;  /*Then it will have a overflown item*/
		main_menubar.activate += (menubar, item) => {
			if(current_window != null) {
				current_window.emit_menu_event(item.path);
			}
		};
		setup_popup_menu(main_menubar);

		menubars.add(main_menubar);
		menubars.child_set(main_menubar, "expand", true, null);	/*Let the main_menubar use the remaining space in the applet.*/


		/*init panel */
		flags = (Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.EXPAND_MAJOR );
		set_background_widget(this);

		Gdk.Color color;
		Gdk.Pixmap pixmap;
		AppletBackgroundType bgtype;
		bgtype = get_background(out color, out pixmap);
		(this as Panel.Applet).change_background(bgtype, color, pixmap);
	}

	private Gnomenu.Window current_window;
	private Gnomenu.Window root_window;

	private Monitor monitor;
	private MenuBarBox menubars;
	private bool disposed;
	private Gnomenu.MenuBar main_menubar;
	private Switcher selector;

	public override void screen_changed(Gdk.Screen previous_screen) {
		if(previous_screen != null) {
			ungrab_menu_bar_key(root_window);
			root_window.destroy();
			root_window = null;
		}
		Gdk.Screen gdk_screen = get_screen();
		if(gdk_screen != null) {
			root_window = Gnomenu.Window.new_from_gdk_window(gdk_screen.get_root_window());
			grab_menu_bar_key(root_window);
			Wnck.Screen screen = gdk_screen_to_wnck_screen(gdk_screen);
			ensure_monitor();
			monitor.screen = screen;
		}
	}
	private void on_window_changed (Monitor monitor, Wnck.Window? previous_window) {
		weak Wnck.Window window = monitor.current_window;
		if(window is Wnck.Window)
			switch_to(window);
	}
	private void switch_to(Wnck.Window? window) {
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
			selector.current_window = window;
		}
		if(current_window != null) {
			current_window.menu_context_changed += (current_window) => {
				update_menubar();
			};
		}
		update_menubar();
	}
	private void ungrab_menu_bar_key(Gnomenu.Window window) {
		int keyval = (int) window.get_data("menu-bar-keyval");
		Gdk.ModifierType mods = 
			(Gdk.ModifierType) window.get_data("menu-bar-keymods");

		window.ungrab_key(keyval, mods);
		window.key_press_event -= on_menu_bar_key;
		window.set_data("menu-bar-keyval", null);
		window.set_data("menu-bar-keymods", null);
	}
	private void grab_menu_bar_key(Gnomenu.Window window) {
		/*FIXME: listen to changes in GTK_SETTINGS.*/
		int keyval;
		Gdk.ModifierType mods;
		get_accel_key(out keyval, out mods);
		window.grab_key(keyval, mods);
		window.key_press_event += on_menu_bar_key; 
		window.set_data("menu-bar-keyval", (void*) keyval);
		window.set_data("menu-bar-keymods", (void*) mods);
	}	
	private bool on_menu_bar_key (Gnomenu.Window window, Gdk.EventKey event) {
		uint keyval;
		Gdk.ModifierType mods;
		get_accel_key(out keyval, out mods);
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
				bg.pixmap = this.style.bg_pixmap[(int)StateType.NORMAL];
				bg.color = this.style.bg[(int)StateType.NORMAL];
				if (bg.pixmap==null)
					bg.type = BackgroundType.COLOR; else
					bg.type = BackgroundType.PIXMAP;
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
		setup_menu (applet_menu_xml, verbs, this);

		get_prefs();

	}
	private void get_prefs() {
		selector.max_size = gconf_get_int("title_max_width");
		selector.show_icon = gconf_get_bool("show_icon");
		selector.show_label = gconf_get_bool("show_name");
		selector.show_window_actions = gconf_get_bool("show_window_actions");
		selector.show_window_list = gconf_get_bool("show_window_list");
	}
	private static void on_about_clicked (BonoboUI.Component component,
                                          void* user_data, string cname) {
		Applet _this = (Applet) user_data;
		
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
		Applet _this = (Applet) user_data;
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

		Applet _this = (Applet) user_data;

		GConfDialog gcd = new GConfDialog("Applet preferences");
		
		gcd.add_key("/apps/gnome_settings_daemon/gtk-modules/globalmenu-gnome");
		gcd.add_subkeys(_this.get_preferences_key(),
							new string[]{
								"show_icon",
								"show_name",
								"title_max_width",
								"show_window_actions",
								"show_window_list"});

		gcd.run();
		gcd.destroy();
    }
	/**
	 * return the accelerator key combination for invoking menu bars
	 * in GTK Settings. It is usually F10.
	 */
	private static void get_accel_key(out uint keyval, out Gdk.ModifierType mods) {
		Settings settings = Settings.get_default();
		string accel = null;
		settings.get("gtk_menu_bar_accel", &accel, null);
		if(accel != null)
			Gtk.accelerator_parse(accel, out keyval, out mods);
	}
	public override bool button_press_event(Gdk.EventButton event) {
		if(event.button == 3)
		return control.do_popup(event.button, event.time);
		return false;
	}

	private void setup_popup_menu(Gtk.Widget widget) {
		widget.button_press_event += (widget, event) => {
			if(event.button == 3) {
				(this as Gtk.Widget).button_press_event(event);
				return true;
			}
			else return false;
		};
	}
}


/**
 * :vim:ts=4:sw=4:
 */
