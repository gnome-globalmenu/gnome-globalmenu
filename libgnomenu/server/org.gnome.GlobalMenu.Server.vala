using GLib;
using Gnomenu;
using DBus;

public class Application {
	public static int main(string[] args) {
			Gtk.init(ref args);
			MainLoop loop = new MainLoop(null, false);
			Server c = new Server();
			loop.run();
			return 0;
	}

}

