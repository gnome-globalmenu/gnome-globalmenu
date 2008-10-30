using GLib;
using GLibCompat;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	/**
	 * The document model for GMarkup. It is some kind of different from the DOM model.
	 */
	public interface DocumentModel: GLib.Object {
		private static StringChunk strings = null;
		private static uint unique;
		/**
		 * The root of the document.
		 */
		public abstract weak Node root {get;}
		/**
		 * The collection of all orphans. 
		 * Orphans are newly created nodes or nodes that haven't been attached the the root.
		 */
		public abstract weak Node orphan {get;}
		/**
		 * The dictionary for looking up nodes from their names.
		 */
		public abstract weak HashTable<weak string, weak Node> dict {get;}
		/**
		 * The attribute used to determine the name of a node for a Tag .
		 */
		public abstract weak string name_attr {get;}
		/**
		 * Create a Node that contains text in the document. 
		 *
		 *   @return a newly created node; unref it when not used.
		 */
		public virtual Text CreateText(string text) {
			Text rt = new Text(this, text);
			rt.name = S("TEXT" + unique.to_string());
			unique++;
			return rt;
		}
		/**
		 * Create a Node that contains special segments in the document. 
		 * Example, <?xml ....?>
		 *
		 *   @return a newly created node; unref it when not used.
		 */
		public virtual Special CreateSpecial(string text) {
			Special rt =new Special(this, text);
			rt.name =  S("SPECIAL" + unique.to_string());
			unique++;
			return rt;
		}
		/**
		 * Create a Node that contains a tag in the document. 
		 * Example, <body name="mybody"/>
		 *	 @param tag  			'body' in our example;
		 *   @return a newly created node; unref it when not used.
		 */
		public virtual Tag CreateTag(string tag) {
			Tag rt = new Tag(this, tag);
			rt.name = S("TAG" + unique.to_string());
			unique++;
			return rt;
		}

		/**
		 * Create a Node that contains a tag in the document.
		 * Merely a wrapper of { @link CreateTag }.
		 * Example: <body name="mybody"/>
		 *   @param tag     		'body' in our example;
		 *   @param attr_names 	names of the attributes;
		 *   @param attr_values values of the attributes;
		 *
		 *   @return a newly created node; unref it when not used.
		 */
		public virtual Tag CreateTagWithAttributes(string tag, 
				string[] attr_names, string[] attr_values) {
			Tag t = CreateTag(tag);
			t.set_attributes(attr_names, attr_values);
			return t;
		}
		/**
		 * Destroy a Node if it is not used in any context. In other words,
		 * if it is a child of the orphan node.
         * Panic if the node is not a child of the orphan node.
		 *
		 *   @param node  		the node to be removed.
		 *
		 */
		public virtual void DestroyNode(Node? node) {
			assert(node.parent == orphan);
			orphan.remove(node);
			transverse(node, (node) => {
				debug("%s  %u", node.name, node.ref_count);
//				assert(node.ref_count == 1);
				dict.remove(node.name);
			});
		}
		/**
		 * Make a static string.
		 *
		 *	 @param s  		the string to be made static.
		 *
		 *   @return the static string. Never free it.
		 */
		public virtual weak string S(string? s) {
			if(strings == null) {
				strings = new StringChunk(1024);
				unique = 1;
			}
			if(s == null) return null;
			return strings.insert_const(s);
		}
		/**
		 * Emitted when an attribute of a node is updated
		 *
		 * 	 @param node 		the node
		 * 	 @param prop 		the name of the attribute.
		 */
		public abstract signal void updated(Node node, string? prop);
		/**
		 * Emitted when a node is inserted to another node.
		 * 	 @param parent 		the parent
		 * 	 @param node 		the node
		 * 	 @param pos 		the position where the insertion is made
		 */
		public abstract signal void inserted(Node parent, Node node, int pos);
		/**
		 * Emitted when a node is removed from another node. 
		 * Notice that the node is not destroyed by this removal.
		 * 	 @param parent 		the parent
		 * 	 @param node 		the node
		 */
		public abstract signal void removed(Node parent, Node node);
		/**
		 * Emitted when a node is renamed.
		 */
		public abstract signal void renamed(Node node, string? oldname, string? newname);
		public abstract signal void destroyed();

		/**
		 * Transverse the document tree from the given node.
		 *
		 * 	 @param node 		the begin of the trip.
		 * 	 @param func 				the callback.
		 */
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
