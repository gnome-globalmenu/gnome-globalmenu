using GLib;
using Gnomenu;
int main(string[] argv) {
		MainLoop loop = new MainLoop (null, false);
		try {
			Gnomenu.init("FakeApp");
		} catch (Error e){
			message("error: %s", e.message);
			return 1;
		}

		Application app = new Application();
		message(app.path);
		Document doc1 = new Document(app, "1");
		Menu app_menu = new Menu(app, "Menu");
		MenuItem app_menu_quit = new MenuItem(app_menu, "Quit", -1);
		Menu doc1_menu = new Menu(doc1, "Menu");
		MenuItem doc1_menu_close = new MenuItem(doc1_menu, "Close", -1);
		app.expose();
		app.expose();
		app_menu_quit.activated += (o) => {
			message("app_menu_quit clicked");
		};
		loop.run ();
	return 0;
}

