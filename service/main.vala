using GLib;
using Gnomenu;
[DBus (name = "org.gnomenu.Service", signals = "newApplication, disposeApplication")]
class Service: Object {
	private HashTable<string, DBus.Object> applications;
	public signal void new_application(string app_name);
	public signal void dispose_application(string app_name);
	construct {
		applications = new HashTable<string, DBus.Object>.full(str_hash, str_equal, g_free, g_object_unref);
	}
	public int register(string #app_name) {
		dynamic DBus.Object a = 
		applications.insert(#app_name, );
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
}
static public int main(string[] argv) {
	MainLoop loop = new MainLoop (null, false);
	Gnomenu.init("GnomenuService", Gnomenu.StartMode.SERVER);
	loop.run();
	return 0;
}
