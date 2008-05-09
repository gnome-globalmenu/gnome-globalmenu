using GLib;
namespace Gnomenu {


[DBus (name = "org.gnomenu.Menu", signals="propChanged")]
public class Menu: BusObject {
	public List<weak MenuItem> children;
	public string title {get; set;}

	public Menu(string name) {
		this.name = name;
	}
	construct {
		children = null;
	}
	public void insert(MenuItem # child, int pos){ /*Takes the ownership*/
		this.children.insert(child, pos);
		child.ref(); /*work around vala in-consistence in ownership transfer*/
		child.parent = this;
		prop_changed("children");
	}
	public void remove(MenuItem child){
		if(this.children.find(child) != null){
			child.parent = null;
			this.children.remove_all(child);
			child.unref();
			prop_changed("children");
		}
	}
	~Menu(){
		foreach(MenuItem item in this.children){
			item.unref();
		}
	}
	[NoArrayLength]
	public string[] getMenuItems() {
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
