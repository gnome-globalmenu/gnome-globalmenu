using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;

public class Applet : Panel.Applet {
	public static const string IID = "OAFIID:GlobalMenu_PanelApplet";
	public static int instance_count = 0;
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
		instance_count--;
	}
	construct {
		instance_count++;
		this.set_name("GlobalMenuPanelApplet");
		this.add_events(Gdk.EventMask.KEY_PRESS_MASK);
		menubars = new MenuBars();
		menubars.visible = true;
		this.add(menubars);
		selector = menubars.selector;
		Parser.parse(selector, SELECTOR.printf("NONE"));

		main_menubar = menubars.menubar;
		main_menubar.activate += (menubar, item) => {
			if(current_window != null) {
				current_window.emit_menu_event(item.path);
			}
		};

		/*init wnck*/
		screen = Wnck.Screen.get_default();
		screen.active_window_changed += (screen, previous_window) => {
			weak Wnck.Window window = screen.get_active_window();
			if((window != previous_window) && (window is Wnck.Window)) {
				weak Wnck.Window transient_for = window.get_transient();
				if(transient_for != null) window = transient_for;
				Parser.parse(selector, SELECTOR.printf(window.get_xid().to_string()));
				if(current_window != null) {
					/* This is a weird way to free a window:
					 * We have two reference counts for current_window
					 * Destroy will release the one held by GTK,
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
					/*TODO: switch to default_window, and continue rather than return*/
				}
				if(current_window != null) {
					current_window.menu_context_changed += (current_window) => {
						update_menubar();
					};
				}
				update_menubar();
			}
		};

		/*init panel*/
		this.flags = (Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.HAS_HANDLE | Panel.AppletFlags.EXPAND_MAJOR );
		set_background_widget(this);

	/* Key grab F10 (gtk-menu-bar-key)*/
		
		root_window = new Gnomenu.Window.from_gdk_window(Gdk.get_default_root_window());
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
				selector.select_first(true);
				this.key_press_event(event);
			return false;
			}
			return true;
		};
	}

	private Wnck.Screen screen;
	private Gnomenu.Window current_window;
	private Gnomenu.Window root_window;

	private MenuBars menubars;
	/*convenient names, should be replaced by direct access to menubars.xxxx*/
	private Gnomenu.MenuBar main_menubar;
	private Gnomenu.MenuBar overflower;
	private Gnomenu.MenuBar selector;

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


