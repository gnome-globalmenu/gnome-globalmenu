using GLib;
using Gdk;
using Gtk;
using Gnomenu;
using GtkAQD;
using XML;

namespace GnomenuGtk {
	private weak Client client() {
		return Singleton.instance().client;
	}
	private weak NodeFactory factory() {
		return Singleton.instance().factory;
	}
	private void bind_widget(Gtk.Widget widget, void* parent_widget /*override parent widget*/= null, int pos = -1) {
		if(!(widget is Gtk.MenuItem)
		&& !(widget is Gtk.MenuShell)) return;
		weak Gtk.Widget pw;
		if(parent_widget == null) pw = widget.parent;
		else pw = (Gtk.Widget) parent_widget;
		weak string name = factory().wrap(widget);
		weak string parent = factory().wrap(pw);
		message("add %s to %s", name, parent);
		client().add_widget(parent, name, pos);
		if(widget is Gtk.MenuShell) {
			weak List<weak Gtk.Widget> children = (widget as Gtk.Container).get_children();
			foreach(weak Gtk.Widget child in children) {
				bind_widget(child, widget, children.index(child));
			}
			(widget as GtkAQD.MenuShell).insert += (w, c, pos) => {
				bind_widget(c, w, pos);
			};
			(widget as GtkAQD.MenuShell).remove += (w, c) => {
				client().remove_widget(factory().wrap(c)); /*This might be not so useful, since the node is removed when 
											the MenuShell is disposed*/
			};
		}
		if(widget is Gtk.MenuItem) {
			weak Gtk.Menu submenu = (widget as Gtk.MenuItem).submenu;
			if(submenu != null) {
				bind_widget(submenu, widget);
			}
			widget.notify["submenu"] += (widget, pspec) => {
				weak Gtk.Menu submenu = (widget as Gtk.MenuItem).submenu;
				if(submenu != null) {
					bind_widget(submenu, widget as Gtk.Widget);
				}
			};
		}
	}
	public void bind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = factory().wrap(window);
		weak string menu_name = factory().wrap(menu);
		client().add_widget(null, window_name);
		bind_widget(menu, window);
		message("bind_menu %s to %s", menu_name, window_name);
	}
	public void unbind_menu(Gtk.Widget window, Gtk.Widget menu) {
		weak string window_name = factory().wrap(window);
		weak string menu_name = factory().wrap(menu);
		message("unbind_menu %s from %s", menu_name, window_name);
		client().remove_widget(menu_name);
	}
}
