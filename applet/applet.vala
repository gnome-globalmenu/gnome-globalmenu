using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;

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
		root_window = new Gnomenu.Window.from_gdk_window(Gdk.get_default_root_window());
	}

	construct {
		set_name("GlobalMenuPanelApplet");
		add_events(Gdk.EventMask.KEY_PRESS_MASK);

		menubars = new MenuBars();
		menubars.visible = true;
		add(menubars);

		selector = new Gnomenu.MenuBar();
		/*Put stuff into the selector?*/
		Parser.parse(selector, SELECTOR.printf("NONE"));
		selector.visible = false; /* Because it is a dummy */

		main_menubar = new Gnomenu.MenuBar();
		main_menubar.activate += (menubar, item) => {
			if(current_window != null) {
				current_window.emit_menu_event(item.path);
			}
		};
		main_menubar.min_length = 0;
		menubars.add(selector);
		menubars.add(main_menubar);
		menubars.child_set(main_menubar, "expand", true, null);	
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

	private MenuBars menubars;
	/*convenient names, should be replaced by direct access to menubars.xxxx*/
	private Gnomenu.MenuBar main_menubar;
	private Gnomenu.MenuBar overflower;
	private Gnomenu.MenuBar selector;

	private void init_wnck() {
		screen.active_window_changed += (screen, previous_window) => {
			weak Wnck.Window window = screen.get_active_window();
			if((window != previous_window) && (window is Wnck.Window)) {
				weak Wnck.Window transient_for = window.get_transient();
				if(transient_for != null) window = transient_for;
				Parser.parse(selector, SELECTOR.printf(window.get_xid().to_string()));
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
				}
				current_window = new Gnomenu.Window.foreign(window.get_xid());

				if(current_window.invalid) {
					current_window.destroy();
					current_window = null; 
					/* TODO: switch to default_window, 
					 * and continue rather than return
					 * Need a little ccode to obtain the desktop
					 * window.*/
				}
				if(current_window != null) {
					current_window.menu_context_changed += (current_window) => {
						update_menubar();
					};
				}
				update_menubar();
			}
		};
	
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

}


