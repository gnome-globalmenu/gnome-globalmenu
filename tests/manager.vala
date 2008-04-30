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
		vbox.pack_start_defaults(apptitle);
		vbox.pack_start_defaults(appmenu);
		vbox.pack_start_defaults(doctitle);
		vbox.pack_start_defaults(docmenu);
		appmenu.show();
		docmenu.show();
		apptitle.show();
		doctitle.show();

		conn = DBus.Bus.get (DBus.BusType. SESSION);
		agent = new BusAgent(conn, "FakeAppInterface"); /*app.vala */
	}
	public void bind_objects(GLib.Object local, GLib.Object remote){
		local.set_data_full("remote-item", remote.ref(), g_object_unref);
		remote.set_data("local-item", local);
	}
	
	public void run() {
		show();
		app = agent.get_object("", "Application");
		string[] paths = decode_paths(app.getDocuments());
		apptitle.set_label(app.getTitle());
		string path;
		app.propChanged += prop_changed;
		appmenu_r = agent.get_object(path = app.getMenu(), "Menu");
		dynamic DBus.Object[] items = agent.get_objects(appmenu_r.getMenuItems(), "MenuItem");
		foreach(dynamic DBus.Object i in items) {
			message("found menu item at %s", i.get_path());
			string title = i.getTitle();
			var item = new Gtk.MenuItem.with_label(title);
			item.show();
			bind_objects(item, i);
			appmenu.append(item);
			item.activate += (sender) => {
				message("clicked");
				dynamic DBus.Object i = (dynamic DBus.Object)sender.get_data("remote-item");
				i.activate();
			};
		}
		Gtk.main();
	}
	public void prop_changed(dynamic DBus.Object sender, string prop_name){
		
		if(prop_name == "title"){
			message("title has changed");
			apptitle.set_label(app.getTitle());
		}
	}
	static int main(string[] args){
		Gtk.init(ref args);
		
		var window = new MainWindow();
		window.run();
		return 0;
	}
}
