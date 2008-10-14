using GLib;
using Gdk;
using Gtk;
using GtkAQD;
using Gnomenu;
using GMarkupDoc;
using DBus;

namespace GnomenuGtk {
	[CCode (cname = "_patch_menu_bar")]
	protected extern  void patch_menu_bar();
	[CCode (cname = "_patch_menu_shell")]
	protected extern  void patch_menu_shell();
	[CCode (cname = "_patch_menu_item")]
	protected extern  void patch_menu_item();

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
	protected weak string translate_gtk_type(Gtk.Widget widget) {
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
		return type;
	}
	private void transverse(Gtk.Widget head, Document.Widget rel_root, int pos = -1) {
		weak Gtk.Widget gtk = head;
		assert(gtk is Gtk.Widget);
		assert(rel_root is Document.Widget);
		weak Document.Widget node = document().wrap(gtk);
		if(gtk is Gtk.MenuShell) {
			rel_root.insert(node, pos);
			foreach(weak Gtk.Widget child in (gtk as Gtk.Container).get_children()) {
				transverse(child, node);
			}
			(gtk as GtkAQD.MenuShell).insert += child_insert;
			(gtk as GtkAQD.MenuShell).remove += child_remove;
		}
		if(gtk is Gtk.MenuItem) {
			rel_root.insert(node, pos);
			weak Gtk.Menu submenu = (gtk as Gtk.MenuItem).submenu;
			if(submenu != null) {
				transverse(submenu, node);
			}
			gtk.notify["submenu"] += submenu_notify;
		}
	}
	private void submenu_notify(Gtk.Widget widget, ParamSpec pspec) {
		weak Gtk.Menu submenu = (widget as Gtk.MenuItem).submenu;
		weak Document.Widget node = document().wrap(widget);
		List<weak Document.Widget> list = node.children.copy();
		foreach(weak Document.Widget child in list) {
			node.remove(child);
		}
		if(submenu != null) {
			transverse(submenu, node);
		}
	}
	private void child_remove(Gtk.Widget widget, Gtk.Widget child) {
		weak Document.Widget node = document().wrap(widget);
		weak Document.Widget child_node = document().wrap(child);
		node.remove(child_node);
	}
	private void child_insert(Gtk.Widget widget, Gtk.Widget child, int pos) {
		weak Document.Widget node = document().wrap(widget);
		transverse(child, node, pos);
	}
	public void bind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak Document.Widget node = document().wrap(window);
		if(document().root.index(node) < 0) {
			document().root.append(node);
			if(0 != (window.get_flags() & WidgetFlags.REALIZED)) {
				weak Document.Widget node = document().wrap(window);
				client().register_window(node.name, XWINDOW(window.window).to_string());
			}
			window.realize += (window) => {
				weak Document.Widget node = document().wrap(window);
				client().register_window(node.name, XWINDOW(window.window).to_string());
			};
			window.unrealize += (window) => {
				weak Document.Widget node = document().wrap(window);
				client().unregister_window(node.name);
			};
		}
		weak Document.Widget menu_node = document().wrap(menu);
		debug("binding menu %s to %s", menu_node.name, node.name);
		/*TODO: transverse menu_node, adding children*/
		transverse(menu, node);
	}
	public void unbind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak Document.Widget node = document().wrap(window);
		weak Document.Widget menu_node = document().wrap(menu);
		debug("unbinding menu %s to %s", menu_node.name, node.name);
		node.remove(menu_node);
	}
}
