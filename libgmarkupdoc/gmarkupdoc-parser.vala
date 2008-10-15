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
						(parser.current).freeze();
						if(parser.propname == null) {
							(parser.current as Tag).unset_all();
						} else {
							(parser.current as Tag).unset(parser.propname);
						}
						for(int i=0; i< names.length; i++) {
							if(parser.propname == null || names[i] == parser.propname)
								(parser.current as Tag).set(names[i], values[i]);
						}
						(parser.current).unfreeze();
						if(parser.propname == null) {
							parser.document.updated(parser.current, null);
						} else  {
							parser.document.updated(parser.current, parser.propname);
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
			if(text_len > 0) {
				string newtext = text.ndup(text_len);
				GMarkupDoc.Text node = parser.document.CreateText(newtext);
				parser.current.append(node);
			}
		}
		
		private static void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len, void* user_data) throws MarkupError {
			weak Parser parser = (Parser) user_data;
			if(parser.type == ParseType.UPDATE) return;
			if(text_len > 0) {
				string newtext = passthrough_text.ndup(text_len);
				Special node = parser.document.CreateSpecial(newtext);
				parser.current.append(node);
			}
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
		public bool update_tag(Tag node, string? propname, string foo) {
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
		public static int test (string [] args){
			DocumentModel document = new Document();
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
