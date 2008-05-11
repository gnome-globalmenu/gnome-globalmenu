using GLib;
using Gnomenu;

[DBus (name = "org.gnomenu.Service", signals = "newApplication, disposeApplication")]
class Service: Object {
	private HashTable<string, DBus.Object> applications;
	private DBus.Connection _conn;
	public DBus.Connection conn {
		get {return _conn;}
		set {
			_conn = value;
		}
	}
	public signal void new_application(string app_name);
	public signal void dispose_application(string app_name);
	construct {
		applications = new HashTable<string, DBus.Object>.full(str_hash, str_equal, g_free, g_object_unref);
	}
	public int register(string app_name) {
		try {
			BusAgent agent = new BusAgent(app_name);
			agent.conn=conn;
			dynamic DBus.Object a = agent.get_object("", "Application");
			message("new application: %s at %s : %s", app_name, a.get_bus_name(), a.get_path());
			applications.insert(app_name, #a); /* 
				implicity ownership transferring for non-referable entity 
				maybe not working*/
		} catch(Error e){
			message("failled to obtain the application: %s, %s", app_name, e.message);
			return 1;
		}
		return 0;
	}
	[NoArrayLength]
	public string [] getApplications(){
		List<weak string> l = applications.get_values();
		string [] apps = new string[l.length()];
		var i = 0;
		foreach(weak string s in l){
			apps[i] = s;
			i++;
		}
		return apps;
	}
	private void start(){
		dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
		uint request_name_result = bus.RequestName ("org.gnomenu.server", (uint) 0);
		if(request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
			throw new GnomenuError.GNOMENU_SERVER_EXISTS("Old server already exsits");
		}
		conn.register_object("/org/gnomenu/Server",this);
	}
	public static int main(string[] argv) {
		MainLoop loop = new MainLoop (null, false);
		var conn = DBus.Bus.get (DBus.BusType.SESSION);
		Service service = new Service();
		service.conn = conn;
		service.start();
		loop.run();
		return 0;
	}
}

