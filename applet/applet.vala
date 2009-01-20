using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;
using GConf;

[CCode (cname = "GlobalMenuPanelApplet")]
public class Applet : Panel.Applet {
	public static const string IID = "OAFIID:GlobalMenu_PanelApplet";

	public Applet() {
	}
	public override void dispose() {
		if(!disposed) {
			disposed = true;
			set_background_widget(null);
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
		add_events(Gdk.EventMask.KEY_PRESS_MASK);

		ensure_monitor(); 
		/* 
		 * already ensured by screen_changed signal, 
		 * leave it to remind us the monitor is already there.
		*/
		menubars = new MenuBarBox();
		menubars.visible = true;
		add(menubars);

		selector = new Switcher();
		selector.visible = true;
		menubars.add(selector);
		setup_popup_menu(selector);

		main_menubar = new Gnomenu.MenuBar();
		main_menubar.min_length = 12;  /*Then it will have a overflown item*/
		monitor.menubar = main_menubar;
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


	private Monitor monitor;
	private MenuBarBox menubars;
	private bool disposed;
	private Gnomenu.MenuBar main_menubar;
	private Switcher selector;

	private Notify.Notification notify;
	public override void screen_changed(Gdk.Screen previous_screen) {
		Gdk.Screen gdk_screen = get_screen();
		if(gdk_screen != null) {
			ensure_monitor();
			monitor.screen = gdk_screen_to_wnck_screen(gdk_screen);
		}
	}

	private void on_window_changed (Monitor monitor, Wnck.Window? previous_window) {
		weak Wnck.Window window = monitor.current_window;
		if(window is Wnck.Window)
			selector.current_window = monitor.current_window;
	}
	public override void change_background(AppletBackgroundType type, Gdk.Color? color, Gdk.Pixmap? pixmap) {
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
	public override void change_orient(AppletOrient orient) {
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

	static const BonoboUI.Verb[] verbs = { 
			{"About", on_about_clicked, null},
			{"Help", on_help_clicked, null},
			{"Preferences", on_preferences_clicked, null},
			{null, null, null}
		};
		    
	public void init() {
		/* Connect to gconf */
		try {
			this.add_preferences(GCONF_SCHEMA_DIR);
		} catch (GLib.Error e) {
			warning("%s", e.message );
		}
		GConf.Client.get_default().value_changed += (key, value) => {
			this.get_prefs();
		};
		
		string applet_menu_xml = _("""
<popup name="button3">
	<menuitem name="Preferences" 
		verb="Preferences" 
		_label="_Preferences" 
		pixtype="stock" 
		pixname="gtk-preferences"/>
	<menuitem name="Help" 
		verb="Help" 
		_label="_Help" 
		pixtype="stock" 
		pixname="gtk-help"/>
	<menuitem name="About" 
		verb="About" _label="_About..." 
		pixtype="stock" 
		pixname="gtk-about"/>
</popup>
		""");
		    
		setup_menu (applet_menu_xml, verbs, this);

		get_prefs();
	
		if(!Notify.is_initted())
			Notify.init(APPLET_NAME);
	}

	public override void realize() {
		base.realize();
		notify = new Notify.Notification("No Global Menu?", 
				"Global Menu Plugin is not enabled on this Desktop. "
				+ "Enable the plugin from the preferences dialog in the right-click menu.", "globalmenu", this);
		if(null == get_settings().gtk_modules.str("globalmenu")) {
			notify.show();
		}
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
//		Applet _this = (Applet) user_data;
		
       	var dialog = new Gtk.AboutDialog();
       	dialog.program_name = APPLET_NAME;
		string ver = Config.VERSION;
		if(Config.SVNVERSION.length > 0 ){
			ver += ".svn" + Config.SVNVERSION;
		}
		dialog.version = ver;
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
//		Applet _this = (Applet) user_data;
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
