using GLib;
using Gtk;
using Gnomenu;

namespace GnomenuGtk{
public class BusAgentGtk: BusAgent {
	private static void clear_menu_shell_callback(Widget widget){
		Gtk.MenuShell shell = (Gtk.MenuShell) (widget.get_parent());
		shell.remove(widget);
	}
	public BusAgentGtk (){
	}
	public void rebuild_menu_shell(MenuShell menu_shell){
		bool visible; // for type casting the dynamic objs

		/*clear the menu*/
		menu_shell.foreach((Gtk.Callback)clear_menu_shell_callback);

		string path = (string) menu_shell.get_data("path");
		if(path == null) return;

		dynamic DBus.Object menu_r = this.get_object(path, "Menu");
		bind_objects(menu_shell, menu_r);
		menu_r.propChanged += menu_prop_changed;

		if(visible = menu_r.getVisible()) menu_shell.show();

		string [] item_paths = menu_r.getMenuItems();
		dynamic DBus.Object[] items = this.get_objects(item_paths, "MenuItem");
		foreach(dynamic DBus.Object item in items){
			Gtk.MenuItem menu_item = new Gtk.MenuItem.with_label(item.getTitle());
			bind_objects(menu_item, item);
			if(visible = item.getVisible()) menu_item.show();
			item.propChanged += item_prop_changed;

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
	public void setup_menu_shell(MenuShell menu_shell, string path){
		menu_shell.set_data("path", path);
		rebuild_menu_shell(menu_shell);
	}
	void item_prop_changed(dynamic DBus.Object sender, string prop_name){
		string sender_path = sender.get_path();
		message("%s.%s is changed", sender_path, prop_name);
	}
	void menu_prop_changed(dynamic DBus.Object sender, string prop_name){
		string sender_path = sender.get_path();
		message("%s.%s is changed", sender_path, prop_name);
		switch(prop_name){
			case "children":
				var local = get_local(sender);
				if(local is MenuShell)
				rebuild_menu_shell((MenuShell)local);
			break;
		}
	}

}
}
