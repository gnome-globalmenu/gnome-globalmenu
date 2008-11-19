using GLib;
using Gee;

namespace DOM {
	public class Entity : Node {
		public Entity (string name) {
			base(null, Node.Type.ENTITY_NODE, name);
		}
		/* Entity Interface */
		public string notationName;
		public string publicId;
		public string systemId;
	}
	public class Notation : Node {
		public Notation (string name) {
			base(null, Node.Type.NOTATION, name);
		}
		/* Notation Interface */
		public string publicId;
		public string systemId;
	}

	public class DocumentType : Node {
		public DocumentType (string name) {
			base(null, Node.Type.DOCUMENT_TYPE, name);
		}
		/* DocumentType Interface */
		public string name {
			get {return nodeName;}
		}
		public Gee.Map<weak string, Entity> entities;
		public Gee.Map<weak string, Notation> notations;
		public string publicId;
		public string systemId;
		public string internalSubset;
	}
}
