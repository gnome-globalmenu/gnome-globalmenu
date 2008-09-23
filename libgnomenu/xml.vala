using GLib;
namespace XML {
	public abstract class Node: Object {
		public weak Node parent;
		protected List<weak Node> children;
		public weak NodeFactory factory {get; construct;}
		public Node (NodeFactory factory){ this.factory = factory;}
		public virtual string to_string () {
			return summary(-1);
		}
		public void append(Node node) {
			node.parent = this;
			children.append(node.ref() as Node);
			factory.added(this, node);
		}
		public void remove(Node node) {
			Node parent = node.parent;
			children.remove(node);
			factory.removed(parent, node);
			node.parent = null;
			node.unref();
		}
		public abstract virtual string summary(int level = 0);
		~Node() {
		}
	}
	public abstract class NodeFactory: Object {
		public abstract virtual RootNode CreateRootNode();
		public abstract virtual TextNode CreateTextNode(string text);
		public abstract virtual SpecialNode CreateSpecialNode(string text);
		public abstract virtual TagNode CreateTagNode(string tag);
		public abstract virtual void FinishNode(Node node);
		public abstract virtual void DestroyNode(Node node);
		public abstract virtual weak string S(string s);
		public signal void updated(Node node, string prop);
		public signal void added(Node parent, Node node);
		public signal void removed(Node parent, Node node);
	}
	public class TextNode : Node {
		public string text;
		public TextNode(NodeFactory factory) {
			this.factory = factory;
		}
		public override string summary (int level = 0) {
			return text;
		}
	}
	public class SpecialNode: Node {
		public string text;
		public SpecialNode(NodeFactory factory) {
			this.factory = factory;
		}
		public override string summary (int level = 0) {
			return text;
		}
	}
	public class RootNode : Node {
		public RootNode(NodeFactory factory){
			this.factory = factory;
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
	public class TagNode : Node {
		public weak string tag;
		private HashTable<weak string, string> props;
		public TagNode (NodeFactory factory) {
			this.factory = factory;
		}
		public void set(string prop, string val) {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			props.insert(factory.S(prop), val);
			factory.updated(this, prop);
		}
		public void unset(string prop) {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			props.remove(prop);
		}
		public weak string? get(string prop) {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
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
					sb.append_printf("<%s%s/>\n", tag, props_to_string());
			else {
				sb.append_printf("<%s%s>\n", tag, props_to_string());
				foreach(weak Node child in children){
					sb.append_printf("%s", child.summary((level>0)?(level - 1):level));
				}
				sb.append_printf("</%s>\n", tag);
			}
			return sb.str;
		}
	}
}
