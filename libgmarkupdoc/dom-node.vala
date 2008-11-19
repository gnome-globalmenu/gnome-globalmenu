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
		public string? nodeValue { get; construct set;}

		public Gee.List<Node> childNodes {get { return _childNodes;}}
		public Gee.Map<weak string, Attr> attributes {get { return _attributes;}}

		public Node parentNode { get { return _parentNode; }}

		public Node? firstChild { 
			get { 
				if(_childNodes.size == 0) return null;
				Node rt = _childNodes.get(0); 
				return rt;
			}
		}
		public Node? lastChild { 
			get { 
				if(_childNodes.size == 0) return null;
				Node rt = _childNodes.get(_childNodes.size - 1);
				return rt;
			}
		}
		public Node? previousSibling { 
			get { 
				if(_parentNode == null) return null;
				int index = _parentNode._childNodes.index_of(this);
				if(index <= 0) return null;
				Node rt = _parentNode._childNodes.get(index - 1);
				return rt;
			}
		}
		public Node? nextSibling {
			get {
				if(_parentNode == null) return null;
				int index = _parentNode._childNodes.index_of(this);
				if(index + 1 == _parentNode._childNodes.size) return null;
				Node rt = _parentNode._childNodes.get(index + 1);
				return rt;
			}	
		}
		
		private weak Node _parentNode;

		public Node insertBefore(Node newChild, Node? refChild) throws Exception {
			check_document(newChild);
			if(newChild.nodeType == Node.Type.DOCUMENT_FRAGMENT) {
				foreach(Node node in newChild.childNodes) {
					node._parentNode = null;
					insertBefore(node, refChild);
				}
				newChild.childNodes.clear();
				return newChild;
			}
			int index;
			if(refChild == null) {
				index = _childNodes.size;
			} else {
				check_document(refChild);
				index = _childNodes.index_of(refChild);
				if(index == -1) throw new Exception.NOT_FOUND_ERR("refChild not found");
			}
			_childNodes.insert(index, newChild);
			newChild._parentNode = this;
			return newChild;
		}

		public Node replaceChild(Node newChild, Node oldChild) throws Exception {
			check_document(newChild);
			check_document(oldChild);
			Node oldChild_ref = oldChild; /*protect the oldChild from being freed by _childNodes*/
			int index = _childNodes.index_of(oldChild);
			if(index == -1) throw new Exception.NOT_FOUND_ERR("oldChild not found");
			_childNodes.set(index, newChild);
			newChild._parentNode = this;
			oldChild_ref._parentNode = null;
			return oldChild_ref;
		}

		public Node appendChild(Node newChild) throws Exception {
			return insertBefore(newChild, null);
		}

		public Node removeChild(Node oldChild) throws Exception {
			Node oldChild_ref = oldChild;
			if(!_childNodes.remove(oldChild)) throw new Exception.NOT_FOUND_ERR("oldChild not found");
			oldChild_ref._parentNode = null;
			return oldChild_ref;
		}

		public bool hasChildNodes() { return _childNodes.size > 0; }

		public virtual Node cloneNode(bool deep = false) {
			/*FIXME*/
			return this;	
		}

		public void normalize() {
			
		}

		/* private */
		public long ref_count;
		private Quark _nodeNameQuark;
		private Gee.List<Node> _childNodes;
		private Gee.Map<Attr> _attributes;

		private void check_document(Node node) throws Exception {
			if(node.ownerDocument == this.ownerDocument) return;
			if(this.nodeType == Node.Type.DOCUMENT && node.ownerDocument == this) return;
			throw new Exception.WRONG_DOCUMENT_ERR("the node is from a differnt document");
		}
	}

}