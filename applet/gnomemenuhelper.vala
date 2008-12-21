using GLib;
using GMenu;

namespace GnomeMenuHelper {
	public static HashTable<string,string> get_flat_list() {
		GnomeVFS.init ();
		TreeDirectory node = GMenu.Tree.lookup ("applications.menu", TreeFlags.INCLUDE_EXCLUDED).get_root_directory();
		return __get_flat_list(node);
	}	
	private static HashTable<string,string> __get_flat_list(TreeDirectory root) {
		TreeDirectory node = root;
		HashTable<string,string> ret = new HashTable<string, string>.full(str_hash, str_equal, g_free, g_free);
		foreach (TreeItem item in node.get_contents()) {
			if (item.get_type() == TreeItemType.ENTRY) {
				string txt = ((TreeEntry)item).get_exec();
				txt = txt.split(" ")[0];
				long co = txt.length-1;
				while ((co>=0) && (txt.substring(co, 1)!="/")) {
					co--;
				}
				txt = txt.substring(co+1,(txt.length-co-1));
				if (ret.lookup(txt)==null) ret.insert(txt, ((TreeEntry)item).get_name());
			}
		}
			
		foreach (TreeItem item in node.get_contents()) {
			if (item.get_type() == TreeItemType.DIRECTORY) {
				HashTable<string,string> children = __get_flat_list((TreeDirectory)item);
				foreach(string k in children.get_keys())
					ret.insert(k, (string)children.lookup(k));
			}
		}
		return ret;
	}
}
