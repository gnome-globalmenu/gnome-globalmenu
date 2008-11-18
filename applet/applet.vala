using GLib;
using Gtk;
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
	private Gtk.Label window_title;
	private Gtk.Box box;
	private Gtk.EventBox event_box;

	public Applet() {
		int i = 0;
	}
	private bool changing_menubar_background;
	private void change_menubar_background() {
		changing_menubar_background = true;
		Gtk.Widget widget = menubar;
		Gtk.Style entire_style = get_style();
		Gdk.Pixmap entire_bg = entire_style.bg_pixmap[(int)StateType.NORMAL];
		Gtk.Allocation allocation = widget.allocation;
		Gtk.Style child_style = (widget.get_style() as GtkCompat.Style).copy();

		if(entire_bg != null) {
			int entire_width;
			int entire_height;
			int x;
			int y;
			int width;
			int height;
			entire_bg.get_size(out entire_width, out entire_height);
			if(allocation.width > entire_width) {
				x = 0;
				width = entire_width;
			} else {
				x = allocation.width;
				width = entire_width;	
			}
			if(allocation.height > entire_height) {
				y = 0;
				height = entire_height;
			} else {
				y = 0;
				height = entire_height;
			}
			Gdk.Pixmap child_bg = new Gdk.Pixmap(entire_bg, width, height, -1);
			Gdk.GC gc = new Gdk.GC(child_bg);
			child_bg.draw_drawable(gc, entire_bg, 
						x, y, 
						0, 0, 
						-1, -1);
			child_style.bg_pixmap[(int)StateType.NORMAL] = child_bg;
			child_style.bg[(int)StateType.NORMAL] = entire_style.bg[(int)StateType.NORMAL];
		} else {
			child_style.bg[(int)StateType.NORMAL] = entire_style.bg[(int)StateType.NORMAL];
			child_style.bg_pixmap[(int)StateType.NORMAL] = null;
		}
		widget.set_style(child_style);
		widget.queue_draw();
	}
	private void change_window_title_format() {
		/*FIXME: get these values from GConf*/
		int window_title_max_width = 15;
		string window_title_font_string = "bold";
		window_title.max_width_chars = window_title_max_width;
		Pango.FontDescription font = PangoCompat.FontDescription.from_string(window_title_font_string);
		message("font %s", font.to_string());
		Pango.Layout layout = window_title.get_layout();
		layout.set_font_description(font);
	}
	construct {
		this.set_name("GlobalMenuPanelApplet");
		menubar = new Gnomenu.MenuBar();
		menubar.set_name("PanelMenuBar");
		menubar.show_tabs = false;

		window_title = new Gtk.Label("");

		box = new Gtk.HBox(false, 0);
		box.pack_start(window_title, false, false, 0);
		box.pack_start(menubar, true, true, 0);
		this.add(box);

		screen = Wnck.Screen.get_default();

		menubar.size_allocate += (menubar, allocation) => {
			message("size sset");
			if(changing_menubar_background) {
				changing_menubar_background = false;
				return;
			}
			this.change_menubar_background();
		};

		(this as GtkCompat.Widget).style_set += (applet, old_style) => {
			this.change_menubar_background();
		};
		(screen as WnckCompat.Screen).active_window_changed += (screen, previous_window) => {
			weak Wnck.Window window = (screen as Wnck.Screen).get_active_window();
			if((window != previous_window) && (window is Wnck.Window)) {
				weak Wnck.Window transient_for = window.get_transient();
				if(transient_for != null) window = transient_for;
				string xid = window.get_xid().to_string();
				xid = menubar.switch(xid);
				string title;
				if(xid != null)	{
					weak Wnck.Window real_window = Wnck.Window.get(xid.to_ulong());
					if(real_window != null) {
						string app_name = real_window.get_application().get_icon_name();
						title = real_window.get_icon_name();
						if(app_name.size() < title.size()) {
							title = app_name;
						}
					} else {
						title = null;
					}
				}
				window_title.set_text(title);
				change_window_title_format();
				window_title.queue_resize();
			}
		};
		this.set_flags(Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.HAS_HANDLE | Panel.AppletFlags.EXPAND_MAJOR );
		(this as PanelCompat.Applet).change_background += (applet, bgtype, color, pixmap) => {
			Gtk.Style style = (Gtk.rc_get_style(this) as GtkCompat.Style).copy();
			switch(bgtype){
				case Panel.AppletBackgroundType.NO_BACKGROUND:
				break;
				case Panel.AppletBackgroundType.COLOR_BACKGROUND:
					style.bg_pixmap[(int)StateType.NORMAL] = null;
					style.bg[(int)StateType.NORMAL] = color;
				break;
				case Panel.AppletBackgroundType.PIXMAP_BACKGROUND:
					style.bg_pixmap[(int)StateType.NORMAL] = pixmap;
				break;
			}
			this.set_style(style);
		};

		this.add_preferences("/schemas/apps/gnome-globalmenu-applet/prefs");
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


