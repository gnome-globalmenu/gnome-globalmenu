using GLib;
namespace XML {
	public class Parser {
		private weak Node current;
		private Document document;
		public Parser(Document document){
			this.document = document;
		}
		[NoArrayLength]
		private static void StartElement (MarkupParseContext context, string element_name, string[] attribute_names, string[] attribute_values, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			Node node = parser.document.CreateTagNode(element_name);
			parser.current.append(node);
			parser.current = node;
			print("StartElement: %s\n", element_name);

			weak TagNode tagnode = node as TagNode;
			for(uint i = 0; attribute_names[i]!=null; i++){
				weak string prop_name = attribute_names[i];
				weak string val = attribute_values[i];
				print("Prop %s = %s\n", prop_name, val);
				tagnode.set(prop_name, val);
			}
			parser.document.FinishNode(node);
		}
		
		private static void EndElement (MarkupParseContext context, string element_name, void* user_data) throws MarkupError{
			weak Parser parser = (Parser) user_data;
			parser.current = parser.current.parent;
			print("EndElement: %s\n", element_name);
		}
		
		private static void Text (MarkupParseContext context, string text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = text.ndup(text_len);
			TextNode node = parser.document.CreateTextNode(newtext);
			parser.current.append(node);
			print("Text: %s\n", newtext);
			parser.document.FinishNode(node);
		}
		
		private static void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = passthrough_text.ndup(text_len);
			SpecialNode node = parser.document.CreateSpecialNode(newtext);
			parser.current.append(node);
			print("Special: %s\n", newtext);
			parser.document.FinishNode(node);
		}
		
		private static void Error (MarkupParseContext context, GLib.Error error, void* user_data) {
			weak Parser parser = (Parser) user_data;

		}
		public bool parse (string foo) {
			MarkupParser parser = {
				StartElement,
				EndElement, 
				Text, 
				Passthrough,
				Error
			};
			MarkupParseContext context = new MarkupParseContext(parser, 0, (void*)this, null);
			current = document.root;
			try {
				context.parse(foo, foo.size());
			} catch(MarkupError e) {
				warning("%s", e.message);
				return false;
			}
			return true;
		}
		private class TestFactory : Document {
			public TestFactory() { }
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
				rt.tag = S(tag);
				return rt;
			}
			public override void FinishNode(Node node) { }
		}
		public static int test (string [] args){
			Document document = new TestFactory();
			Parser parser = new Parser(document);
			parser.parse("<?xml?>\n" +
					"<docroot id=\"root\">" +
					"<node id=\"node1\">"+
					"node1data" +
					"</node>\n"+
					"rootdata\n" +
					"</docroot>\n");
			print("back to string %s\n", parser.document.root.to_string());
			return 0;
		}
	}
}
