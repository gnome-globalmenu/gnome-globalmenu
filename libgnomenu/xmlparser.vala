using GLib;
namespace XML {
	public class Parser {
		public Node root;
		private weak Node current;
		private NodeFactory factory;
		public Parser(NodeFactory # factory){
			this.factory = #factory;
			root = this.factory.CreateRootNode();
		}
		[NoArrayLength]
		private static void StartElement (MarkupParseContext context, string element_name, string[] attribute_names, string[] attribute_values, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			Node node = parser.factory.CreateTagNode(element_name);
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
			parser.factory.FinishNode(node);
		}
		
		private static void EndElement (MarkupParseContext context, string element_name, void* user_data) throws MarkupError{
			weak Parser parser = (Parser) user_data;
			parser.current = parser.current.parent;
			print("EndElement: %s\n", element_name);
		}
		
		private static void Text (MarkupParseContext context, string text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = text.ndup(text_len);
			TextNode node = parser.factory.CreateTextNode(newtext);
			parser.current.append(node);
			print("Text: %s\n", newtext);
			parser.factory.FinishNode(node);
		}
		
		private static void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = passthrough_text.ndup(text_len);
			SpecialNode node = parser.factory.CreateSpecialNode(newtext);
			parser.current.append(node);
			print("Special: %s\n", newtext);
			parser.factory.FinishNode(node);
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
			current = root;
			try {
				context.parse(foo, foo.size());
			} catch(MarkupError e) {
				warning("%s", e.message);
				return false;
			}
			return true;
		}
		public static int test (string [] args){
			SimpleNodeFactory factory = new SimpleNodeFactory();
			Parser parser = new Parser(factory);
			parser.parse("<?xml?>\n" +
					"<root id=\"root\">" +
					"<node id=\"node1\">"+
					"node1data" +
					"</node>\n"+
					"rootdata\n" +
					"</root>\n");
			print("back to string %s\n", parser.root.to_string());
			return 0;
		}
	}
}
