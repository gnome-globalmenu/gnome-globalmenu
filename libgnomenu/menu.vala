using GLib;
namespace Gnomenu {


[DBus (name = "org.gnomenu.Menu", signals="propChanged")]
public class Menu: BusObject {
	public weak MenuOwner parent {get; construct;}
	public List<MenuItem> children;
	public string title {get; set;}

	public Menu(weak MenuOwner parent, string name) {
		this.name = name;
		this.path = parent.path + "/" + name;
		this.parent = parent;
		this.title = name;
	}
	construct {
		children = null;
		parent.menu = this;
	}
	public void insert(MenuItem child, int pos){
		this.children.insert(child, pos);
	}
	public string getTitle() {
		return title;
	}
	public string getMenuItems(){
		string [] paths = new string[children.length()];
		int i = 0;
		foreach (MenuItem m in children){
			paths[i] = m.path;
			i++;
		}
		return encode_paths(paths);
	}
	public override void expose() {
		base.expose();
		foreach (MenuItem m in children){
			m.expose();
		}
	}
}

}
