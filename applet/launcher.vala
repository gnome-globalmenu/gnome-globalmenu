
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
				menu.append(app.get_proxy_item() as Gtk.MenuItem);
			}
			menu.show_all();
		};
	}
	private class Menu : Gtk.Menu {
		static construct {
			Gnomenu.Application.init();
			Wnck.Screen.get_default().force_update();
		}

	}
}
