using GLib;
using Gnomenu;
class App: Object {
		Application app; 
		Document doc1; 
		Menu app_menu; 
		MenuItem app_menu_quit; 
		Menu doc1_menu; 
		MenuItem doc1_menu_close; 
	void run(){
		MainLoop loop = new MainLoop (null, false);
		try {
			Gnomenu.init("FakeAppInterface");
		} catch (Error e){
			message("error: %s", e.message);
			return ;
		}
		app = new Application("FakeApp");
		doc1 = new Document(app, "1");
		app_menu = new Menu(app, "Menu");
		app_menu_quit = new MenuItem(app_menu, "Quit", -1);
		doc1_menu = new Menu(doc1, "Menu");
		doc1_menu_close = new MenuItem(doc1_menu, "Close", -1);
		app.expose();
		app_menu_quit.activated += on_app_menu_quit;
		loop.run ();
	}
	void on_app_menu_quit(Object sender){
		message("app_menu_quit clicked");
		app.title = "NewApplication";
		app.notify("title");
		message("new title %s ", app.title);
	}
	static int main(string[] argv) {

		App a = new App();
		a.run();
	return 0;
	}
}

