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
	if(bluh.len>0)
		bluh.erase(bluh.len-1, 1);
	return bluh.str;
}



}
