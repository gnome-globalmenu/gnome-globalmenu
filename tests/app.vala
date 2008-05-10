using GLib;
using Gnomenu;

struct MenuItemInfo {
	public string name;
	[NoArrayLength]
	public MenuItemInfo[]? submenu_info;
}
const MenuItemInfo [] app_menu_main_item_info = {
	{"_New", null},
	{"_Quit", null},
	{"ChangeName", null},
	{null, null}
};
const MenuItemInfo [] app_menu_menu_item_info = {
	{"_Add", null},
	{"_Remove", null},
	{"_Hide", null},
	{"_Show", null},
	{"_Modify", null},
	{"_Enable", null},
	{"_Disable", null},
	{"Add SubMenu", null},
	{"Remove SubMenu", null},
	{null, null}
};

const MenuItemInfo [] app_menu_item_info = {
	{"AppMenuMain",  app_menu_main_item_info },
	{"AppMenuMenu", app_menu_menu_item_info },
	{null, null}
};
const MenuItemInfo [] test_menu_item_info = {
	{"<em>Line1</em>", null},
	{"Line2", null},
	{null, null}
};
const MenuItemInfo [] doc_menu_file_item_info = {
	{"_Save", null},
	{"_Close", null},
	{null, null}
};
const MenuItemInfo [] doc_menu_item_info = {
	{"_File", doc_menu_file_item_info},
	{"_Edit", null},
	{"_Help", null},
	{null, null}
};

class App: Object {
		Application app; 
		Menu app_menu; 
		MenuItem test_item;
		Menu test_menu;
		MainLoop loop;
	static int number;
	void on_activated(MenuItem item) {
		switch(item.name) {
			case "ChangeName":
				app.title = "ChangedTitle";
			break;
			case "_Add":
				test_item.visible = true;
				app_menu.insert(test_item, -1);
			break;
			case "_Remove":
				app_menu.remove(test_item);
			break;
			case "Add SubMenu":
				test_item.menu = test_menu;
			break;
			case "Remove SubMenu":
				test_item.menu = null;
			break;
			case "_Hide":
				test_item.visible = false;
			break;
			case "_Show":
				test_item.visible = true;
			break;
			case "_Enable":
				test_item.enabled = true;
			break;
			case "_Disable":
				test_item.enabled = false;
			break;
			case "_Modify":
				test_item.title = "_Modified Title";
			break;
			case "_New":
				Document doc = new Document("NewDoc");
				doc.menu = new Menu("DocMenu");
				setup_menu(doc.menu, doc_menu_item_info);
				app.insert((number++).to_string(), #doc);
			break;
			case "_Quit":
				quit();
			break;
			case "_Close":
				weak BusObject o = item;
				while(o != null && !(o is Document)){
					o = o.parent;
				}
				if(o is Document){
					app.remove(((Document)o).key);
				}
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
		loop = new MainLoop (null, false);
		app = new Application("FakeApp");
		try {
		//	Gnomenu.init("FakeAppInterface", Gnomenu.StartMode.APPLICATION);
			app.conn =  DBus.Bus.get (DBus.BusType.SESSION);
		} catch (Error e){
			message("error: %s", e.message);
			return ;
		}
		app_menu = new Menu("AppMenu");
		app.menu = app_menu;
		test_item = new MenuItem("TestItem");
		test_menu = new Menu("TestMenu");


		setup_menu(app_menu, app_menu_item_info);
		setup_menu(test_menu, test_menu_item_info);

		{
			Document [] docs = new Document[2];
			for(int i=0; i<2; i++){
				string docname = "Doc%d".printf(i);
				docs[i]= new Document(docname);
				docs[i].menu = new Menu("DocMenu");
				setup_menu(docs[i].menu, doc_menu_item_info);
				app.insert(docname, docs[i]);
			}
		}

		app.expose();
		loop.run ();
	}
	void quit(){
		loop.quit();
	}
	static int main(string[] argv) {

		App a = new App();
		a.run();
	return 0;
	}
}

