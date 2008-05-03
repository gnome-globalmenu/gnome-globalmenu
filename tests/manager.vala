using GLib;
using Gtk;
using Gnomenu;
using GnomenuGtk;


public class MainWindow : Window {
	private MenuBar appmenu;
	private MenuBar docmenu;
	private Label apptitle;
	private Label doctitle;

	dynamic DBus.Object app;
	dynamic DBus.Object doc;
	public BusAgentGtk agent;
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
		agent = new BusAgentGtk(); /*app.vala */
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
		path = app.getMenu();
		dynamic DBus.Object menu = agent.get_object(path, "Menu");
		string[] paths2 = menu.getMenuItems2();
		message("%d", paths2.length);
		agent.setup_menu_shell(appmenu, path);
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
		Gnomenu.init("FakeAppInterface", Gnomenu.StartMode.MANAGER);
		
		var window = new MainWindow();
		window.run();
		return 0;
	}
}
