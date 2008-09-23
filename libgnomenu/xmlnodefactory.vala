using GLib;

namespace XML {
	public class SimpleNodeFactory : NodeFactory {
		private StringChunk strings;
		public SimpleNodeFactory() {
		}
		public override RootNode CreateRootNode() {
			RootNode rt = new RootNode(this);
			return rt;
		}
		public override TextNode CreateTextNode(string text) {
			TextNode rt = new TextNode(this);
			rt.text = text;
			return rt;
		}
		public override  SpecialNode CreateSpecialNode(string text) {
			SpecialNode rt = new SpecialNode(this);
			rt.text = text;
			return rt;
		}
		public override TagNode CreateTagNode(string tag) {
			TagNode rt = new TagNode(this);
			rt.tag = strings.insert_const(tag);
			return rt;
		}
		public override void FinishNode(Node node) { }
	}
}
