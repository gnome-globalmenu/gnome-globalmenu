using GLib;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public class Document: GLib.Object, DocumentModel {
		private Root _root;
		private Root _orphan;
		private HashTable <weak string, weak Node> _dict;
		public weak Node root {get {return _root;}}
		public weak Node orphan {get {return _orphan;}}
		public weak HashTable<weak string, weak Node> dict { get{return _dict;} }
		public weak string name_attr {get {return S("name");}}
		construct {
			_dict = new HashTable<weak string, weak Node>.full(str_hash, str_equal, null, g_object_unref);
			_root = new Root(this);
			_orphan = new Orphan(this);
		}
	}
}
