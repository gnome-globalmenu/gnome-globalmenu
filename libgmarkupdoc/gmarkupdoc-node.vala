using GLib;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public abstract class Node: GLib.Object {
		private bool disposed;
		protected int frozen;
		protected weak Node _parent;
		public weak Node parent {
				get {return _parent;} 
				set {_parent = value; 
					if(parent != null)
						this.unfreeze();
					else
						this.freeze();
				}
		}
		protected List<weak Node> children;
		public weak DocumentModel document {get; construct;}
		public Node (DocumentModel document){ this.document = document;}
		private weak string _name;
		public weak string name {
			get {return _name;}
			set {
				if(name == value) return;
				this.ref();
				string oldname = name;
				if(name != null)
					document.dict.remove(name);
				assert(value != null);
				_name = document.S(value);
				document.dict.insert(name, this); /*weird! vala automatically ref it!*/
				this.unref();
				debug("name changed from %s to %s", oldname, name);
				document.renamed(this, oldname, name);
			}
		}
		construct {
			disposed = false;
			frozen = 1;
		}
		public virtual string to_string () {
			return summary(-1);
		}
		public void append(Node node) {
			insert(node, -1);
		}
		public virtual void insert(Node node, int pos) {
			assert(node.parent == document.orphan);
			document.orphan.remove(node);
			node.parent = this;
			this.children.insert(node, pos);
			document.inserted(this, node, pos);
		}
		public virtual void remove(Node node) {
			document.removed(this, node);
			children.remove(node);
			document.orphan.append(node);
		}
		public int index(Node node) {
			return children.index(node);
		}
		public abstract virtual string summary(int level = 0);
		protected override void dispose() {
			if(!disposed){
				disposed = true;
			}
		}
		public void freeze() {
			frozen++;
		}
		public void unfreeze() {
			frozen--;
		}	
		~Node() {
		}
	}
	public class Root : Node {
		public Root(DocumentModel document){
			this.document = document;
			this.name = "root";
		}
		public override string summary(int level = 1) {
			StringBuilder sb = new StringBuilder("");
			if(this.children == null)
				return "";
			else {
				foreach(weak Node child in children){
					sb.append_printf("%s", child.summary(level));
				}
			}
			return sb.str;
		}
	}
	public class Orphan : Root {
		public Orphan(DocumentModel document){
			this.document = document;
			this.name = "Orphan";
		}
		public override void insert(Node child, int pos) {
			this.children.insert(child, pos);
			child.parent = this;
			document.inserted(this, child, pos);
		}
		public override void remove(Node child) {
			this.children.remove(child);
			document.removed(this, child);
		}
	}
	public class Text : Node {
		public string text {
			get; construct set;
		}
		construct {
			document.orphan.append(this);
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
		construct {
			document.orphan.append(this);
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
			document.orphan.append(this);
		}
		public void set_attributes(string[] names, string[] values) {
			assert(names.length == values.length);
			for(int i=0; i< names.length && i < values.length; i++) {
				this.set(names[i], values[i]);
			}
		}
		public virtual void set(string prop, string? val) {
			if(prop == document.name_attr) {
				this.name = val;
			} else {
				if(val == null)
					props.remove(prop);
				else 
					props.insert(document.S(prop), val);
				if(this.frozen <=0)
					document.updated(this, prop);
			}
		}
		public virtual void unset(string prop) {
			set(prop, null);
		}
		public virtual void unset_all() {
			props.remove_all();
			if(this.frozen <=0)
				document.updated(this, null);
		}
		public virtual weak string? get(string prop) {
			return props.lookup(prop);
		}
		private string props_to_string() {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			StringBuilder sb = new StringBuilder("");
			sb.append_printf(" %s=\"%s\"", document.name_attr, this.name);
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
