using GLib;
using Gnomenu;

struct MenuItemInfo {
	public string name;
	[NoArrayLength]
	public MenuItemInfo[]? submenu_info;
}
const MenuItemInfo [] app_menu_main_item_info = {
	{"Spawn", null},
	{"Quit", null},
	{"ChangeName", null},
	{null, null}
};
const MenuItemInfo [] app_menu_item_info = {
		{"AppMenuMain",  app_menu_main_item_info },
		{null, null}
	};
class App: Object {
		Application app; 
		Document doc1; 
		Menu app_menu; 
		Menu app_menu_main; 
		Menu doc1_menu; 
	void on_activated(MenuItem item) {
		switch(item.name) {
			case "ChangeName":
				app.title = "ChangedTitle";
			break;
		}
	}
	[NoArrayLength]
	void setup_menu(Menu menu, MenuItemInfo[] infos){
		message("setting up menu %s", menu.name);
		int i = 0;	
		for(i=0; infos[i].name!=null; i++) {
			MenuItemInfo info = infos[i];
			message("setting up item %s", info.name);
			MenuItem item = new MenuItem(info.name);
			if(info.submenu_info != null){ 
				Menu submenu = new Menu(info.name);
				item.menu = submenu;
				setup_menu(submenu, info.submenu_info);
				submenu.visible = true;
			} else {
				item.activated += on_activated;
			}
			menu.insert(item, -1);
			item.visible = true;
		}
		menu.visible = true;
	}
	void run(){
		MainLoop loop = new MainLoop (null, false);
		try {
			Gnomenu.init("FakeAppInterface", Gnomenu.StartMode.APPLICATION);
		} catch (Error e){
			message("error: %s", e.message);
			return ;
		}
		app = new Application("FakeApp");
		doc1 = new Document( "1");
		app_menu = new Menu("AppMenu");
		app.menu = app_menu;
		setup_menu(app_menu, app_menu_item_info);

		app.expose();
		loop.run ();
	}
	static int main(string[] argv) {

		App a = new App();
		a.run();
	return 0;
	}
}

