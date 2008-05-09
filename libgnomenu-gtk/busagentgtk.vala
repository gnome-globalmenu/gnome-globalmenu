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

		weak string path = (string) menu_shell.get_data("path");
		if(path == null) return;

		dynamic DBus.Object menu_r = this.get_object(path, "Menu");
		bind_objects(menu_shell, menu_r);
		menu_r.propChanged += menu_prop_changed;

		if(visible = menu_r.getVisible()) menu_shell.show();

		string [] item_paths = menu_r.getMenuItems();
//		dynamic DBus.Object[] items = this.get_objects(item_paths, "MenuItem");
		foreach(string path in item_paths) {
			Gtk.MenuItem item = new Gtk.MenuItem();
			setup_menu_item(item, path);
			menu_shell.append(item);
		}
	}
	public void rebuild_menu_item(Gtk.MenuItem item){
		bool visible; // for type casting the dynamic objs

		weak string path = (string) item.get_data("path");
		message("rebuild path=%s", path);
		if(path == null) return;

		dynamic DBus.Object item_r = this.get_object(path ,"MenuItem");
		bind_objects(item, item_r);
		if(visible = item_r.getVisible()) item.show();
		else item.hide();
		string title = item_r.getTitle();
		Gtk.Label label = (Gtk.Label) item.get_child();
		if(label == null) {
			label = new Gtk.Label("");
			item.add(label);
		} 

		label.set_markup_with_mnemonic(title);
		label.show();
		item_r.propChanged += item_prop_changed;

		string submenu_path = item_r.getMenu();
		if(submenu_path != null && submenu_path.size() >0) {
			Gtk.Menu submenu = new Gtk.Menu();
			this.setup_menu_shell(submenu, submenu_path);
			item.set_submenu(submenu);
		} else {
			item.set_submenu(null);
			item.activate += (sender) =>{
				dynamic DBus.Object item_r = get_remote(sender);
				if(sender.submenu == null) 
				/* working around: if submenu is attached after connecting signal, 
				::activate signal is emitted every time the item is clicked*/
					item_r.activate();
			};
		}

	}
	public void setup_menu_shell(MenuShell menu_shell, string path){
		menu_shell.set_data_full("path", path.ref(), g_free);
		message("MenuShell %s", path);
		rebuild_menu_shell(menu_shell);
	}
	public void setup_menu_item(Gtk.MenuItem item, string path){
		item.set_data_full("path", path.ref(), g_free);
		message("MenuItem %s", path);
		rebuild_menu_item(item);
	}
	void item_prop_changed(dynamic DBus.Object sender, string prop_name){
		string sender_path = sender.get_path();
		Gtk.MenuItem local = (Gtk.MenuItem) get_local(sender);
		message("%s.%s is changed", sender_path, prop_name);
		switch(prop_name){
			case "visible":
				bool visible = sender.getVisible();
				if(visible)
					local.show();
				else local.hide();
			break;
			case "menu":
			case "title":
				rebuild_menu_item(local);
			break;
		}
	}
	void menu_prop_changed(dynamic DBus.Object sender, string prop_name){
		string sender_path = sender.get_path();
		var local = get_local(sender);
		message("%s.%s is changed", sender_path, prop_name);
		switch(prop_name){
			case "children":
				if(local is MenuShell)
				rebuild_menu_shell((MenuShell)local);
			break;
		}
	}

}
}
