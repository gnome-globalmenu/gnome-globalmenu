using GLib;

namespace Gnomenu {
[DBus (name = "org.gnomenu.Document", signals="propChanged") ]
public class Document: MenuOwner {
	public weak Application app { get; construct;}
	public string title {get; set;}

	public Document(weak Application app, string name){
		this.app = app;
		this.name = name;
		this.path = app.path + "/" + name;
		this.title = name;
	}
	construct {
		app.docs.insert(name, this);
	}
	public string getTitle() {
		return title;
	}
	public string getMenu() {
		if(menu is Menu) return menu.path;
		else return "";
	}
}
}
