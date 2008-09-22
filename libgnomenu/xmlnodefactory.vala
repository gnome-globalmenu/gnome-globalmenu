using GLib;

namespace XML {
	public class SimpleNodeFactory : NodeFactory {
		private StringChunk strings;
		public SimpleNodeFactory() {
		}
		construct {
			strings = new StringChunk(1024);
		}
		public override weak string S(string s) {
			return strings.insert_const(s);
		}
		public override RootNode CreateRootNode() {
			return new RootNode(this);
		}
		public override TextNode CreateTextNode(string text) {
			TextNode rt = new TextNode(this);
			rt.text = text;
			return rt;
		}
		public override SpecialNode CreateSpecialNode(string text) {
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
