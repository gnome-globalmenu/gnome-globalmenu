using GLib;
namespace XML {
	public abstract class Node: Object {
		private bool disposed;
		protected bool freezed;
		public weak Node parent;
		protected List<weak Node> children;
		public weak Document document {get; construct;}
		public Node (Document document){ this.document = document;}
		construct {
			disposed = false;
			freezed = false;
		}
		public virtual string to_string () {
			return summary(-1);
		}
		public void append(Node node) {
			insert(node, -1);
		}
		public void insert(Node node, int pos) {
			node.parent = this;
			children.insert(node.ref() as Node, pos);
			if(!freezed)
			document.added(this, node, pos);
		}
		public void remove(Node node) {
			Node parent = node.parent;
			children.remove(node);
			if(!freezed)
			document.removed(parent, node);
			node.parent = null;
			node.unref();
		}
		public int index(Node node) {
			return children.index(node);
		}
		public abstract virtual string summary(int level = 0);
		protected override void dispose() {
			if(!disposed){
				disposed = true;
				foreach(weak Node node in children){
					node.unref();
				}
			}
		}
		public void freeze() {
			freezed = true;
		}
		public void unfreeze() {
			freezed = false;	
		}	
		~Node() {
		}
	}
	public abstract class Document: Object {
		private StringChunk strings;
		private Root _root;
		public Node root {get{return _root;}}
		private class Root : Node {
			public Root(Document document){
				this.document = document;
			}
			public override string summary(int level = 0) {
				StringBuilder sb = new StringBuilder("");
				if(this.children == null)
					return "";
				else {
					foreach(weak Node child in children){
						sb.append_printf("%s", child.summary(level - 1));
					}
				}
				return sb.str;
			}
		}
		construct {
			strings = new StringChunk(1024);
			_root = new Root(this);
		}
		public abstract virtual Document.Text CreateTextNode(string text);
		public abstract virtual Document.Special CreateSpecialNode(string text);
		public abstract virtual Document.Tag CreateTagNode(string tag);
		public abstract virtual void FinishNode(Node node);
		public virtual weak string S(string s) {
			return strings.insert_const(s);
		}
		public signal void updated(Node node, string prop);
		public signal void added(Node parent, Node node, int pos);
		public signal void removed(Node parent, Node node);
		public class Text : Node {
			public string text;
			public Text(Document document) {
				this.document = document;
			}
			public override string summary (int level = 0) {
				return text;
			}
		}
		public class Special: Node {
			public string text;
			public Special(Document document) {
				this.document = document;
			}
			public override string summary (int level = 0) {
				return text;
			}
		}
		public class Tag : Node {
			public weak string tag;
			private HashTable<weak string, string> props;
			public Tag (Document document) {
				this.document = document;
			}
			construct {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			public void set(string prop, string val) {
				props.insert(document.S(prop), val);
				if(!freezed)
				document.updated(this, prop);
			}
			public void unset(string prop) {
				props.remove(prop);
				if(!freezed)
				document.updated(this, prop);
			}
			public weak string? get(string prop) {
				return props.lookup(prop);
			}
			private string props_to_string() {
				if(props == null) {
					props = new HashTable<weak string, string>(str_hash, str_equal);
				}
				StringBuilder sb = new StringBuilder("");
				foreach(weak string key in props.get_keys()) {
					string escaped = GLib.Markup.escape_text(props.lookup(key));
					sb.append_printf(" %s=\"%s\"", key, escaped);
				}
				return sb.str;
			}
			public override string summary(int level) {
				StringBuilder sb = new StringBuilder("");
				if(this.children == null || level == 0)
						sb.append_printf("<%s%s/>", tag, props_to_string());
				else {
					sb.append_printf("<%s%s>\n", tag, props_to_string());
					foreach(weak Node child in children){
						sb.append_printf("%s\n", child.summary((level>0)?(level - 1):level));
					}
					sb.append_printf("</%s>", tag);
				}
				return sb.str;
			}
		}
	}
}
