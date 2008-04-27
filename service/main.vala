using GLib;

class DBusAgent : Object {
	private DBus.Connection conn {get; set;}
	public DBusAgent(DBus.Connection conn) {
		this.conn = conn;
	}
	public dynamic DBus.Object getObject(string path, string ifname){
		return conn.get_object ("org.gnomenu.Service", path, "org.gnomenu." + ifname);
	}
}
int main(string[] argv) {
		MainLoop loop = new MainLoop (null, false);

		DBus.Connection conn;

		conn = DBus.Bus.get (DBus.BusType. SESSION);
        dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");

        // try to register service in session bus
        uint request_name_result = bus.RequestName ("org.gnomenu.Service", (uint) 0);

        if (request_name_result == DBus.RequestNameReply.PRIMARY_OWNER) {
                // start server

				var factory = new Factory(conn);

                loop.run ();
        } else {
                // client
				DBusAgent agent = new DBusAgent(conn);
                dynamic DBus.Object factory = agent.getObject ("/org/gnomenu/Factory", "Factory");
				List<dynamic DBus.Object> menuitems = new List<dynamic DBus.Object>();
				List<dynamic DBus.Object> menus = new List<dynamic DBus.Object>();
				for(int i=0; i<10; i++){
					dynamic DBus.Object item = agent.getObject (factory.createMenuItem(), "MenuItem");
					dynamic DBus.Object menu = agent.getObject (factory.createMenu(), "Menu");
					item.setTitle("Item" + i.to_string());
					menu.setTitle("menu" + i.to_string());
					menuitems.append(item);
					menus.append(menu);
					if(i>0)
						item.setSubmenu(menus.nth_data(i-1).get_path());
					menu.insert(item.get_path(), 0);
				}

        }	
	stdout.printf("Hello world");
	return 0;
}
