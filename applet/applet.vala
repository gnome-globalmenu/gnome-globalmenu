using GLib;
using Gtk;
using Gnomenu;
using Wnck;
using Panel;

private class Applet : Panel.Applet {
	static const string FACTORY_IID = "OAFIID:GlobalMenu_PanelApplet_Factory";
	static const string APPLET_IID = "OAFIID:GlobalMenu_PanelApplet";
	public PackDirection pack_direction {
		get {
			return _pack_direction;
		}
		set {
			foreach(Gnomenu.MenuBar menubar in internal_children) {
				menubar.pack_direction = value;
				menubar.child_pack_direction = value;
			}
			_pack_direction = value;
			queue_resize();
		}
	}
	public Gnomenu.Gravity gravity {
		get {
			return _gravity;
		}
		set {
			_gravity = value;
			foreach(Gnomenu.MenuBar menubar in internal_children) {
				menubar.gravity = value;
			}
			queue_draw();
		}
	}
	public Applet() {
		int i = 0;
	}
	construct {
		this.set_name("GlobalMenuPanelApplet");

		add_menubar_from_string(
"""
<menu>
	<item label="WindowSelector">
		<menu>
			<item label="Notepad"/>
			<item label="Terminal"/>
		</menu>
	</item>
</menu>
"""
		);
		add_menubar_from_string(
"""
<menu>
	<item label="File">
		<menu>
			<item label="New">
				<menu>
					<item label="Zombie"/>
					<item label="Worm"/>
				</menu>
			</item>
		</menu>
	</item>
	<item label="Edit">
		<menu>
			<item label="Shoot"/>
			<item label="Run"/>
		</menu>
	</item>
	<item label="Help">
		<menu>
			<item label="Police"/>
			<item label="Army"/>
		</menu>
	</item>
</menu>
"""
		);
		/*init wnck*/
		screen = Wnck.Screen.get_default();
		screen.active_window_changed += (screen, previous_window) => {
			weak Wnck.Window window = screen.get_active_window();
			if((window != previous_window) && (window is Wnck.Window)) {
				weak Wnck.Window transient_for = window.get_transient();
				if(transient_for != null) window = transient_for;
			}
		};

		/*init panel*/
		this.flags = (Panel.AppletFlags.EXPAND_MINOR | Panel.AppletFlags.HAS_HANDLE | Panel.AppletFlags.EXPAND_MAJOR );
		set_background_widget(this);
	}
	private override void change_background(AppletBackgroundType type, Gdk.Color? color, Gdk.Pixmap? pixmap) {
		Background bg = new Background();
		switch(type){
			case Panel.AppletBackgroundType.NO_BACKGROUND:
				bg.type = BackgroundType.NONE;
				/*
				style = null;
				RcStyle rc_style = new RcStyle();
				modify_style (rc_style);*/
			break;
			case Panel.AppletBackgroundType.COLOR_BACKGROUND:
				bg.type = BackgroundType.COLOR;
				bg.color = color;
//				modify_bg(StateType.NORMAL, color);
			break;
			case Panel.AppletBackgroundType.PIXMAP_BACKGROUND:
				bg.type = BackgroundType.PIXMAP;
				bg.pixmap = pixmap;
			break;
		}
		foreach(Gnomenu.MenuBar menubar in internal_children) {
			bg.offset_x = menubar.allocation.x - allocation.x;
			bg.offset_y = menubar.allocation.y - allocation.y;
			menubar.background = bg;
		}
	}
	private override void change_orient(AppletOrient orient) {
		switch(orient) {
			case AppletOrient.UP:
				gravity = Gravity.DOWN;
				pack_direction = PackDirection.LTR;
			break;
			case AppletOrient.DOWN:
				gravity = Gravity.DOWN;
				pack_direction = PackDirection.LTR;
			break;
			case AppletOrient.LEFT:
				gravity = Gravity.LEFT;
				pack_direction = PackDirection.TTB;
			break;
			case AppletOrient.RIGHT:
				gravity = Gravity.RIGHT;
				pack_direction = PackDirection.BTT;
			break;
		}
	}
	private override void forall(Gtk.Callback cb, void* data) {
		bool include_internal;

		if(include_internal) {
			foreach(Gnomenu.MenuBar menubar in internal_children) {
				cb(menubar);
			}
		}
		base.forall(cb, data);
	}
	private override void size_request(out Requisition r) {
		r.width = 0;
		r.height = 0;
		Requisition cr;
		switch(pack_direction) {
			case PackDirection.LTR:
			case PackDirection.RTL:
				foreach(Gnomenu.MenuBar menubar in internal_children) {
					menubar.size_request(out cr);
					r.width += cr.width;
					r.height = r.height>cr.height?r.height:cr.height;
				}
			break;
			case PackDirection.BTT:
			case PackDirection.TTB:
				foreach(Gnomenu.MenuBar menubar in internal_children) {
					menubar.size_request(out cr);
					r.height += cr.height;
					r.width = r.width>cr.width?r.width:cr.width;
				}
			break;
		}
	}
	private override void map() {
		base.map();
		foreach(Gnomenu.MenuBar menubar in internal_children) {
			menubar.map();
		}
	}
	private override void size_allocate(Gdk.Rectangle a) {
		allocation = (Allocation) a;
		Requisition cr;
		Allocation ca;
		int x;
		int y;
		int rev_x;
		int rev_y;
		x = 0;
		y = 0;
		rev_x = a.width;
		rev_y = a.height;

		foreach(Gnomenu.MenuBar menubar in internal_children) {
			menubar.get_child_requisition(out cr);
			switch(pack_direction) {
				case PackDirection.LTR:
					ca.x = x;
					ca.y = y;
					ca.width = cr.width;
					ca.height = a.height;
					x += cr.width;
				break;
				case PackDirection.RTL:
					ca.x = rev_x - cr.width;
					ca.y = y;
					ca.width = cr.width;
					ca.height = a.height;
					rev_x -= cr.width;
				break;
				case PackDirection.BTT:
					ca.x = x;
					ca.y = rev_y - cr.height;
					ca.width = a.width;
					ca.height = cr.height;
					rev_y -= cr.height;
				break;
				case PackDirection.TTB:
					ca.x = x;
					ca.y = y;
					ca.width = a.width;
					ca.height = cr.height;
					y += cr.height;
				break;
			}
			menubar.size_allocate((Gdk.Rectangle)ca);
		}
		base.size_allocate(a);
	}
	private Wnck.Screen screen;
	private Gnomenu.MenuBar menubar;

	private Label label; /*Replace with the selector later*/

	private PackDirection _pack_direction;
	private Gnomenu.Gravity _gravity;

	private List<weak Gnomenu.MenuBar> internal_children;
	private static bool verbose = false;
	const OptionEntry[] options = {
		{"verbose", 'v',0, OptionArg.NONE, ref verbose, "Show debug messages from GMarkupDoc and Gnomenu", null},
		{null}
	};
	private void add_menubar_from_string(string str) {
		Gnomenu.MenuBar menubar = new Gnomenu.MenuBar();
		menubar.visible = true;
		menubar.set_name("PanelMenuBar");
		menubar.set_parent(this);

		Parser.parse(menubar, str);
		internal_children.append(menubar);
	}
	static const string STANDARD_PROPERTIES = "";
	public static int main(string[] args) {
		GLib.OptionContext context = new GLib.OptionContext("- GlobalMenu.PanelApplet");
		context.set_help_enabled (true);
		context.add_main_entries(options, null);
		Gnome.Program program = Gnome.Program.init (
				"GlobalMenu.PanelApplet", "0.7", 
				Gnome.libgnomeui_module, 
				args, 
				Gnome.PARAM_GOPTION_CONTEXT, context,
				Gnome.CLIENT_PARAM_SM_CONNECT, false,
				STANDARD_PROPERTIES,
				null);

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


