using GLib;
using Gee;

namespace DOM {
	public class Node {
		public enum Type {
			ELEMENT,
			ATTRIBUTE,
			TEXT,
			CDATA_SECTION,
			ENTITY_REFERENCE,
			ENTITY_NODE,
			PROCESSING_INSTRUCTION,
			COMMENT,
			DOCUMENT,
			DOCUMENT_TYPE,
			DOCUMENT_FRAGMENT,
			NOTATION,
		}
		public Node(Document? owner, Type type, string name) {
			ownerDocument = owner;
			nodeType = type;
			nodeName = name;
			_childNodes = new Gee.ArrayList<Node>();
			_attributes = new Gee.HashMap<weak string, Attr>(str_hash, str_equal, direct_equal);
			_weak_pointers = new Gee.HashSet<void**>();
			if(owner != null) {
				owner.add_weak_pointer((void**)(&_ownerDocument));
			}
		}		
		~Node () {
			if(ownerDocument != null) {
				ownerDocument.remove_weak_pointer((void**)(&_ownerDocument));
			}
			if(parentNode != null) {
				parentNode.remove_weak_pointer((void**)(&_parentNode));
			}
			foreach(void** pointer in _weak_pointers) {
				*pointer = null;
			}
		}
		/* Node interface */
		public weak Document ownerDocument { get; construct; }

		public Type nodeType {get; construct set;}
		public weak string? nodeName { 
			get { 
				return _nodeNameQuark.to_string();
			} 
			construct set {
				_nodeNameQuark = Quark.from_string(value);
			}
		}
		public virtual string? nodeValue { get; construct set;}

		public virtual Gee.List<Node> childNodes {get { return _childNodes;}}
		public Gee.Map<weak string, Attr> attributes {get { return _attributes;}}

		public Node parentNode { get { return _parentNode; }}

		public Node? firstChild { 
			get { 
				if(childNodes.size == 0) return null;
				return childNodes.get(0); 
			}
		}
		public Node? lastChild { 
			get { 
				if(childNodes.size == 0) return null;
				return childNodes.get(childNodes.size - 1);
			}
		}
		public Node? previousSibling { 
			get { 
				if(_parentNode == null) return null;
				int index = _parentNode.childNodes.index_of(this);
				if(index <= 0) return null;
				return _parentNode.childNodes.get(index - 1);
			}
		}
		public Node? nextSibling {
			get {
				if(_parentNode == null) return null;
				int index = _parentNode.childNodes.index_of(this);
				if(index + 1 == _parentNode.childNodes.size) return null;
				return _parentNode.childNodes.get(index + 1);
			}	
		}

		private weak Node _parentNode;

		public Node insertBefore(Node newChild, Node? refChild) throws Exception {
			check_document(newChild);
			check_hierarchy(newChild);
			if(newChild.nodeType == Node.Type.DOCUMENT_FRAGMENT) {
				foreach(Node node in newChild.childNodes) {
					node.set_parent_node(null);
					insertBefore(node, refChild);
				}
				newChild.childNodes.clear();
				return newChild;
			}
			int index;
			if(refChild == null) {
				index = childNodes.size;
			} else {
				check_document(refChild);
				index = childNodes.index_of(refChild);
				if(index == -1) throw new Exception.NOT_FOUND_ERR("refChild not found");
			}
			childNodes.insert(index, newChild);
			newChild.set_parent_node(this);
			return newChild;
		}

		public Node replaceChild(Node newChild, Node oldChild) throws Exception {
			check_document(newChild);
			check_document(oldChild);
			Node oldChild_ref = oldChild; /*protect the oldChild from being freed by _childNodes*/
			int index = childNodes.index_of(oldChild);
			if(index == -1) throw new Exception.NOT_FOUND_ERR("oldChild not found");
			childNodes.set(index, newChild);
			newChild.set_parent_node(this);
			oldChild_ref.set_parent_node(null);
			return oldChild_ref;
		}

		public Node appendChild(Node newChild) throws Exception {
			return insertBefore(newChild, null);
		}

		public Node removeChild(Node oldChild) throws Exception {
			Node oldChild_ref = oldChild;
			if(!childNodes.remove(oldChild)) throw new Exception.NOT_FOUND_ERR("oldChild not found");
			oldChild_ref.set_parent_node(null);
			return oldChild_ref;
		}

		public bool hasChildNodes() { return childNodes.size > 0; }
		public bool hasAttriutes() { return attributes.size > 0; }

		public virtual Node cloneNode(bool deep = false) {
			return this;
		}

		public void normalize() {
			/*TODO: write this*/
		}

		/* private */
		public long ref_count; /* disable this hack if Node is based on Object*/
		private Quark _nodeNameQuark;
		private Gee.List<Node> _childNodes;
		private Gee.Map<Attr> _attributes;
		private Gee.Set<void**> _weak_pointers;

		private void check_document(Node node) throws Exception {
			if(node.ownerDocument == this.ownerDocument) return;
			if(this.nodeType == Node.Type.DOCUMENT && node.ownerDocument == this) return;
			throw new Exception.WRONG_DOCUMENT_ERR("the node is from a differnt document");
		}
		private void check_hierarchy(Node node) throws Exception {
			switch(nodeType) {
				case Node.Type.DOCUMENT:
					if((this as Document).documentElement != null) 
						throw new Exception.HIERARCHY_REQUEST_ERR("the document already has an element");
					break;
				case Node.Type.TEXT:
				case Node.Type.COMMENT:
					throw new Exception.HIERARCHY_REQUEST_ERR("no child is allowed for TEXT and COMMENT");
					break;
			}
			if(node.nodeType == Node.Type.ATTRIBUTE) {
				throw new Exception.HIERARCHY_REQUEST_ERR("attributes cannot be a child of any node");
			}
			if(is_anchestor(node))
				throw new Exception.HIERARCHY_REQUEST_ERR("an anchester of current node cannot be also a child of it");
		}
		public void add_weak_pointer ( void** pointer ) {
			_weak_pointers.add(pointer);
		}
		public void remove_weak_pointer ( void** pointer ) {
			_weak_pointers.remove(pointer);
		}
		private void set_parent_node(Node? parent) {
			if(_parentNode != null) 
				_parentNode.remove_weak_pointer((void**)(&_parentNode));
			_parentNode = parent;
			if(_parentNode != null) 
				_parentNode.add_weak_pointer((void**)(&_parentNode));
		}
		private bool is_anchestor (Node node) {
			weak Node p;
			for(p = _parentNode; p != null; p = p._parentNode) {
				if(p == node) return true;
			}
			return false;
		}
	}

}
