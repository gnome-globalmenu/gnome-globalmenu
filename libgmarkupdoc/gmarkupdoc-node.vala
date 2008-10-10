using GLib;
namespace GMarkupDoc {
	public abstract class Node: GLib.Object {
		private bool disposed;
		protected int freezed;
		protected weak Node _parent;
		public weak Node parent {
				get {return _parent;} 
				set{
					Node old_parent = _parent;
					_parent = value;
					if(freezed <=0)
					parent_set(old_parent);
				}
		}
		public signal void parent_set(Node? old_parent);
		protected List<weak Node> children;
		public weak DocumentModel document {get; construct;}
		public Node (DocumentModel document){ this.document = document;}
		private weak string _name;
		public weak string name {
			get {return _name;}
			set {
				if(name == value) return;
				if(name != null)
					document.dict.remove(name);
				_name = document.S(value);
				if(name != null)
					document.dict.insert(name, this);
			}
		}
		construct {
			disposed = false;
			freezed = 0;
		}
		public virtual string to_string () {
			return summary(-1);
		}
		public void append(Node node) {
			insert(node, -1);
		}
		public void insert(Node node, int pos) {
			children.insert(node.ref() as Node, pos);
			node.parent = this;
			if(freezed <= 0)
			document.inserted(this, node, pos);
		}
		public void remove_all() {
			foreach(weak Node node in children) {
				node.remove_all();
				if(freezed <= 0)
					document.removed(this, node);
				node.parent = null;
				node.unref();
			}
			children = null;
		}
		public void remove(Node node) {
			node.remove_all();
			children.remove(node);
			if(freezed <= 0)
			document.removed(this, node);
			node.parent = null;
			//message("ref count = %u", node.ref_count);
			node.unref();
		}
		public int index(Node node) {
			return children.index(node);
		}
		public abstract virtual string summary(int level = 0);
		protected override void dispose() {
			if(!disposed){
				disposed = true;
				remove_all();
			}
			if(name != null)
				document.dict.remove(name);
		}
		public void freeze() {
			freezed++;
		}
		public void unfreeze() {
			freezed--;
		}	
		~Node() {
		}
	}
	public class Root : Node {
		public Root(DocumentModel document){
			this.document = document;
		}
		construct {
			this.name = "root";
		}
		public override string summary(int level = 1) {
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
	public class Text : Node {
		public string text {
			get; construct set;
		}
		public Text(DocumentModel document, string text) {
			this.document = document;
			this.text = text;
		}
		public override string summary (int level = 0) {
			return text;
		}
	}
	public class Special: Node {
		public string text {
			get; construct set;
		}
		public Special(DocumentModel document, string text) {
			this.document = document;
			this.text = text;
		}
		public override string summary (int level = 0) {
			return text;
		}
	}
	public class Tag : Node {
		private weak string _tag;
		public weak string tag {
			get{ return _tag;}
			construct set {
				_tag = document.S(value);
			}
		}
		private HashTable<weak string, string> props;
		public Tag (DocumentModel document, string tag) {
			this.document = document;
			this.tag = tag;
		}
		construct {
			props = new HashTable<weak string, string>(str_hash, str_equal);
		}
		public void set_attributes(string[] names, string[] values) {
			assert(names.length == values.length);
			for(int i=0; i< names.length && i < values.length; i++) {
				this.set(names[i], values[i]);
			}
		}
		public virtual void set(string prop, string? val) {
			if(val == null)
				props.remove(prop);
			else 
				props.insert(document.S(prop), val);
			if(prop == document.name_attr)
				this.name = val;
			if(freezed <= 0)
				document.updated(this, prop);
		}
		public virtual void unset(string prop) {
			set(prop, null);
		}
		public virtual weak string? get(string prop) {
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
		public override string summary(int level = 0) {
			StringBuilder sb = new StringBuilder("");
			if(this.children == null || level == 0)
					sb.append_printf("<%s%s/>", tag, props_to_string());
			else {
				sb.append_printf("<%s%s>", tag, props_to_string());
				foreach(weak Node child in children){
					sb.append_printf("%s", child.summary((level>0)?(level - 1):level));
				}
				sb.append_printf("</%s>", tag);
			}
			return sb.str;
		}
	}
}
