using GLib;
using Gee;

namespace DOM {
	public enum Stage {
		START,
		ATTRIBUTES_END,
		END,
	}
	public class Visitor {
		public virtual void visitAttribute (Attr attr, Stage stage) { assert_not_reached(); }
		public virtual void visitTextNode (Text text, Stage stage) { assert_not_reached(); }
		public virtual void visitElement (Element element, Stage stage) { assert_not_reached(); }
		public virtual void visitComment (Comment comment, Stage stage) { assert_not_reached(); }
		public virtual void visitDocument (Document document, Stage stage) { assert_not_reached();}
		public void visit(Node node) {
			visit_node_at_stage(node, Stage.START);
			foreach(Node child in node.childNodes) {
				visit(child);
			}
			visit_node_at_stage(node, Stage.END);
		}
		private void visit_node_at_stage(Node node, Stage stage) {
			switch(node.nodeType) {
				case Node.Type.ELEMENT:
					assert(node is Element);
					visitElement(node as Element, stage);
					if(stage == Stage.START) {
						foreach(Node attr in node.attributes.get_values()) {
							visit(attr);
						}
						visitElement(node as Element, Stage.ATTRIBUTES_END);
					}
				break;
				case Node.Type.ATTRIBUTE:
					visitAttribute(node as Attr, stage);
				break;
				case Node.Type.TEXT:
					visitTextNode(node as Text, stage);
				break;
				case Node.Type.COMMENT:
					visitComment(node as Comment, stage);
				break;
				case Node.Type.DOCUMENT:
					visitDocument(node as Document, stage);
				break;
			}
		}
	}
}
