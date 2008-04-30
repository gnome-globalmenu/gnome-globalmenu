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
public string [] decode_paths(string paths){
	return paths.split("\n");
}
public string encode_paths(string[] paths){
	StringBuilder bluh = new StringBuilder();
	foreach(string s in paths) {
		bluh.append(s);
		bluh.append("\n");
	}
	bluh.erase(bluh.len-1, 1);
	return bluh.str;
}

public class BusAgent:Object {
    public DBus.Connection conn {get; construct;}
    public string appname {get; construct;}
    public BusAgent(DBus.Connection conn, string appname) {
        this.conn = conn;
		this.appname = appname;
    }   
    public dynamic DBus.Object get_object(string path, string ifname){
		string full_path;
		if(path != null && path.size() >0 ) {
			if(full_path[0] != '/') {
				full_path = "/org/gnomenu/Application/" + path;
			} else {
				full_path = path;
			}
		} else full_path = "/org/gnomenu/Application";
        return conn.get_object ("org.gnomenu.apps." + appname, full_path, "org.gnomenu." + ifname);
    }   
}

public class BusObject:Object {
	public string path {get; construct;}
	public string name {get; construct;}
	public string test {get; set;}
	public signal void prop_changed(string prop);

	public void notify(string prop){
			prop_changed(prop);
	}
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


}
