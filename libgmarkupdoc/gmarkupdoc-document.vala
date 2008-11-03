using GLib;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public class Document: GLib.Object, DocumentModel {
		private Node _root;
		private HashTable _pool;
		private int _unique;
		public weak HashTable node_pool { get{return _pool;} }
		public weak Node root { get{return _root;} }
		public int unique {get { return _unique++;}}
		construct {
			_unique = 0;
			_pool = createPool();
			_root = createRoot();
		}
	}
}
