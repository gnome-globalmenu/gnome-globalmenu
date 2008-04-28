using GLib;
public errordomain GnomenuError {
	GNOMENU_SERVER_EXISTS
}
namespace Gnomenu {
public DBus.Connection conn;
public string app_name;

public void init (string name) throws GnomenuError {
	conn = DBus.Bus.get (DBus.BusType. SESSION);
	app_name = name;
	dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
	uint request_name_result = bus.RequestName ("org.gnomenu.apps." + name, (uint) 0);
	if(request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
		throw new GnomenuError.GNOMENU_SERVER_EXISTS("Old server already exsits");
	}
}

public class BusObject:Object {
	public string path {get; construct;}
	public string name {get; construct;}

	public virtual void expose() {
		Object o = conn.lookup_object(path);
		if( o is Object) {
			if( o == this ) {
				message("%s is already exposed at: %s", name, path);
				return;
			} else {
				message("remove the old object at:%s", path);
				o.unref();
			}
		}
		conn.register_object(path, this);
	}
}
public class MenuOwner: BusObject {
	public Menu menu {get; set;}
	public override void expose() {
		base.expose();
		if(menu is Menu) menu.expose();
	}
}

[DBus (name = "org.gnomenu.Application")]
public class Application: MenuOwner {
	public HashTable<string, Document> docs;

	public Application () {
		path = "/org/gnomenu/Application";
		name = "";
	}
	construct {
		docs = new HashTable.full<string, Document>(str_hash, str_equal, null, g_object_unref);
	}
	public string getMenu() {
		if(menu is Menu) return menu.path;
		else return "";
	}
	public string getDocument(uint64 id) {
		Document m = docs.lookup(id);
		if(m is Document) return m.path;
		else return "";
	}
	public override void expose() {
		base.expose();
		docs.for_each((k,v,d) => {
			((Document) v).expose();
		}, null);
	}
}

[DBus (name = "org.gnomenu.Document")]
public class Document: MenuOwner {
	public weak Application app { get; construct;}

	public Document(weak Application app, string name){
		this.app = app;
		this.name = name;
		this.path = app.path + "/" + name;
	}
	construct {
		app.docs.insert(name, this);
	}
	public string getMenu() {
		if(menu is Menu) return menu.path;
		else return "";
	}
}

[DBus (name = "org.gnomenu.Menu", signals="notify")]
public class Menu: BusObject {
	public weak MenuOwner parent {get; construct;}
	public List<Menu> children;
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
	public string getTitle() {
		return title;
	}
	public override void expose() {
		base.expose();
		foreach (Menu m in children){
			m.expose();
		}
	}
}

[DBus (name = "org.gnomenu.MenuItem", signals ="notify")]
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
		this.parent.children.insert(this, pos);
	}
	public string getTitle() {
		return title;
	}
}
}
