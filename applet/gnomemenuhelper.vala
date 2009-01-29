using GLib;
using GMenu;

namespace GnomeMenuHelper {
	public static HashTable<string,string> get_flat_list() {
		GnomeVFS.init ();
		TreeDirectory node = GMenu.Tree.lookup ("applications.menu", TreeFlags.INCLUDE_EXCLUDED).get_root_directory();
		return __get_flat_list(node);
	}
	private static string replace(string source, string find, string replacement) {
		/* replaces the string.replace method which depends on GLib.RegEx >= 2.12 */
		string[] buf = source.split(find);
		return join(buf, replacement);
	}
	private static string join(string[] buf, string separator) {
		string ret = "";
		for (int co=0; co<buf.length; co++) {
			ret+=buf[co];
			if (co!=(buf.length-1)) ret+=separator;
		}
		return ret;
	}
	public static void message(string msg) {
		Gtk.MessageDialog m = new Gtk.MessageDialog(null,
								Gtk.DialogFlags.MODAL,
								Gtk.MessageType.INFO,
								Gtk.ButtonsType.OK,
								msg);
		m.run();
		m.destroy();
	}
	private static string adjust_spaces(string source) {
		string ret = "";
		bool quoted = false;
		for (int co=0; co<source.length; co++) {
			if (source[co]=='"') quoted = !quoted;
			if ((source[co]==' ') && quoted)
				ret += "&nbsp;"; else
				ret += source.substring(co, 1);
		}
		return ret;
	}
	private static HashTable<string,string> __get_flat_list(TreeDirectory root) {
		TreeDirectory node = root;
		HashTable<string,string> ret = new HashTable<string, string>.full(str_hash, str_equal, g_free, g_free);
		foreach (TreeItem item in node.get_contents()) {
			if (item.get_type() == TreeItemType.ENTRY) {
				string txt = ((TreeEntry)item).get_exec();
				txt = adjust_spaces(txt);
				
				string[] buf = txt.split(" ");
				int cc=0;
				while(buf[cc]=="env") cc+=2;
				while(buf[cc]=="wine") cc++;
				txt = buf[cc];
				
				long co = txt.length-1;
				while ((co>=0) && (txt.substring(co, 1)!="/")) {
					co--;
				}
				txt = txt.substring(co+1,(txt.length-co-1));
				txt = replace(txt, "&nbsp;", " ");
				txt = replace(txt, "\"", "");
				
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
