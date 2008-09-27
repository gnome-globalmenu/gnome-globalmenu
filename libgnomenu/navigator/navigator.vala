using GLib;
using Gtk;
using XML;
using Gnomenu;



public class Navigator :Gtk.Window{
	private ListView server_viewer;
	private ListView viewer;
	private RemoteDocument server;
	public Navigator() {
		type = Gtk.WindowType.TOPLEVEL;
	}
	construct {
		server = new Gnomenu.RemoteDocument("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server");
		server_viewer = new ListView(server);
		viewer = new ListView(null);
		Gtk.Box box = new Gtk.HBox(false, 0);
		this.add(box);
		box.pack_start_defaults(server_viewer);
		box.pack_start_defaults(viewer);

		server.activated += (docu, node)=> {
			string bus = node.get("bus");
			print("attatch to bus %s", bus);
			RemoteDocument doc = new Gnomenu.RemoteDocument(bus, "/org/gnome/GlobalMenu/Application");
			viewer.document = doc;
		};
	}
	public static int main(string[] args) {
		Gtk.init(ref args);
		MainLoop loop = new MainLoop(null, false);

		Navigator nav = new Navigator();
		nav.show_all();
		loop.run();
		return 0;	
	}

}
