using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;
using GConf;

[CCode (cname = "GlobalMenuPanelApplet")]
public class Applet : Panel.Applet {
	public static const string IID = "OAFIID:GlobalMenu_PanelApplet";

	public Applet() { }
	public override void dispose() {
		if(!disposed) {
			disposed = true;
			set_background_widget(null);
		}
		base.dispose();	
	}
	~Applet() {
	}

	construct {
		disposed = false;
		add_events(Gdk.EventMask.KEY_PRESS_MASK);

		menubars.visible = true;
		add(menubars);

		switcher.visible = true;
		menubars.add(switcher);
		setup_popup_menu(switcher);

		main_menubar.min_length = 12;  /*Then it will have a overflown item*/
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

		monitor.window_changed += on_window_changed;
		on_window_changed(monitor, 0);
	}


	private Monitor monitor = new Monitor();
	private MenuBarBox menubars = new MenuBarBox();
	private bool disposed;
	private GlobalMenu main_menubar = new GlobalMenu();
	private Switcher switcher = new Switcher();

	private Notify.Notification notify_no_plugin;
	public override void screen_changed(Gdk.Screen? previous_screen) {
		Gdk.Screen screen = get_screen();
		if(previous_screen != null) {
			Settings old_settings = Settings.get_for_screen(previous_screen);
			/* Work around an old vala bug on disconnecting signals
			 * perhaps already fixed but ...*/
			old_settings.notify -= check_module;
		}
		if(screen != null) {
			check_module();
			monitor.screen = screen;
			get_settings().notify["gtk-modules"] += check_module;
		}
	}

	private void on_window_changed (Monitor monitor, ulong prev_xid) {
		ulong xid = monitor.current_xid;
		Wnck.Window window = Wnck.Window.get(xid);
		if(window is Wnck.Window) {
			switcher.current_window = window;
		}
		main_menubar.switch_to(xid);
	}
	public override void change_background(AppletBackgroundType type, Gdk.Color? color, Gdk.Pixmap? pixmap) {
		Background bg = new Background();
		switch(type){
			case Panel.AppletBackgroundType.NO_BACKGROUND:
				/*Don't think this is still applicable,
				 * and it causes Issue 314; With MAC-OSX theme,
				 * the pixmap is 0x01 -- how could GTK allow this
				 * nonsense pixmap and won't fail?

				bg.pixmap = this.style.bg_pixmap[(int)StateType.NORMAL];
				bg.color = this.style.bg[(int)StateType.NORMAL];
				if (bg.pixmap==null)
					bg.type = BackgroundType.COLOR; else
					bg.type = BackgroundType.PIXMAP;

			*********/
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
						"Valiant Wing <Valiant.Wing@gmail.com>",
						null};
	
	static const string[] APPLET_ADOCUMENTERS = {
		"Pierre Slamich <pierre.slamich@gmail.com>", 
		null};

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
		GConf.Client client = GConf.Client.get_default();

		if(client != null) {
			client.value_changed += (key, value) => {
				this.get_prefs();
			};
		}	

		string applet_menu_xml_template = """
<popup name="button3">
	<menuitem name="Preferences" 
		verb="Preferences" 
		_label="@Preferences@"
		pixtype="stock" 
		pixname="gtk-preferences"/>
	<menuitem name="Help" 
		verb="Help" 
		_label="@Help@"
		pixtype="stock" 
		pixname="gtk-help"/>
	<menuitem name="About" 
		verb="About" 
		_label="@About@"
		pixtype="stock" 
		pixname="gtk-about"/>
</popup>
		""";
		    
		string[] subs = {
			"@Preferences@", _("_Preferences"),
			"@Help@", _("_Help"),
			"@About@", _("_About")
		};
		string applet_menu_xml = Template.replace(applet_menu_xml_template, subs);

		setup_menu (applet_menu_xml, verbs, this);

		get_prefs();
	
	}

	private void check_module() {
		Settings settings = get_settings();
		if(settings == null) return;
		string modules = settings.gtk_modules;
			
		if(modules != null && modules.str("globalmenu") != null) {
			return;
		}
		
		if(notify_no_plugin == null) {
			if(!Notify.is_initted()) {
				if(!Notify.init(APPLET_NAME)) {
					critical("libnotify is not initialized");	
					return;
				}
			}
			notify_no_plugin = new Notify.Notification("No Global Menu?", 
							"Global Menu Plugin is not enabled on this Desktop. "
							+ "Enable the plugin from the preferences dialog in the right-click menu.", "globalmenu", null);
		}
		try {
		notify_no_plugin.show();
		} catch (GLib.Error e) {
			/*ignore the error*/
		}
	
	}
	public override void realize() {
		base.realize();

	}
	private void get_prefs() {
		switcher.max_size = gconf_get_int("title_max_width");
		switcher.show_icon = gconf_get_bool("show_icon");
		switcher.show_label = gconf_get_bool("show_name");
		switcher.show_window_actions = gconf_get_bool("show_window_actions");
		switcher.show_window_list = gconf_get_bool("show_window_list");
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
		dialog.translator_credits = _("translator-credits");
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
		dialog.website = "http://code.google.com/p/gnome2-globalmenu/wiki/HelpCentral";
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
