using GLib;
using Gtk; 
using GMarkup;
using Gnomenu;
using DBus;


public class Navigator :Gtk.Window{
	private ListView server_viewer;
	private ListView viewer;
	private MenuView viewer2;
	private RemoteDocument server;
	private dynamic DBus.Object client;
	public Navigator() {
		type = Gtk.WindowType.TOPLEVEL;
	}
	construct {
		server = RemoteDocument.connect("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server");
		server_viewer = new ListView(server);
		viewer = new ListView(null);
		viewer2 = new MenuView();
		Gtk.Paned vpan = new Gtk.VPaned();
		Gtk.Box vbox = new Gtk.VBox(false, 0);
		this.add(vpan);
		vpan.pack1(server_viewer, true, true);
		vpan.pack2(vbox, true, false);
		vbox.pack_start(viewer2, false, true, 0);
		vbox.pack_start_defaults(viewer);
		
		server_viewer.activated += (server_viewer, node)=> {
			string bus = (node as GMarkup.Tag).get("bus");
			print("attatch to bus %s", bus);
			RemoteDocument doc = RemoteDocument.connect(bus, "/org/gnome/GlobalMenu/Application");
			DBus.Connection conn;
			conn = Bus.get(DBus.BusType.SESSION);
			this.client = conn.get_object(bus, "/org/gnome/GlobalMenu/Application", "org.gnome.GlobalMenu.Client");
			viewer.document = doc;
		};
		viewer.activated += (viewer, node) => {
			if((node as GMarkup.Tag).tag == "menubar") {
				Section section = new Section(viewer.document, node);
				viewer2.document = section;
			}
		};
		viewer2.activated += (viewer2, node) => {
			message("activated!");
			this.client.Activate("NULL", node.name);
		};
	}
	public static int main(string[] args) {
		Gtk.init(ref args);
	//	MainLoop loop = new MainLoop(null, false);

		Navigator nav = new Navigator();
		nav.show_all();
		nav.delete_event += (widget) => {
			Gtk.main_quit();
			return false;
		};
		Gtk.main();
		return 0;	
	}

}
