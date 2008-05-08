using GLib;
namespace Gnomenu {
	public class BusAgent:Object {
		public BusAgent() {
		}   
		public dynamic DBus.Object get_object(string path, string ifname){
			string full_path;
			if(path != null && path.size() >0 ) {
				if(path[0] != '/') {
					full_path = "/org/gnomenu/Application/" + path;
				} else {
					full_path = path;
				}
			} else full_path = "/org/gnomenu/Application";
			return conn.get_object (get_app_bus_name(), full_path, "org.gnomenu." + ifname);
		}   
		public dynamic DBus.Object [] get_objects(string [] paths, string ifname){
//			string [] paths = decode_paths(s_paths);
			dynamic DBus.Object [] rt = new DBus.Object [paths.length];
			int i = 0;
			foreach(string p in paths){
				rt[i] = get_object(p, ifname);
				i++;
			}
			return rt;
		}
	}

}
