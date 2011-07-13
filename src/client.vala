[DBus (name="org.globalmenu.menu")]
internal class Menu: Object {

	public uint64 xwindow { get; private set; }

	public void get_ui(string path, out string ui) {
		var widget = get_widget(path);
		if(widget is Gtk.MenuShell) {
			widget.show();
			ui = this.serialize(widget as Gtk.MenuShell);
		} else {
			ui = "<empty/>";
		}
	}

	public void emit(string path) {
		var widget = get_widget(path);
		if(widget is Gtk.TearoffMenuItem) {
			return;
		}
		if(widget is Gtk.SeparatorMenuItem) {
			return;
		}
		if(widget is Gtk.MenuItem) {
			(widget as Gtk.MenuItem).activate();
		}
	}

	internal Menu(Gtk.MenuShell shell) {
		this.shell = shell;
		this.shell.hierarchy_changed.connect(
			(prev) => {
				if(prev is Gtk.Window)
					prev.disconnect(realize_handler_id);
					prev.disconnect(unrealize_handler_id);
				var next = this.shell.get_toplevel();
				if(next is Gtk.Window) {
					this.attach_to_toplevel(next);
				}
			}
		);
		var toplevel = this.shell.get_toplevel();
		if(toplevel is Gtk.Window) {
			this.attach_to_toplevel(toplevel);
		}
	}
	internal static void register(Gtk.MenuShell shell) {
		try {
			Menu menu = new Menu(shell);
			Manager manager = Bus.get_proxy_sync(BusType.SESSION, "org.globalmenu.manager", "/org/globalmenu/manager");
			var object_path = "/org/globalmenu/%p".printf(shell);
			var reg_id = Bus.get_sync(BusType.SESSION).register_object(object_path, menu);
			manager.add(object_path);
			var destroy_id = shell.destroy.connect(
				(shell) => {
					Bus.get_sync(BusType.SESSION).unregister_object(reg_id);
					
				}
			);
			shell.set_data<uint>("globalmenu", 1000);
		} catch (IOError e) {
			warning("could not register menu %s\n", e.message);
		} catch (Error e) {
			error("%s\n", e.message);
		}
	}
	internal static bool has_registered(Gtk.MenuShell shell) {
		return (shell.get_data<uint>("globalmenu") != 0);
	}
	internal static void register_all() {
		var l = Gtk.Window.list_toplevels();
		foreach(var window in l) {
			var menubar = widget_by_type(window, typeof(Gtk.MenuBar)) as Gtk.MenuBar;
			if(menubar != null && !Menu.has_registered(menubar)) 
				Menu.register(menubar);
		}
	}

	private Gtk.MenuShell shell;
	private ulong realize_handler_id = 0;
	private ulong unrealize_handler_id = 0;

	private void attach_to_toplevel(Gtk.Widget toplevel) {
		this.realize_handler_id = toplevel.realize.connect(
			(obj) => {
#if GTK_VERSION == 3
				this.xwindow = Gdk.X11Window.get_xid(obj.get_window());
#else
				this.xwindow = Gdk.x11_drawable_get_xid(obj.get_window());
#endif
			}
		);

		this.unrealize_handler_id = toplevel.unrealize.connect(
			(obj) => {
				this.xwindow = 0;
			}
		);
		if(toplevel.get_realized()) {
#if GTK_VERSION == 3
			this.xwindow = Gdk.X11Window.get_xid(toplevel.get_window());
#else
			this.xwindow = Gdk.x11_drawable_get_xid(toplevel.get_window());
#endif
		}

	}

	/* if the path ends with '/' returns the shell, otherwise the item */
	private Gtk.Widget? get_widget(string path) {
		var segs = path.split("/");
		Gtk.MenuShell shell = this.shell;
		Gtk.MenuItem cursor = null;
		for(int i = 0; i < segs.length; i++) {
			if(segs[i].length == 0) continue;
			int loc = int.parse(segs[i]);
			int j = 0;
			cursor = null;
			shell.forall( (item) => {
				if(j == loc) cursor = item as Gtk.MenuItem;
				j++;
			});
			/* if failed to find at any level, return null*/
			if(cursor == null) return null;
			shell = cursor.submenu;
		}
		if(path.has_suffix("/")) {
			return shell;
		}
		return cursor;
	}

	private static string serialize(Gtk.Widget widget) {
		StringBuilder s = new StringBuilder("");
		serialize_to(s, widget);
		return s.str;
	}
	private static void serialize_to(StringBuilder * sb, Gtk.Widget widget) {
		if(widget is Gtk.MenuShell) {
			(widget as Gtk.MenuShell).forall((item) => {
				serialize_to(sb, item);
			});
			return;
		} 
		bool visible = widget.visible;
		bool sensitive = widget.sensitive;
		if(widget is Gtk.SeparatorMenuItem) {
			sb->append_printf("<separator");
		} else if(widget is Gtk.TearoffMenuItem) {
			sb->append_printf("<tearoff");
		} else if(widget is Gtk.MenuItem) {
			var label = widget_by_type(widget, typeof(Gtk.Label)) as Gtk.Label;
			bool has_submenu = ((widget as Gtk.MenuItem).get_submenu() != null);
			
			sb->append_printf("<menuitem label=\"%s\"", label == null?"null":label.get_label());
			if(has_submenu)
				sb->append_printf(" submenu=\"%s\"", has_submenu.to_string());
		} else {
			sb->append_printf("<unknown");
		}
		if(!visible)
			sb->append_printf(" visible=\"%s\"", visible.to_string());
		if(!sensitive)
			sb->append_printf(" sensitive=\"%s\"", sensitive.to_string());
		sb->append_printf("/>");

	}
	private static Gtk.Widget widget_by_type(Gtk.Widget parent, Type type) {
		Gtk.Widget rt = null;
		(parent as Gtk.Container).forall(
			(widget) => {
				if(rt != null) return;
				if(widget.get_type().is_a(type)) {
					rt = widget;
				} else if(widget is Gtk.Container) {
					rt = widget_by_type(widget, type);
				}
			}
		);
		return rt;
	}
}

[DBus (name="org.globalmenu.manager")]
internal interface Manager: Object {
	public abstract void add(string object_path) throws IOError;
}

void callback(Gtk.Action action) {
	print("widget %s called", action.get_name());
}
void main(string[] args) {
	const string uidef = """
<ui>
  <menubar>
    <menu name="FileMenu" action="FileMenu">
      <menuitem name="New" action="New"/>
      <placeholder name="name" />
    </menu>
    <menu name="JustifyMenu" action="JustifyMenu">
      <menuitem name="Left" action="Left"/>
      <menuitem name="Centre" action="Centre"/>
      <menuitem name="Right" action="Right"/>
      <menuitem name="Fill" action="Fill"/>
    </menu>
  </menubar>
</ui>
	""";
	const Gtk.ActionEntry [] ae = {
		{"FileMenu", null, "File", null, null, callback},
		{"JustifyMenu", null, "Justify", null, null, callback},
		{"New", null, "New", null, null, callback},
		{"Left", null, "Left", null, null, callback},
		{"Centre", null, "Centre", null, null, callback},
		{"Right", null, "Right", null, null, callback},
		{"Fill", null, "Fill", null, null, callback}
	};

	Gtk.init(ref args);
	try {
		Gtk.ActionGroup ag = new Gtk.ActionGroup("default");
		ag.add_actions(ae, null);

		Gtk.UIManager uiman = new Gtk.UIManager();
		uiman.insert_action_group(ag, 0);
		uiman.add_ui_from_string(uidef, uidef.length);
		Gtk.MenuBar shell = uiman.get_widget("/menubar") as Gtk.MenuBar;
		Menu.register(shell);
	} catch (IOError e) {
		error("%s\n", e.message);
	}
	new MainLoop().run();
}

