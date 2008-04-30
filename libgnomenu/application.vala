using GLib;
namespace Gnomenu {

[DBus (name = "org.gnomenu.Application", signals="")]
public class Application: MenuOwner {
	public HashTable<string, Document> docs;
	public string title {get; set;}
	public Application (string name) {
		this.path = "/org/gnomenu/Application";
		this.name = name;
		this.title = name;
	}
	construct {
		docs = new HashTable.full<string, Document>(str_hash, str_equal, null, g_object_unref);
	}
	public string getTitle() {
		return title;
	}
	public string getMenu() {
		if(menu is Menu) return menu.path;
		else return "";
	}
	public string getDocument(string key) {
		Document m = docs.lookup(key);
		if(m is Document) return m.path;
		else return "";
	}
	public string getDocuments() {
		List<Document> l = docs.get_values();
		string [] paths = new string[l.length()];
		var i = 0;
		foreach(Document d in l){
			paths[i] = d.path;
			i++;
		}	
		return encode_paths(paths);
	}
	public override void expose() {
		base.expose();
		docs.for_each((k,v,d) => {
			((Document) v).expose();
		}, null);
	}
}
}
