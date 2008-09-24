using GLib;
using Gtk;
using XML;
namespace Gnomenu {
	public abstract class WidgetNode:XML.TagNode {
		public WidgetNode(NodeFactory factory) {
			this.factory = factory;
		}
		~WidgetNode(){
			message("WidgetNode %s is removed", this.get("name"));
		}
		public abstract virtual void activate();
	}
	public abstract class NodeFactory: XML.NodeFactory {
		public abstract virtual weak WidgetNode? lookup(string name);
		public abstract virtual WidgetNode CreateWidgetNode(string name);
		public override void FinishNode(XML.Node node) {
			node.unfreeze();
		}
	}
}
