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
		public override RootNode CreateRootNode() {
			RootNode rt = new RootNode(this);
			rt.freeze();
			return rt;
		}
		public override TextNode CreateTextNode(string text) {
			TextNode rt = new TextNode(this);
			rt.freeze();
			rt.text = text;
			return rt;
		}
		public override  SpecialNode CreateSpecialNode(string text) {
			SpecialNode rt = new SpecialNode(this);
			rt.freeze();
			rt.text = text;
			return rt;
		}
		public override TagNode CreateTagNode(string tag) {
			TagNode rt = new TagNode(this);
			rt.freeze();
			rt.tag = S(tag);
			return rt;
		}
		public abstract virtual weak WidgetNode? lookup(string name);
		public abstract virtual WidgetNode CreateWidgetNode(string name);
		public override void FinishNode(XML.Node node) {
			node.unfreeze();
		}
	}
}
