using GLib;
using Gtk;
namespace GMarkupDoc {
	public class Document: GLib.Object, DocumentModel {
		private Root _root;
		private HashTable <weak string, weak Node> _dict;
		public weak Node root {get {return _root;}}
		public weak HashTable<weak string, weak Node> dict { get{return _dict;} }
		public weak string name_attr {get {return S("name");}}
		construct {
			_dict = new HashTable<weak string, weak Node>(str_hash, str_equal);
			_root = new Root(this);
			this.activated += (document, node, detail) => {
				message("a node %s is activated", node.name);
			};
		}
	}
}
