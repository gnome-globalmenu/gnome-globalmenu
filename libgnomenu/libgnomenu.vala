using GLib;
public errordomain GnomenuError {
	GNOMENU_APPLICATION_EXISTS,
	GNOMENU_CONNECTION_FAILS,
	GNOMENU_SERVER_EXISTS
}
namespace Gnomenu {

public enum StartMode {
	APPLICATION,
	SERVER,
	MANAGER
}
public DBus.Connection conn;
public string app_name;
public void init (string name, StartMode mode) throws GnomenuError {
	conn = DBus.Bus.get (DBus.BusType.SESSION);
	app_name = name;
	if(conn != null ) {
		dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
		if(mode == StartMode.APPLICATION){
			uint request_name_result = bus.RequestName (get_app_bus_name(), (uint) 0);
			if(request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
				throw new GnomenuError.GNOMENU_APPLICATION_EXISTS("Old application already exsits");
			}
		}
		if(mode == StartMode.SERVER){
			uint request_name_result = bus.RequestName ("org.gnomenu.server", (uint) 0);
			if(request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
				throw new GnomenuError.GNOMENU_SERVER_EXISTS("Old server already exsits");
			}
		}
	} else {
		throw new GnomenuError.GNOMENU_CONNECTION_FAILS("can't establish a connection with the DBus");
	}
}
public string get_app_bus_name(){
	return "org.gnomenu.apps." + app_name;
}
public string encode_name(string name){
	string rt = name.escape("");
	rt = rt.delimit(":./<>_ |", '_');
	return rt;	
}
public string decode_name(string name){
	return name;	
}
public void bind_objects(Object local, DBus.Object remote){
	local.set_data_full("dbus-obj", remote.ref(), g_object_unref);
	remote.set_data("local-obj", local);
}
public weak GLib.Object get_local(Object remote){
	return (GLib.Object) remote.get_data("local-obj");
}
public weak DBus.Object get_remote(Object local){
	return (DBus.Object) local.get_data("dbus-obj");
}
/*
public string [] decode_paths(string paths){
	string[] rt = paths.split("\n");
	rt.length = string.lengthv(rt); //work around NoArray length
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
*/
}
