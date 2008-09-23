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
	public void bind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = factory().wrap(window);
		weak string menu_name = factory().wrap(menu);
		client().add_widget(null, window_name);
		client().add_widget(window_name, menu_name);
		message("bind_menu %s to %s", menu_name, window_name);
	}
	public void unbind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = factory().wrap(window);
		weak string menu_name = factory().wrap(menu);
		message("unbind_menu %s from %s", menu_name, window_name);
		client().remove_widget(menu_name);
	}
}
