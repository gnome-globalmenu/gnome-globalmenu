using GLib;
using Gtk;
using Gnomenu;

namespace GnomenuGtk{
public class BusAgentGtk: BusAgent {
	private static void clear_menu_shell_callback(Widget widget){
		Gtk.MenuShell shell = (Gtk.MenuShell) (widget.get_parent());
		shell.remove(widget);
	}
	public BusAgentGtk (DBus.Connection conn, string appname){
		this.conn = conn;
		this.appname = appname;
	}
	public void setup_menu_shell(MenuShell menu_shell, string path){
		dynamic DBus.Object menu_r = this.get_object(path, "Menu");
		dynamic DBus.Object[] items = this.get_objects(menu_r.getMenuItems(), "MenuItem");
		/*clear the menu*/
		bool visible;
		if(visible = menu_r.getVisible()) menu_shell.show();
		menu_shell.foreach((Gtk.Callback)clear_menu_shell_callback);
		foreach(dynamic DBus.Object item in items){
			Gtk.MenuItem menu_item = new Gtk.MenuItem.with_label(item.getTitle());
			if(visible = item.getVisible()){
				menu_item.show();
			}
			menu_item.set_data_full("dbus-obj", item.ref(), g_object_unref);
			menu_shell.append(menu_item);
			string submenu_path = item.getMenu();
			if(submenu_path != null && submenu_path.size() >0) {
				Gtk.Menu submenu = new Gtk.Menu();
				this.setup_menu_shell(submenu, submenu_path);
				menu_item.set_submenu(submenu);
			} else {
				menu_item.activate += (sender) =>{
					dynamic DBus.Object item = (DBus.Object)sender.get_data("dbus-obj");
					item.activate();
				};
			}
		}
	}

}
}
