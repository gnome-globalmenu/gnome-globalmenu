using GLib;
namespace GMarkupDoc {
	public class Parser {
		private weak Node current;
		private DocumentModel document;
		private enum ParseType {
			CHILD, /*create a new child node*/
			ROOT, /*create a new document*/
			UPDATE, /*update a node nonrecursively*/
		}
		private int pos;
		private bool first;
		private ParseType type;
		private weak string propname;
		public static MarkupParser parser_funcs;
		public Parser(DocumentModel document){
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
			switch(parser.type) {
				case ParseType.ROOT:
				case ParseType.CHILD:
					Tag node = parser.document.CreateTagWithAttributes(element_name,
						names,
						values
						);
					if((parser.type == ParseType.CHILD) && parser.first) {
						parser.first = false;
						parser.current.insert(node, parser.pos);
					} else
						parser.current.append(node);
							
					parser.current = node;
				break;
				case ParseType.UPDATE:
					if(parser.first) {
						for(int i=0; i< names.length; i++) {
							if(names[i] == parser.propname)
								(parser.current as Tag).set(parser.propname, values[i]);
						}
						parser.first = false;
					}
				break;
			}
		}
		
		private static void EndElement (MarkupParseContext context, string element_name, void* user_data) throws MarkupError{
			weak Parser parser = (Parser) user_data;
			if(parser.type == ParseType.UPDATE) return;
			parser.current = parser.current.parent;
		}
		
		private static void Text (MarkupParseContext context, string text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			if(parser.type == ParseType.UPDATE) return;
			string newtext = text.ndup(text_len);
			GMarkupDoc.Text node = parser.document.CreateText(newtext);
			parser.current.append(node);
		}
		
		private static void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			if(parser.type == ParseType.UPDATE) return;
			string newtext = passthrough_text.ndup(text_len);
			Special node = parser.document.CreateSpecial(newtext);
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
		public bool parse_child(Node parent, string foo, int pos) {
			MarkupParseContext context = new MarkupParseContext(parser_funcs, 0, (void*)this, null);
			type = ParseType.CHILD;
			this.pos = pos;
			this.first = true;
			current = parent;
			try {
				context.parse(foo, foo.size());
			} catch(MarkupError e) {
				warning("%s", e.message);
				return false;
			}
			return true;
		}
		public bool update_tag(Tag node, string propname, string foo) {
			MarkupParseContext context = new MarkupParseContext(parser_funcs, 0, (void*)this, null);
			type = ParseType.UPDATE;
			current = node;
			first = true;
			this.propname = propname;
			try {
				context.parse(foo, foo.size());
			} catch(MarkupError e) {
				warning("%s", e.message);
				return false;
			}
			return true;

		}
		private class TestDocument : Object, DocumentModel {
			private Root _root;
			public weak GMarkupDoc.Node root { get {return _root;}}
			public HashTable<weak string, Node> _dict;
			public weak HashTable<weak string, weak Node> dict {get { return _dict;}}
			public weak string name_attr {get { return S("name");}}
			public TestDocument() { }
			construct {
				_root = new Root(this);
				_dict = new HashTable<weak string, weak Node>(str_hash, str_equal);
			}
		}
		public static int test (string [] args){
			DocumentModel document = new TestDocument();
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
			return 0;
		}
	}
}
