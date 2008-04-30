using GLib;
namespace Gnomenu {

[DBus (name = "org.gnomenu.MenuItem", signals ="propChanged")]
public class MenuItem: MenuOwner {
	public weak Menu parent {get; construct;}
	public int pos {get; construct;}
	public string title {get; set;}

	public int activate(){
		activated();
		return 0;
	}

	public signal void activated();

	public MenuItem(weak Menu parent, string name, int pos) {
		this.name = name;
		this.title = name;
		this.path = parent.path + "/" + name;
		this.pos = pos; 
		this.parent = parent;
	}
	
	construct {
		parent.insert(this, pos);
	}
	public string getTitle() {
		return title;
	}
	public string getMenu() {
		return menu.path;
	}
}

}
