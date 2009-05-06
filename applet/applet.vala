using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;
using GConf;

[CCode (cname = "GlobalMenuPanelApplet")]
public class Applet : Panel.Applet {
	public static const string IID = "OAFIID:GlobalMenu_PanelApplet";
	static const string applet_menu_xml_template = """
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
	static string[] subs = {
			"@Preferences@", _("_Preferences"),
			"@Help@", _("_Help"),
			"@About@", _("_About")
		};
	static const string APPLET_NAME = _("globalmenu-panel-applet");
	static const string APPLET_ICON = "globalmenu";
	static const string GCONF_SCHEMA_DIR = "/schemas/apps/globalmenu-panel-applet/prefs";
	
	static const BonoboUI.Verb[] verbs = { 
		{"About", (BonoboUI.VerbFn) applet_menu_clicked, null},
		{"Help", (BonoboUI.VerbFn) applet_menu_clicked, null},
		{"Preferences", (BonoboUI.VerbFn) applet_menu_clicked, null},
		{null, null, null}
	};

	public Applet() { }
	public override void dispose() {
		if(!disposed) {
			disposed = true;
			set_background_widget(null);
		}
		base.dispose();	
	}

	construct {
		add_events(Gdk.EventMask.KEY_PRESS_MASK);

		menubars.visible = true;
		add(menubars);

		switcher.visible = true;
		menubars.add(switcher);
		setup_popup_menu(switcher);

		setup_popup_menu(main_menubar);

		menubars.add(main_menubar);
		menubars.child_set(main_menubar, "shrink", true, null);

		/*init panel */
		has_handle = false;
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
	private bool disposed = false;
	private GlobalMenu main_menubar = new GlobalMenu();
	private Switcher switcher = new Switcher();

	private Notify.Notification notify_no_plugin;

	private bool initialized = false;
	public bool disable_module_check = false;
	private bool _has_handle = false;
	public bool has_handle {
		set {
			if(value) {
				flags = (0
				| Panel.AppletFlags.HAS_HANDLE
				| Panel.AppletFlags.EXPAND_MINOR  
				| Panel.AppletFlags.EXPAND_MAJOR
				);
			} else {
				flags = (0
				| Panel.AppletFlags.EXPAND_MINOR  
				| Panel.AppletFlags.EXPAND_MAJOR
				);
				set_size_hints(null, 0);
			}
			_has_handle = value;
		}
		get {
			return _has_handle;
		}
	}
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
		update_size();
	}
	public override void change_size(uint size) {
		update_size();
	}
	private void update_size() {
		switch(orient) {
			case AppletOrient.UP:
				menubars.gravity = Gravity.DOWN;
				menubars.pack_direction = PackDirection.LTR;
				menubars.child_pack_direction = PackDirection.LTR;
				menubars.set_size_request((int)size, -1);
			break;
			case AppletOrient.DOWN:
				menubars.gravity = Gravity.DOWN;
				menubars.pack_direction = PackDirection.LTR;
				menubars.child_pack_direction = PackDirection.LTR;
				menubars.set_size_request((int)size, -1);
			break;
			case AppletOrient.LEFT:
				menubars.gravity = Gravity.LEFT;
				menubars.pack_direction = PackDirection.TTB;
				menubars.child_pack_direction = PackDirection.TTB;
				menubars.set_size_request(-1, (int)size);
			break;
			case AppletOrient.RIGHT:
				menubars.gravity = Gravity.RIGHT;
				menubars.pack_direction = PackDirection.TTB;
				menubars.child_pack_direction = PackDirection.TTB;
				menubars.set_size_request(-1, (int)size);
			break;
		}
	}
	
		    
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

		    
		string applet_menu_xml = Template.replace(applet_menu_xml_template, subs);

		setup_menu (applet_menu_xml, verbs, this);

		get_prefs();

		initialized = true;
		check_module();
	}

	private void check_module() {
		if(!initialized) return;
		if(disable_module_check) return;

		string modules = Environment.get_variable("GTK_MODULES");
		if(modules == null) modules = "";
		Settings settings = get_settings();
		if(settings != null) {
			modules += settings.gtk_modules;
		}

		if(modules.str("globalmenu") != null) return;

		try {
			Notify.init(APPLET_NAME);
			notify_no_plugin = new Notify.Notification(
				_("No Global Menu?"), 
				_("The Global Menu Plugin is not enabled on this desktop.") +
				_("Enable the plugin by accessing the preferences dialog via a right-click,") +
				_("or by exporting GTK_MODULES=globalmenu-gnome in your profile.")
				, "globalmenu", null);
		
			notify_no_plugin.show();
		} catch (GLib.Error e) {
			/*ignore the error*/
			warning("notify library doesn't work as intended");
		}
	
	}

	private void get_prefs() {
		switcher.max_size = gconf_get_int("title_max_width");
		switcher.show_icon = gconf_get_bool("show_icon");
		switcher.show_label = gconf_get_bool("show_name");
		switcher.show_window_actions = gconf_get_bool("show_window_actions");
		switcher.show_window_list = gconf_get_bool("show_window_list");
		this.has_handle = gconf_get_bool("has_handle");
		this.disable_module_check = gconf_get_bool("disable_module_check");
	}

	[CCode (instance_pos = 1.1)]
	private void applet_menu_clicked (BonoboUI.Component component, 
			string cname) {
		switch(cname) {
			case "About":
				show_about();
			break;
			case "Preferences":
				show_preferences();
			break;
			case "Help":
				show_help();
			break;
		}
    }

	private void show_about() {
       	var dialog = new Gtk.AboutDialog();
       	dialog.program_name = APPLET_NAME;
		string ver = Config.VERSION;
		dialog.version = ver;
		dialog.website = "http://code.google.com/p/gnome2-globalmenu";
		dialog.website_label = _("Project Home");
		dialog.wrap_license = false;
		try {
			string license = null;
			FileUtils.get_contents(Config.DOCDIR + "/COPYING", out license);
			dialog.license = license;
		} catch(FileError e) {
			warning("%s", e.message);
		}
		try {
			string authors = null;
			FileUtils.get_contents(Config.DOCDIR + "/AUTHORS", out authors);
			string[] authors_array = authors.split("\n");
			dialog.authors = authors_array;
		} catch(FileError e) {
			warning("%s", e.message);
		}
		dialog.logo_icon_name = APPLET_ICON;
		dialog.translator_credits = _("translator-credits");
		dialog.set_icon_name("gtk-about");
       	dialog.run();
       	dialog.destroy();
	
	
	}
	private void show_preferences() {
		GConfDialog gcd = new GConfDialog(_("Global Menu Applet Preferences"));
		
		string root = get_preferences_key();

		gcd.add_key_group(
			_("System Preferences"),
			new string[] {
				"/apps/gnome_settings_daemon/gtk-modules/globalmenu-gnome",
				root + "/disable_module_check"
			}
		);

		gcd.add_key_group(
			_("Applet Behaviors"),
			new string[]{
				root + "/has_handle",
				root + "/show_icon",
				root + "/show_name",
				root + "/title_max_width",
				root + "/show_window_actions",
				root + "/show_window_list"
			}
		);

		switch(gcd.run()) {
			case Gtk.ResponseType.HELP:
				show_help();
				break;
		}
		gcd.destroy();
    }
	private void show_help() {
		try {
		Gnome.url_show("http://code.google.com/p/gnome2-globalmenu/wiki/HelpCentral");
		} catch(GLib.Error e) {
			warning("%s", e.message);
		}
	}
	public override bool button_press_event(Gdk.EventButton event) {
		if(event.button == 3)
		return control.do_popup(event.button, event.time);
		return false;
	}

	public override void size_request(out Gtk.Requisition req) {
		base.size_request(out req);	
		unowned int[] hints = menubars.get_size_hints();
		assert(hints.length % 2 == 0);
		if(has_handle) {
			/* This is to workaround a problem with gnome panel.
			 * gnome panel won't handle size_hints correctly if
			 * the applet has no handle.
			 *
			 * Take a look at the sample code of gnome-panel/applets/wncklets
			 * */
			set_size_hints(hints, 0);
		}
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
