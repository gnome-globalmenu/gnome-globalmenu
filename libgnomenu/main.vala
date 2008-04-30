using GLib;
public errordomain GnomenuError {
	GNOMENU_SERVER_EXISTS
}
namespace Gnomenu {
public DBus.Connection conn;
public void init (string name) throws GnomenuError {
	conn = DBus.Bus.get (DBus.BusType. SESSION);
	dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
	uint request_name_result = bus.RequestName ("org.gnomenu.apps." + name, (uint) 0);
	if(request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
		throw new GnomenuError.GNOMENU_SERVER_EXISTS("Old server already exsits");
	}
}
public string [] decode_paths(string paths){
	string[] rt = paths.split("\n");
	rt.length = string.lengthv(rt); /*work around NoArray length*/
	return rt;
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
