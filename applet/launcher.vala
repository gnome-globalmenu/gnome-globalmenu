
public class Launcher : Gnomenu.MenuBar {
	Gtk.MenuItem root_item;
	private const string MENU_TEMPLATE = """<?xml version="1.0" encoding="UTF-8"?>
<menu>
	<item id="root" label="%label%" font="bold"/>
</menu>""";
	private Gtk.Window window = new Gtk.Window(Gtk.WindowType.POPUP);

	private bool duplicated_cancel = false;

	private Gtk.Menu menu = new Menu();
	static construct {
	}
	construct {
		Gnomenu.Parser.parse(this,
		Template.replace(MENU_TEMPLATE,
			new string[] {
				"%label", "Applications"
			}));
		root_item = get_item_by_id("root") as Gtk.MenuItem;
		root_item.submenu = menu;

		this.activate += () => {
			foreach(var widget in menu.get_children()) {
				menu.remove(widget);
			}
			foreach(var app in Gnomenu.Application.applications) {
				if(app.wnck_applications == null) continue;
				ProxyItem item = new ProxyItem();
				item.application = app;
				item.visible = true;
				menu.append(item);
			}
			Gtk.MenuItem more = new Gtk.MenuItem.with_label("more...");
			more.visible = true;
			menu.append(more);
			more.submenu = new Gtk.Menu();
			foreach(var app in Gnomenu.Application.applications) {
				if(app.wnck_applications != null) continue;
				ProxyItem item = new ProxyItem();
				item.application = app;
				item.visible = true;
				item.activate += (obj) => {
					Gdk.spawn_command_line_on_screen(get_screen(),
						obj.application.exec_path);
				};
				more.submenu.append(item);
			}
		};
	}

	private class ProxyItem : Gtk.MenuItem {
		Gtk.Label name_widget = new Gtk.Label("");
		Gtk.Label status_widget = new Gtk.Label("");
		Gtk.Image icon_widget = new Gtk.Image();
		construct {
			Gtk.HBox hbox = new Gtk.HBox(false, 0);
			Gtk.VBox vbox = new Gtk.VBox(false, 0);
			hbox.pack_start(icon_widget, false, false, 0);
			hbox.pack_start(vbox, true, true, 0);
			vbox.pack_start(name_widget, false, false, 0);
			vbox.pack_start(status_widget, false, false, 0);
			hbox.show_all();
			add(hbox);
		}
		private Gnomenu.Application _application;
		public Gnomenu.Application application {
			get {
				return _application;
			}
			set {
				if(_application != null) {
					_application.update -= update;
				}
				_application = value;
				_application.update += update;
				update(value);
			}
		}
		private void update(Gnomenu.Application app) {
			if(app.wnck_applications != null) {
				int n = 0;
				foreach(Wnck.Application wapp in app.wnck_applications) {
					n += wapp.get_n_windows();
				}
				status_widget.label = "%u instances, %d windows"
				.printf(app.wnck_applications.length(), n);
			} else {
				status_widget.label = "not launched";
			}
			name_widget.label = app.readable_name;
			icon_widget.icon_name = app.icon_name;
		}
	}
	private class Menu : Gtk.Menu {
		static construct {
			Gnomenu.Application.init();
			Wnck.Screen.get_default().force_update();
		}

	}
}
