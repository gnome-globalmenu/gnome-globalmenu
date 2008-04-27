using GLib;

[DBus (name = "org.gnomenu.Factory")]
[CCode (type_signature = "o")]
public class Factory: Object {
	private HashTable<string, MenuItem> menu_items_;
	private HashTable<string, Menu> menus_;
	public DBus.Connection conn {get; construct;}
	static int64 seq = 0;

	public Factory(DBus.Connection conn){
		this.conn = conn;
	}
	construct {
		conn.register_object ("/org/gnomenu/Factory", this);
	}
	public HashTable<string, MenuItem> menu_items {
		get {
			if(menu_items_ == null) 
				menu_items_ = new HashTable.full<string, MenuItem>(str_hash, str_equal, g_free, g_object_unref);
			return menu_items_;
		}
	}
	public HashTable<string, Menu> menus {
		get {
			if(menus_ == null) 
				menus_ = new HashTable.full<string, MenuItem>(str_hash, str_equal, g_free, g_object_unref);
			return menus_;
		}
	}
	private string gen_path(){
		seq ++;
		return "/org/gnomenu/Pool/ANONOBJ_" + seq.to_string();
	}
	public string createMenu(){
		string path = gen_path();
		menus.insert(path, new Menu(this));
		conn.register_object(path, menus.lookup(path));
		return path;
	}
	public string createMenuItem(){
		string path = gen_path();
		var item = new MenuItem(this);
		menu_items.insert(path, item);
		conn.register_object(path, menu_items.lookup(path));
		return path;
	}
	public int destroyMenu(string path){
		menus.remove(path);
		return 0;
	}
	public int destroyMenuItem(string path){
		menu_items.remove(path);
		return 0;
	}
}

[DBus (name = "org.gnomenu.Menu")]
[CCode (type_signature = "o")]
public class Menu: Object {
	private List<string> children;
	public weak Factory factory{get; construct;}
	private string title;

	public Menu(weak Factory factory) {
		this.factory = factory;
	}
	public int insert(string path, int pos){
		children.insert(path, pos);
		return 0;
	}
	public int setTitle(string title_) {
		title = title_;
		return 0;
	}
	public string getTitle() {
		return title;
	}
}

[DBus (name = "org.gnomenu.MenuItem")]
[CCode (type_signature = "o")]
public class MenuItem: Object {
	private string submenu {get; set;}
	public weak Factory factory{get; construct;}
	private string title;

	public signal void activated();
	public void activate(){
		activated();
	}
	public MenuItem(weak Factory factory) {
		this.factory = factory;
	}
	public int setSubmenu(string path){
		submenu = path;
		Menu m = (Menu) factory.conn.lookup_object(path);
		message(m.getTitle());
		return 0;
	}
	public string getSubmenu(){
		return submenu;
	}
	public int setTitle(string title_) {
		title = title_;
		return 0;
	}
	public string getTitle() {
		return title;
	}
}
