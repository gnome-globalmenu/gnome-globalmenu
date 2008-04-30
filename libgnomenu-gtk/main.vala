using GLib;
using Gtk;
using Gnomenu;

namespace Gnomenu{
	public static void clear_menu_shell_callback(Widget widget){
		Gtk.MenuShell shell = (Gtk.MenuShell) (widget.get_parent());
		shell.remove(widget);
	}
	public void setup_menu_shell(MenuShell menu_shell, Gnomenu.BusAgent agent, string path){
		dynamic DBus.Object menu_r = agent.get_object(path, "Menu");
		dynamic DBus.Object[] items = agent.get_objects(menu_r.getMenuItems(), "MenuItem");
		/*clear the menu*/
		menu_shell.foreach((Gtk.Callback)clear_menu_shell_callback);
		foreach(dynamic DBus.Object item in items){
			Gtk.MenuItem menu_item = new Gtk.MenuItem.with_label(item.getTitle());
			menu_shell.append(menu_item);
			string submenu_path = item.getMenu();
			if(submenu_path != null && submenu_path.size() >0) {
				Gtk.Menu submenu = new Gtk.Menu();
				setup_menu_shell(submenu, agent, submenu_path);
				menu_item.set_submenu(submenu);
			}
		}
	}

}
