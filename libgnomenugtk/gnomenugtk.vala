using GLib;
using Gdk;
using Gtk;
using GtkAQD;
using Gnomenu;
using GMarkupDoc;
using DBus;

namespace GnomenuGtk {
	[CCode (cname = "_patch_menu_bar")]
	private extern  void patch_menu_bar();
	[CCode (cname = "_patch_menu_shell")]
	private extern  void patch_menu_shell();
	[CCode (cname = "_patch_menu_item")]
	private extern  void patch_menu_item();

	private bool hook_func (SignalInvocationHint ihint, [CCode (array_length_pos = 1.9)] Value[] param_values) {
		Gtk.Widget self = param_values[0].get_object() as Gtk.Widget;
		if(self is Gtk.MenuBar) {
			if(ihint.run_type != SignalFlags.RUN_FIRST) return true;
			Gtk.Widget old_toplevel = param_values[1].get_object() as Gtk.Widget;
			Gtk.Widget toplevel = self.get_toplevel();
			if(old_toplevel is Gtk.Widget)
				old_toplevel = old_toplevel.get_ancestor(typeof(Gtk.Window));
			if(toplevel is Gtk.Widget)
				toplevel = toplevel.get_ancestor(typeof(Gtk.Window));
			if(old_toplevel != null) {
				unbind_menu(old_toplevel, self);
			}
			if(toplevel != null && (0 != (toplevel.get_flags() & WidgetFlags.TOPLEVEL))) {
				bind_menu(toplevel, self);
			}
		} 
		return true;
	}
	[CCode (cname="gtk_module_init")]
	public void init([CCode (array_length_pos = 0.9)] ref weak string[] args) {
		DBus.Connection conn;
		try {
			conn = Bus.get(DBus.BusType.SESSION);
		} catch (GLib.Error e) {
			warning("%s", e.message);
			conn = null;
		}
		if(conn == null) {
			message("DBus is unavailable. GlobalMenu is disabled.");
			return;
		}
		if(Environment.get_variable("GTK_MENUBAR_NO_MAC")!=null) {
			message("GTK_MENUBAR_NO_MAC is set. GlobalMenu is disabled");
			return;
		}
		switch(Environment.get_prgname()) {
			case "gnome-panel":
			case "GlobalMenu.PanelApplet":
			case "gdm-user-switch-applet":
			message("GlobalMenu is disabled");
			return;
			break;
		}
		typeof(Gtk.Widget).class_ref();
		uint signal_id = Signal.lookup("hierarchy-changed", typeof(Gtk.Widget));
		Signal.add_emission_hook (signal_id, 0, hook_func, null);
		patch_menu_item();
		patch_menu_shell();
		patch_menu_bar();
		message("GlobalMenu is enabled");
	}
	private weak Client client() {
		return Singleton.instance().client;
	}
	private weak Document document() {
		return Singleton.instance().document;
	}
	private void submenu_notify(Gtk.Widget widget, ParamSpec pspec) {
		weak Gtk.Menu submenu = (widget as Gtk.MenuItem).submenu;
		if(submenu != null) {
			bind_widget(submenu, widget as Gtk.Widget);
		}
	}
	private void child_remove(Gtk.Widget widget, Gtk.Widget c) {
		debug("child_remove");
		unbind_widget(c); /*This might be not so useful, since the node is removed when 
									the MenuShell is disposed*/
	}
	private void child_insert(Gtk.Widget w, Gtk.Widget c, int pos) {
		debug("created widget at %d", pos);
		bind_widget(c, w, pos);
	}
	private void bind_widget(Gtk.Widget widget, Gtk.Widget? parent_widget /*override parent widget*/= null, int pos = -1) {
		if(!(widget is Gtk.MenuItem)
		&& !(widget is Gtk.MenuShell)
		&& !(widget is Gtk.Window)) return;

		weak string name = document().wrap(widget);
		if(document().dict.lookup(name) != null) return;

		weak GMarkupDoc.Node parent_node;
		if(parent_widget == null) {
			parent_node = document().root;
		}
		else {
			weak string parent = document().wrap(parent_widget);
			parent_node = document().dict.lookup(parent);
		}

		weak string type;
		type = "widget";
		if(widget is Gtk.Window)
			type = "window";
		if(widget is Gtk.MenuBar)
			type = "menubar";
		if(widget is Gtk.Menu)
			type = "menu";
		if(widget is Gtk.MenuItem)
			type = "item";
		if(widget is Gtk.CheckMenuItem)
			type = "check";
		if(widget is Gtk.ImageMenuItem)
			type = "imageitem";
		
		Document.Widget node = document().CreateWidget(type, name);
		parent_node.insert(node, pos);

		if(widget is Gtk.MenuShell) {
			weak List<weak Gtk.Widget> children = (widget as Gtk.Container).get_children();
			foreach(weak Gtk.Widget child in children) {
				bind_widget(child, widget, children.index(child));
			}
			(widget as GtkAQD.MenuShell).insert += child_insert;
			(widget as GtkAQD.MenuShell).remove += child_remove;
		}
		if(widget is Gtk.MenuItem) {
			weak Gtk.Menu submenu = (widget as Gtk.MenuItem).submenu;
			if(submenu != null) {
				bind_widget(submenu, widget);
			}
			widget.notify["submenu"] += submenu_notify;
		}
		widget.weak_ref(weak_ref_notify, null);
	}
	protected void unbind_widget(Gtk.Widget widget) {
		if(widget is Gtk.MenuShell) {
			weak List<weak Gtk.Widget> children = (widget as Gtk.Container).get_children();
			foreach(weak Gtk.Widget child in children) {
				unbind_widget(child);
			}
			(widget as GtkAQD.MenuShell).insert -= child_insert;
			(widget as GtkAQD.MenuShell).remove -= child_remove;
		}
		if(widget is Gtk.MenuItem) {
			weak Gtk.Menu submenu = (widget as Gtk.MenuItem).submenu;
			if(submenu != null) {
				unbind_widget(submenu);
			}
			widget.notify["submenu"] -= submenu_notify;
		}
		document().unwrap(widget);
	}
	private void weak_ref_notify(void* data, GLib.Object object){
		unbind_widget(object as Gtk.Widget);
	}
	public void bind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = document().wrap(window);
		weak string menu_name = document().wrap(menu);
		debug("binding menu %s to %s", menu_name, window_name);
		bind_widget(window);
		bind_widget(menu, window);
		if(0 != (window.get_flags() & WidgetFlags.REALIZED)) {
			weak string window_name = document().wrap(window);
			client().register_window(window_name, XWINDOW(window.window).to_string());
		}
		window.realize += (window) => {
			weak string window_name = document().wrap(window);
			client().register_window(window_name, XWINDOW(window.window).to_string());
		};
		window.unrealize += (window) => {
			weak string window_name = document().wrap(window);
			client().unregister_window(window_name);
		};
	}
	public void unbind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = document().wrap(window);
		weak string menu_name = document().wrap(menu);
		debug("unbinding menu %s to %s", menu_name, window_name);
		weak Document.Widget node =document().dict.lookup(menu_name) as Document.Widget;
		if(node != null && node.parent != null) node.parent.remove(node);
	}
}
