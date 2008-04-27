using GLib;

int main(string[] argv) {
		MainLoop loop = new MainLoop (null, false);

		var conn = DBus.Bus.get (DBus.BusType. SESSION);

        dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");

        // try to register service in session bus
        uint request_name_result = bus.RequestName ("org.gnomenu.Service", (uint) 0);

        if (request_name_result == DBus.RequestNameReply.PRIMARY_OWNER) {
                // start server

                var testMenuItem = new MenuItem ();
                var testMenu = new Menu ();
                conn.register_object ("/org/gnomenu/testMenuItem", testMenuItem);
                conn.register_object ("/org/gnomenu/testMenu", testMenu);

                loop.run ();
        } else {
                // client
                dynamic DBus.Object testMenuItem = conn.get_object ("org.gnomenu.Service", "/org/gnomenu/testMenuItem", "org.gnomenu.MenuItem");
                dynamic DBus.Object testMenu = conn.get_object ("org.gnomenu.Service", "/org/gnomenu/testMenu", "org.gnomenu.Menu");

				string pong = testMenuItem.ping();
                message (pong);
        }	
	stdout.printf("Hello world");
	return 0;
}
