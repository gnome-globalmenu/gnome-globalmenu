using GLib;
using Gtk;
using Gnomenu;


public class MainWindow : Window {
	private MenuBar appmenu;
	private MenuBar docmenu;
	private Label apptitle;
	private Label doctitle;

	public DBus.Connection conn;
	dynamic DBus.Object app;
	dynamic DBus.Object doc;
	dynamic DBus.Object appmenu_r;
	dynamic DBus.Object docmenu_r;
	public BusAgent agent;
	public MainWindow () {
		title = "manager";
	}
	construct {
		var vbox = new VBox(false, 0);
		add (vbox);
		vbox.show();
		appmenu = new MenuBar();
		docmenu = new MenuBar();
		apptitle = new Label("");
		doctitle = new Label("");
		vbox.pack_start_defaults(appmenu);
		vbox.pack_start_defaults(apptitle);
		vbox.pack_start_defaults(docmenu);
		vbox.pack_start_defaults(doctitle);
		appmenu.show();
		docmenu.show();
		apptitle.show();
		doctitle.show();

		conn = DBus.Bus.get (DBus.BusType. SESSION);
		agent = new BusAgent(conn, "FakeAppInterface"); /*app.vala */
		app = agent.get_object("", "Application");
		string[] paths = decode_paths(app.getDocuments());
		apptitle.set_label(app.getTitle());
		string path;
		appmenu_r = agent.get_object(path = app.getMenu(), "Menu");
		paths = decode_paths(appmenu_r.getMenuItems());
		foreach(string p in paths) {
			message("found menu item at %s", p);
			dynamic DBus.Object i = agent.get_object(p, "MenuItem");
			string title = i.getTitle();
			var item = new Gtk.MenuItem.with_label(title);
			item.show();
			appmenu.append(item);
		}
	}
	public void run() {
		show();
		Gtk.main();
	}
	static int main(string[] args){
		Gtk.init(ref args);
		
		var window = new MainWindow();
		window.run();
		return 0;
	}
}
