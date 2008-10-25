using GLib;
using GLibCompat;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public interface DocumentModel: GLib.Object {
		private static StringChunk strings = null;
		private static uint unique;
		public abstract weak Node root {get;}
		public abstract weak Node orphan {get;}
		public abstract weak HashTable<weak string, Node> dict {get;}
		public abstract weak string name_attr {get;}
		public virtual Text CreateText(string text) {
			Text rt = new Text(this, text);
			rt.name = S("TEXT" + unique.to_string());
			unique++;
			return rt;
		}
	
		public virtual Special CreateSpecial(string text) {
			Special rt =new Special(this, text);
			rt.name =  S("SPECIAL" + unique.to_string());
			unique++;
			return rt;
		}
		public virtual Tag CreateTag(string tag) {
			Tag rt = new Tag(this, tag);
			rt.name = S("TAG" + unique.to_string());
			unique++;
			return rt;
		}
		public virtual Tag CreateTagWithAttributes(string tag, 
				string[] attr_names, string[] attr_values) {
			Tag t = CreateTag(tag);
			t.set_attributes(attr_names, attr_values);
			return t;
		}
		public virtual void DestroyNode(Node? node) {
			assert(node.parent == orphan);
			orphan.remove(node);
			transverse(node, (node) => {
				debug("%s  %u", node.name, node.ref_count);
//				assert(node.ref_count == 1);
				dict.remove(node.name);
			});
		}
		public virtual weak string S(string? s) {
			if(strings == null) {
				strings = new StringChunk(1024);
				unique = 1;
			}
			if(s == null) return null;
			return strings.insert_const(s);
		}
		public abstract signal void updated(Node node, string? prop);
		public abstract signal void inserted(Node parent, Node node, int pos);
		public abstract signal void removed(Node parent, Node node);
		public abstract signal void renamed(Node node, string? oldname, string? newname);
		public abstract signal void destroyed();

		public virtual void transverse(Node node, TransverseFunc func) {
			Queue<weak Node> queue = new Queue<weak Node>();
			queue.push_head(node);
			while(!queue.is_empty()) {
				weak Node n = queue.pop_head();
				foreach(weak Node child in n.children) {
					queue.push_tail(child);
				}
				func(n);
			}
		}
		public delegate bool TransverseFunc(Node node);
	}

}
