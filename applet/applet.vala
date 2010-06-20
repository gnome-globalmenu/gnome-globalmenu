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
	static const string APPLET_NAME = _("gnome-applet-globalmenu");
	static const string APPLET_ICON = "globalmenu";
	static const string GCONF_SCHEMA_DIR = "/schemas/apps/gnome-applet-globalmenu/prefs";
	
	static const BonoboUI.Verb[] verbs = { 
		{"About", (BonoboUI.VerbFn) applet_menu_clicked, null},
		{"Help", (BonoboUI.VerbFn) applet_menu_clicked, null},
		{"Preferences", (BonoboUI.VerbFn) applet_menu_clicked, null},
		{null, null, null}
	};

	public Applet() { }

	construct {
		settings = Gnomenu.GlobalSettings.get(Gdk.Screen.get_default());
		add_events(Gdk.EventMask.KEY_PRESS_MASK);

		menubars.visible = true;
		add(menubars);

		switcher.visible = true;
		menubars.add(switcher);
		setup_popup_menu(switcher);

		setup_popup_menu(main_menubar);

		menubars.add(main_menubar);
		menubars.add(tiny_menubar);

		main_menubar.visible = true;
		tiny_menubar.visible = false;

		Gtk.MenuItem item = new GlobalMenuItem();
		item.visible = true;
		tiny_menubar.append(item);

		if(main_menubar.active_window != null) {
			switcher.current_window = Wnck.Window.get(main_menubar.active_window.get_xid());
		}
		main_menubar.active_window_changed += on_active_window_changed;
		
		menubars.child_set(main_menubar, "shrink", true, null);

		this.change_background += on_change_background;
		this.change_orient += update_size;
		this.change_size += update_size;

		/*init panel */
		has_handle = false;
		set_background_widget(this);

		Gdk.Color color;
		Gdk.Pixmap pixmap;
		AppletBackgroundType bgtype;
		bgtype = get_background(out color, out pixmap);
		on_change_background(bgtype, color, pixmap);
		this.screen_changed += () => {
			settings.attach_to_screen(this.get_screen());
		};
	}

	private Gnomenu.GlobalSettings settings;
	private MenuBarBox menubars = new MenuBarBox();
	private GlobalMenuBar main_menubar = new GlobalMenuBar();
	private Gnomenu.MenuBar tiny_menubar = new Gnomenu.MenuBar();
	private Switcher switcher = new Switcher();

	private bool initialized = false;

	private bool disposed = false;
	public override void dispose() {
		if(!disposed) {
			disposed = true;
			settings.show_local_menu = true;
			settings.push();
			set_background_widget(null);
		}
		base.dispose();
	}


	public bool disable_module_check { get; set; default = false;}
	private bool _tiny_mode = false;
	public bool tiny_mode {
		get {return _tiny_mode;}
		set {
			_tiny_mode = value;
			tiny_menubar.visible = value;
			main_menubar.visible = !value;
		}
	}
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

	private void on_active_window_changed() {
		if(main_menubar.active_window != null) {
			switcher.current_window = Wnck.Window.get(main_menubar.active_window.get_xid());
		} else {
			switcher.current_window = null;
		}
	}

	private void on_change_background(AppletBackgroundType type, Gdk.Color? color, Gdk.Pixmap? pixmap) {
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
	}

	private void get_prefs() {
		try {
			switcher.max_size = gconf_get_int("title_max_width");
		} catch (GLib.Error e) {}
		try {
			switcher.show_icon = gconf_get_bool("show_icon");
		} catch (GLib.Error e) {}
		try {
			switcher.show_label = gconf_get_bool("show_name");
		} catch (GLib.Error e) {}
		try {
			switcher.show_window_actions = gconf_get_bool("show_window_actions");
		} catch (GLib.Error e) {}
		try {
			switcher.show_window_list = gconf_get_bool("show_window_list");
		} catch (GLib.Error e) {}
		try {
			switcher.enable_search_box = gconf_get_bool("enable_search_box");
		} catch (GLib.Error e) {}
		try {
			Gnomenu.Menu.default_use_rgba_colormap = gconf_get_bool("use_rgba_colormap");
		} catch (GLib.Error e) {}
		try {
			main_menubar.grab_keys = gconf_get_bool("grab_mnemonic_keys");
		} catch (GLib.Error e) {}
		try {
			main_menubar.grab_menu_key = gconf_get_bool("grab_menu_bar_key");
		} catch (GLib.Error e) {}
		try {
			main_menubar.per_monitor_mode = gconf_get_bool("per_monitor_mode");
		} catch (GLib.Error e) {}
		try {
			this.has_handle = gconf_get_bool("has_handle");
		} catch (GLib.Error e) {}
		try {
			this.tiny_mode = gconf_get_bool("tiny_mode");
		} catch (GLib.Error e) {}
		try {
			settings.show_local_menu = gconf_get_bool("show_local_menu");
		} catch (GLib.Error e) {}
		try {
			settings.show_menu_icons = gconf_get_bool("show_menu_icons");
		} catch (GLib.Error e) {}
		settings.push();
	}

	private Gtk.Dialog get_pref_dialog() {
		var gcd = new GConfDialog(_("Global Menu Applet Preferences"));
		
		string root = get_preferences_key();

		gcd.add_key_group(
			_("General Settings"),
			new string[] {
				root + "/show_local_menu",
				root + "/show_menu_icons",
				root + "/per_monitor_mode"
			}
		);

		gcd.add_key_group(
			_("Applet"),
			new string[]{
				root + "/has_handle",
				root + "/tiny_mode",
				root + "/use_rgba_colormap",
				root + "/grab_mnemonic_keys",
				root + "/grab_menu_bar_key"
			}
		);

		gcd.add_key_group(
			_("Switcher"),
			new string[] {
				root + "/show_icon",
				root + "/show_name",
				root + "/title_max_width",
				root + "/show_window_actions",
				root + "/show_window_list",
				root + "/enable_search_box"
			}
		);
		return gcd;
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
		var gcd = get_pref_dialog();
		switch(gcd.run()) {
			case Gtk.ResponseType.HELP:
				show_help();
				break;
		}
		gcd.destroy();
    }
	private new void show_help() {
		try {
		Gtk.show_uri(null, "http://code.google.com/p/gnome2-globalmenu/wiki/HelpCentral",
		Gdk.CURRENT_TIME);
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
