using GLib;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
		private class AttrStrBuilder {
			public weak StringBuilder builder;
			private void foreach_func (Quark key, string value) {
				this.builder.append_printf(" %s=\"%s\"",
					key.to_string(), value);
			}
			public void build (Datalist<weak string> dl) {
				dl.foreach(foreach_func);
			}
		}
	public enum NodeType {
		TEXT,
		SPECIAL,
		ELEMENT,
		DOCUMENT,
		FRAGMENT,
		META,
		INVALID
	}
	public NodeType string_to_node_type(string str) {
		switch(str) {
			case "#DOCUMENT":
				return NodeType.DOCUMENT;
			case "#FRAGMENT":
				return NodeType.FRAGMENT;
			case "#TEXT":
				return NodeType.TEXT;
			case "#SPECIAL":
				return NodeType.SPECIAL;
			case "#META":
				return NodeType.META;
			default:
				return NodeType.ELEMENT;
		}
	}
	public weak string? node_type_to_string(NodeType type) {
		switch(type) {
			case NodeType.DOCUMENT:
				return "#DOCUMENT";
			case NodeType.FRAGMENT:
				return "#FRAGMENT";
			case NodeType.TEXT:
				return "#TEXT";
			case NodeType.SPECIAL:
				return "#SPECIAL";
			case NodeType.META:
				return "#META";
			default:
				return null;
		}	
	
	}
	public class Node: GLib.Object {
		public NodeType nodeType {get; construct;}
		public int id {get; construct set;}
		public weak DocumentModel document {get; construct;}
		public weak string value {get{return _value;} set{
			weak string old_value = _value;
			_value = value;
			if(old_value != value && anchored)	{
				if(frozen) dirty = true;
				else
					document.updated(this, null);
			}
		}}
		public weak string? name {get{return _name.to_string();} set{_name = Quark.from_string(value);}}
		public weak Node? firstChild {get{return (_children!=null)?_children.first().data:null;}}
		public weak Datalist attributes {get {return _attributes;}}
		public weak Node parent {
				get {return _parent;} 
				set {_parent = value;
					if(_parent == null) {
						freeze();
					} else {
						thaw();
					}
				}
		}
		public weak List<weak Node> children {
			get {return _children;}
		}
		public bool frozen {
			get { return _frozen >0;}
		}
		private bool dirty;
		private int _frozen;
		private weak Node _parent;
		private string _value;
		private List<weak Node> _children;
		private Quark _name;
		private Datalist<weak string> _attributes;
		public bool anchored;
		public Node (DocumentModel document, int id, NodeType type){
			this.document = document;
			this.id = id;
			this.nodeType = type;	
		}
		construct {
			_frozen = 0;
			dirty = false;
			anchored = false;
		}
		public string to_string (bool recursive = true) {
			StringBuilder sb = new StringBuilder("");
			build_string(sb, false, recursive);
			return sb.str;
		}
		public string to_meta_string (bool recursive = true) {
			StringBuilder sb = new StringBuilder("");
			build_string(sb, true, recursive);
			return sb.str;
		}
		private void build_string(StringBuilder sb, bool meta, bool recursive) {
			switch(nodeType) {
				case NodeType.DOCUMENT:
				case NodeType.FRAGMENT:
					foreach(weak Node child in children){
						child.build_string(sb, meta, true);
					}
				return;
			}
			if(meta) {
				sb.append_printf("<gmarkup:meta name=\"%s\" id=\"%d\">", name, id);
			}
			switch(nodeType) {
				case NodeType.TEXT:
						sb.append(value);
				break;
				case NodeType.SPECIAL:
						sb.append(value);
				break;
				case NodeType.ELEMENT:
				case NodeType.META:
					sb.append_printf("<%s", name);
					AttrStrBuilder b = new AttrStrBuilder();
					b.builder = sb;
					b.build(_attributes);
					if(this.children == null || !recursive) sb.append("/>");
					else {
						sb.append_c('>');
						foreach(weak Node child in children){
							child.build_string(sb, meta, true);
						}
						sb.append_printf("</%s>", name);
					}
				break;
			}
			if(meta)
				sb.append_printf("</gmarkup:meta>");
		}
		public weak Node append(Node node) {
			return insert(node, null);
		}
		public weak Node replace(Node node, Node old_node) {
			insert(node, old_node);
			remove(old_node);
			return old_node;
		}
		public weak Node? insert(Node node, Node? ref_node) {
			if(node.parent != null) {
				message("%s", node.to_string(true));
			}
			message("%s", anchored.to_string());
			assert(node.parent == null);
			if(node.nodeType == NodeType.FRAGMENT) {
				foreach(weak Node child in node.children) {
					insert(child, ref_node);
				}
				return node;
			}
			if(this.nodeType != NodeType.FRAGMENT) {
				node.parent = this;
			}
			if(ref_node != null) {
				weak List<weak Node> ref_pos = children.find(ref_node);
				this._children.insert_before(ref_pos, node);
			} else {
				this._children.append(node);
			}
			transverse((node) => { node.anchored = this.anchored;});
			if(anchored) {
				document.inserted(this, node, ref_node);
			}
			if(ref_node != null) {
				debug("inserted %s to %s %s ", node.name, this.name, ref_node.name);
			} else
				debug("appended %s to %s ", node.name, this.name);
			return node;
		}
		public weak Node remove(Node node) {
			if(anchored) {
				node.transverse((node) => { node.anchored = false;});
				document.removed(this, node);
			}
			_children.remove(node);
			node.parent = null;
			debug("removed %s from %s", node.name, this.name);
			return node;
		}
		public void clear() {
			_attributes.clear();
		}
		public int index(Node node) {
			return _children.index(node);
		}
		public void freeze() {
			_frozen++;
		}
		public void thaw() {
			_frozen--;
			if(!frozen && dirty && anchored) {
				document.updated(this, null);	
				dirty = false;
			}
		}	
		public virtual void set(string key, string? value) {
			weak string old_value = get(key);
			if(value == null) {
				_attributes.remove_data(key);
			} else 
				_attributes.set_data_full(key, (string) Memory.dup(value, (uint) value.size() + 1), g_free);
			if(value != old_value) {
				if(!frozen && anchored)
					document.updated(this, key);
				else
					dirty = true;
			}
		}
		public virtual weak string? get(string key) {
			return _attributes.get_data(key);	
		}
		/**
		 * Transverse the document tree from the given node.
		 *
		 * 	 @param node 		the begin of the trip.
		 * 	 @param func 				the callback.
		 */
		public void transverse(TransverseFunc func) {
			Queue<weak Node> queue = new Queue<weak Node>();
			queue.push_head(this);
			while(!queue.is_empty()) {
				weak Node n = queue.pop_head();
				foreach(weak Node child in n.children) {
					queue.push_tail(child);
				}
				func(n);
			}
		}
		public delegate bool TransverseFunc(Node node);
		private void obtain_attributes_foreach_func (Quark key, string value) {
			set(key.to_string(), value);
		}
		public void obtain_attributes(Node node) {
			node.attributes.foreach(obtain_attributes_foreach_func);				
		}
		public Node clone() {
			Node rt = new Node(document, -1, nodeType);
			rt.name = name;
			rt.value = value;
			rt.obtain_attributes(this);
			return rt;
		}
		~Node() {
		}
	}
}
