using GLib;
namespace Gnomenu {

[DBus (name = "org.gnomenu.MenuItem", signals ="propChanged")]
public class MenuItem: MenuOwner {

	public int activate(){
		message("%s is activated", path);
		activated();
		return 0;
	}

	public signal void activated();

	public MenuItem(string name) {
		this.name = name;
	}
	
	public override bool getVisible() {
		return base.getVisible();
	}
	public override string getTitle() {
		return base.getTitle();
	}
	public override string getMenu() {
		return base.getMenu();
	}
}

}
