using GLib;

namespace Gnomenu {
[DBus (name = "org.gnomenu.Document", signals="propChanged, close") ]
public class Document: MenuOwner {
	public signal void close();
	public weak string key;
	public Document(string name){
		this.name = name;
	}
	~Document(){
		message("dispose a document");
		close();
	}
	public override string getTitle() {
		return base.getTitle();
	}
	public override string getMenu() {
		return base.getMenu();
	}
}
}
