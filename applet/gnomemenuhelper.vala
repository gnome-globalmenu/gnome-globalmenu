using GLib;

namespace GnomeMenuHelper {
	public static GLib.HashTable<string,string> get_flat_list() {
		GnomeVFS.init ();
		GMenu.TreeDirectory node = GMenu.Tree.lookup ("applications.menu", GMenu.TreeFlags.INCLUDE_EXCLUDED).get_root_directory();
		return __get_flat_list(node);
	}	
	private static GLib.HashTable<string,string> __get_flat_list(GMenu.TreeDirectory root) {
		GMenu.TreeDirectory node = root;
		GLib.HashTable<string,string> ret = new HashTable<string, string>.full(str_hash, str_equal, g_free, g_free);
		foreach (GMenu.TreeItem item in node.get_contents()) {
			if (item.get_type() == GMenu.TreeItemType.ENTRY) {
				string txt = ((GMenu.TreeEntry)item).get_exec();
				txt = txt.split(" ")[0];
				long co = txt.length-1;
				while ((co>=0) && (txt.substring(co, 1)!="/")) {
					co--;
				}
				txt = txt.substring(co+1,(txt.length-co-1));
				if (ret.lookup(txt)==null) ret.insert(txt, ((GMenu.TreeEntry)item).get_name());
			}
		}
			
		foreach (GMenu.TreeItem item in node.get_contents()) {
			if (item.get_type() == GMenu.TreeItemType.DIRECTORY) {
				GLib.HashTable<string,string> children = __get_flat_list((GMenu.TreeDirectory)item);
				foreach(string k in children.get_keys())
					ret.insert(k, (string)children.lookup(k));
			}
		}
		return ret;
	}
}
