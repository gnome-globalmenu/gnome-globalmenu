using GLib;
namespace Gnomenu {


[DBus (name = "org.gnomenu.Menu", signals="propChanged")]
public class Menu: BusObject {
	public List<MenuItem> children;
	public string title {get; set;}

	public Menu(string name) {
		this.name = name;
	}
	construct {
		children = null;
	}
	public void insert(MenuItem child, int pos){
		this.children.insert(child, pos);
		child.parent = this;
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
	[NoArrayLength]
	public string[] getMenuItems2() {
		string [] paths = new string[children.length()+1];
		var i = 0;
		foreach(MenuItem m in children){
			paths[i] = m.path;
			message("%s", m.path);
			i++;
		}	
		paths[i] = null;
		message("length = %d", i);
		return paths;
	}
	public override void expose() {
		base.expose();
		foreach (MenuItem m in children){
			m.expose();
		}
	}
	public override void reset_path() {
		base.reset_path();
		foreach (MenuItem m in children){
			m.reset_path();
		}
	}
	public override bool getVisible() {
		return base.getVisible();
	}
	public override string getTitle() {
		return base.getTitle();
	}
}

}
