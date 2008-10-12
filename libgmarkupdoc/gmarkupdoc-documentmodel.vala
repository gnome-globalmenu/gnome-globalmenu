using GLib;
using GLibCompat;
namespace GMarkupDoc {
	public interface DocumentModel: GLib.Object {
		private static StringChunk strings = null;
		public abstract weak Node root {get;}
		public abstract weak HashTable<weak string, weak Node> dict {get;}
		public abstract weak string name_attr {get;}
		public virtual Text CreateText(string text) {
			return new Text(this, text);
		}
		public virtual Special CreateSpecial(string text) {
			return new Special(this, text);
		}
		public virtual Tag CreateTag(string tag) {
			return new Tag(this, tag);
		}
		public virtual Tag CreateTagWithAttributes(string tag, 
				string[] attr_names, string[] attr_values) {
			Tag t = CreateTag(tag);
			t.freeze();
			t.set_attributes(attr_names, attr_values);
			t.unfreeze();
			return t;
		}
		public virtual weak string S(string s) {
			if(strings == null) strings = new StringChunk(1024);
			return strings.insert_const(s);
		}
		public signal void updated(Node node, string prop);
		public signal void inserted(Node parent, Node node, int pos);
		public signal void removed(Node parent, Node node);
		public signal void activated(Node node, Quark detail);
		public void activate(Node node, Quark detail) {
			this.activated(node, detail);
		}
		public virtual void transverse(Node node, TransverseFunc func) {
			Queue<weak Node> queue = new Queue<weak Node>();
			queue.push_head(node);
			while(!queue.is_empty()) {
				weak Node n = queue.pop_head();
				func(n);
				foreach(weak Node child in n.children) {
					queue.push_tail(child);
				}
			}
		}
		public delegate bool TransverseFunc(Node node);
	}

}
