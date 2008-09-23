using GLib;
using Gdk;
using Gtk;
using Gnomenu;
using XML;

namespace GnomenuGtk {
	private weak Client client() {
		return Singleton.instance().client;
	}
	private weak GtkNodeFactory factory() {
		return Singleton.instance().factory;
	}
	private void bind_widget(Gtk.Widget widget) {
		if(!(widget is Gtk.MenuItem)
		&& !(widget is Gtk.MenuShell)) return;
		if(!(widget is Gtk.MenuBar)) {
			assert(widget.parent != null);
			weak string name = factory().wrap(widget);
			weak string parent = factory().wrap(widget.parent);
			message("add %s to %s", name, parent);
			client().add_widget(parent, name);
		}
		if(widget is Gtk.MenuShell) {
			weak List<weak Gtk.Widget> children = (widget as Gtk.Container).get_children();
			foreach(weak Gtk.Widget child in children) {
				bind_widget(child);
			}
		}
	}
	public void bind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = factory().wrap(window);
		weak string menu_name = factory().wrap(menu);
		client().add_widget(null, window_name);
		client().add_widget(window_name, menu_name);
		bind_widget(menu);
		message("bind_menu %s to %s", menu_name, window_name);
	}
	public void unbind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = factory().wrap(window);
		weak string menu_name = factory().wrap(menu);
		message("unbind_menu %s from %s", menu_name, window_name);
		client().remove_widget(menu_name);
	}
}
