using GLib;
namespace Gnomenu {

[DBus (name = "org.gnomenu.Application", signals="propChanged")]
public class Application: MenuOwner {
	public HashTable<string, Document> docs;
	public Application (string name) {
		this.name = name;
	}
	construct {
		docs = new HashTable<string, Document>.full(str_hash, str_equal, g_free, g_object_unref);
		_path = "/org/gnomenu/Application";
	}
	public void insert(string #key, Document #doc){
		doc.parent = this;
		docs.insert(#key, #doc);
	}
	public void remove(string key){
		docs.remove(key);
	}
	public string getDocument(string key) {
		Document m = docs.lookup(key);
		if(m is Document) return m.path;
		else return "";
	}
/*
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
*/
	[NoArrayLength]
	public string[] getDocuments() {
		List<Document> l = docs.get_values();
		string [] paths = new string[l.length()];
		var i = 0;
		foreach(Document d in l){
			paths[i] = d.path;
			i++;
		}	
		return paths;
	}
	public override void expose() {
		base.expose();
		docs.for_each((k,v,d) => {
			((Document) v).expose();
		}, null);
	}
	public override void reset_path() {
		base.reset_path();
		docs.for_each((k,v,d) => {
			((Document) v).reset_path();
		}, null);
	}
	public override string getTitle() {
		return base.getTitle();
	}
	public override string getMenu() {
		return base.getMenu();
	}
}
}
