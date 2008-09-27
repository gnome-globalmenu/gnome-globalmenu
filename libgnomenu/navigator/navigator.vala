using GLib;
using Gtk;
using XML;
using Gnomenu;



public class Navigator {

	public static int main(string[] args) {
		Gtk.init(ref args);
		MainLoop loop = new MainLoop(null, false);
		Gnomenu.RemoteDocument server = new Gnomenu.RemoteDocument("org.gnome.GlobalMenu.Server", "/org/gnome/GlobalMenu/Server");
		ListView viewer = new ListView(server);
		Gtk.Window window = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
		Gtk.Box box = new Gtk.HBox(false, 0);
		window.add(box);
		box.pack_start_defaults(viewer);
		window.show_all();
		loop.run();
		return 0;	
	}

}
