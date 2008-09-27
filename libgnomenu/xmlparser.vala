using GLib;
namespace XML {
	public class Parser {
		private weak Node current;
		private Document.Tag result; /*the result of a PARTIAL parse*/
		private Document document;
		private enum ParseType {
			TAG, /*create a new node by parsing a tag*/
			ROOT /*create a new document*/
		}
		private ParseType type;
		public static MarkupParser parser_funcs;
		public Parser(Document document){
			this.document = document;
			parser_funcs.start_element = StartElement;
			parser_funcs.end_element = EndElement;
			parser_funcs.text = Text;
			parser_funcs.passthrough = Passthrough;
			parser_funcs.error = Error;
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
			if(parser.current == null) { /*First node in a partial parse*/
				assert(parser.type == ParseType.TAG);
				if(parser.result != null) {
					critical("parsing multiple tags for parse_tag is not supported, extra tags are ignored");
				} else {
					parser.result = node;
				}
			} else {
				parser.current.append(node);
			}
			parser.current = node;
		}
		
		private static void EndElement (MarkupParseContext context, string element_name, void* user_data) throws MarkupError{
			weak Parser parser = (Parser) user_data;
			parser.current = parser.current.parent;
		}
		
		private static void Text (MarkupParseContext context, string text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = text.ndup(text_len);
			Document.Text node = parser.document.CreateText(newtext);
			parser.current.append(node);
		}
		
		private static void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			string newtext = passthrough_text.ndup(text_len);
			Document.Special node = parser.document.CreateSpecial(newtext);
			parser.current.append(node);
		}
		
		private static void Error (MarkupParseContext context, GLib.Error error, void* user_data) {
			weak Parser parser = (Parser) user_data;

		}
		public bool parse (string foo) {
			MarkupParseContext context = new MarkupParseContext(parser_funcs, 0, (void*)this, null);
			current = document.root;
			type = ParseType.ROOT;
			try {
				context.parse(foo, foo.size());
			} catch(MarkupError e) {
				warning("%s", e.message);
				return false;
			}
			return true;
		}
		public Document.Tag? parse_tag(string foo) {
			MarkupParseContext context = new MarkupParseContext(parser_funcs, 0, (void*)this, null);
			type = ParseType.TAG;
			current = null;
			try {
				context.parse(foo, foo.size());
			} catch(MarkupError e) {
				warning("%s", e.message);
				return null;
			}
			return #result;
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
			parser.parse(
"""
<html><title>title</title>
<body name="body">
<div name="header">
	<h1> This is a header</h1>
</div>
<div name="content"></div>
<div name="tail"><br/></div>
</body>
"""
			);
			print("back to string %s\n", parser.document.root.to_string());
			Node n = parser.parse_tag(
"""
<p name="paragraph">
Sucks
</p>
"""
			);
			print("a node %s", n.to_string());
			return 0;
		}
	}
}
