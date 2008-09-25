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
			weak string[] names = attribute_names;
			weak string[] values = attribute_values;
			names.length = (int) strv_length(attribute_names);
			values.length = (int) strv_length(attribute_values);
			Document.Tag node = parser.document.CreateTagWithAttributes(element_name,
				names,
				values
				);
			parser.current.append(node);
			parser.current = node;
			print("StartElement: %s\n", element_name);
			print("to string= %s\n", node.summary(0));
			print("root = %s\n", parser.document.root.to_string());
		}
		
		private static void EndElement (MarkupParseContext context, string element_name, void* user_data) throws MarkupError{
			weak Parser parser = (Parser) user_data;
			parser.current = parser.current.parent;
			print("EndElement: %s\n", element_name);
		}
		
		private static void Text (MarkupParseContext context, string text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = text.ndup(text_len);
			Document.Text node = parser.document.CreateText(newtext);
			parser.current.append(node);
			print("Text: \"%s\"\n", newtext);
		}
		
		private static void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = passthrough_text.ndup(text_len);
			Document.Special node = parser.document.CreateSpecial(newtext);
			parser.current.append(node);
			print("Special: \"%s\"\n", newtext);
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
		private class TestDocument : Object, Document {
			private Document.Root _root;
			public Document.Root root { get {return _root;}}
			public TestDocument() { }
			construct {
				_root = new Document.Root(this);
			}
		}
		public static int test (string [] args){
			Document document = new TestDocument();
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
