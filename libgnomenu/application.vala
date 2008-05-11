using GLib;
namespace Gnomenu {

[DBus (name = "org.gnomenu.Application", signals="propChanged, newDocument, quit")]
public class Application: MenuOwner {
	public HashTable<string, Document> docs;

	public signal void quit();
	public signal void new_document(string path);
	public Application (string name) {
		this.name = name;
	}
	~Application (){
		quit();
	}
	construct {
		docs = new HashTable<string, Document>.full(str_hash, str_equal, g_free, g_object_unref);
		_path = "/org/gnomenu/Application";
	}
	public void insert(string #key, Document #doc){
		doc.parent = this;
		doc.key = key;
		var path = doc.path;
		message("ref count = %d", doc.ref_count);
		docs.insert(#key, #doc);
		prop_changed("children");
		new_document(path);
	}
	protected override void @foreach(BusObject.Func func){
		List<weak BusObject> l = docs.get_values();
		foreach(BusObject doc in l){
			func(doc);
		}
		base.@foreach(func);
	}
	protected override void reexpose(){
		if(!_exposed) return;
		message("app.reexpose");
		dynamic DBus.Object bus = conn.get_object ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
		uint request_name_result = bus.RequestName (get_app_bus_name(name), (uint) 0);
		if(request_name_result != DBus.RequestNameReply.PRIMARY_OWNER) {
			throw new GnomenuError.GNOMENU_APPLICATION_EXISTS("Old application already exsits");
		}
		base.reexpose();
	}
	public void remove(string key){
		message("an object is removed, key = %s", key);
		var doc = docs.lookup(key);
		message("ref count = %d", doc.ref_count);
		docs.remove(key);
		message("ref count = %d", doc.ref_count);
		prop_changed("children");
	}
	public string getDocument(string key) {
		Document m = docs.lookup(key);
		if(m is Document) return m.path;
		else return "";
	}

	[NoArrayLength]
	public string[] getDocuments() {
		List<weak Document> l = docs.get_values();
		string [] paths = new string[l.length()];
		var i = 0;
		foreach(Document d in l){
			paths[i] = d.path;
			i++;
		}	
		return paths;
	}
	public override string getTitle() {
		return base.getTitle();
	}
	public override string getMenu() {
		return base.getMenu();
	}
}
}
