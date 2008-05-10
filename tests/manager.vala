using GLib;
using Gtk;
using Gnomenu;
using GnomenuGtk;


public class MainWindow : Window {
	private MenuBar appmenu;
	private MenuBar docmenu;
	private Label apptitle;
	private Notebook notebook;
	dynamic DBus.Object []  docs;
	dynamic DBus.Object app;
	public BusAgentGtk agent;
	public MainWindow () {
		title = "manager";
	}
	construct {
		var vbox = new VBox(false, 0);
		add (vbox);
		vbox.show();
		appmenu = new MenuBar();
		apptitle = new Label("");
		notebook = new Notebook();
		vbox.pack_start_defaults(apptitle);
		vbox.pack_start_defaults(appmenu);
		vbox.pack_start_defaults(notebook);
		appmenu.show();
		apptitle.show();
		notebook.show();
		agent = new BusAgentGtk("FakeApp"); /*app.vala */
		agent.conn = DBus.Bus.get(DBus.BusType.SESSION);
	}
	public void bind_objects(GLib.Object local, GLib.Object remote){
		local.set_data_full("remote-item", remote.ref(), g_object_unref);
		remote.set_data("local-item", local);
	}
	
	public void run() {
		show();
		app = agent.get_object("", "Application");
		string [] paths = app.getDocuments();
		docs = agent.get_objects(paths, "Document");
		foreach (dynamic DBus.Object d in docs){
			new_document(d.get_path());
		}
		app.newDocument += on_new_document;
		app.quit += on_app_quit;
		apptitle.set_label(app.getTitle());
		app.propChanged += on_prop_changed;
		agent.setup_menu_shell(appmenu, app.getMenu());
		Gtk.main();
	}
	public void new_document(string path){
		dynamic DBus.Object d = agent.get_object(path, "Document");
		MenuBar menubar = new MenuBar();
		Label label = new Label("");
		bind_objects(label, d);
		d.close += on_doc_close;
		menubar.show();
		string menu_path = d.getMenu();
		string doc_title = d.getTitle();
		d.set_data("menubar", menubar);
		agent.setup_menu_shell(menubar, menu_path);
		label.set_markup_with_mnemonic(doc_title);
		notebook.append_page(menubar, label);
	}
	public void on_new_document(dynamic DBus.Object sender, string path){
		new_document(path);
	}
	public void on_prop_changed(dynamic DBus.Object sender, string prop_name){
	/*	
		if(prop_name == "title"){
			message("title has changed");
			apptitle.set_label(app.getTitle());
		}*/
	}
	public void on_doc_close(dynamic DBus.Object sender) {
		weak Widget menubar = (Widget) sender.get_data("menubar");
		message("close a doc");
		notebook.remove_page(notebook.page_num(menubar));
	}
	public void on_app_quit(dynamic DBus.Object sender) {
		message("on_app_quit();");
	}
	static int main(string[] args){
		Gtk.init(ref args);
		
		var window = new MainWindow();
		window.run();
		return 0;
	}
}
