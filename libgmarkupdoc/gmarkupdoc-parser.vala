using GLib;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	public class DocumentParser {
		private Queue<weak Node> queue;
		private DocumentModel document;
		public DocumentParser(DocumentModel document){
			this.document = document;
		}
		[NoArrayLength]
		private void StartElement (MarkupParseContext context, string element_name, string[] attribute_names, string[] attribute_values) {
			weak Node current = queue.peek_head();
			weak string[] names = attribute_names;
			weak string[] values = attribute_values;
			names.length = (int) strv_length(attribute_names);
			values.length = (int) strv_length(attribute_values);
			if(element_name == "gmarkup:meta"){
				Node node;
				node = document.createMeta();
				node.freeze();
				for(int i=0; i<names.length; i++) {
					node.set(names[i], values[i]);
				}
				node.thaw();
				current.append(node.ref() as Node);
				queue.push_head(node);
			}
			else {
				weak Node node;
				node = document.createElement(element_name);
				node.freeze();
				for(int i=0; i<names.length; i++) {
					node.set(names[i], values[i]);
				}
				node.thaw();
				current.append(node);
				queue.push_head(node);
			}
		}
		
		private void EndElement (MarkupParseContext context, string element_name) {
			queue.pop_head();
		}
		
		private void Text (MarkupParseContext context, string text, ulong text_len) {
			weak Node current = queue.peek_head();
			if(text_len > 0) {
				string newtext = text.ndup(text_len);
				current.append(document.createText(newtext));
			}
		}
		
		private void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len) {
			weak Node current = queue.peek_head();
			if(text_len > 0) {
				string newtext = passthrough_text.ndup(text_len);
				current.append(document.createSpecial(newtext));
			}
		}
		
		private void Error (MarkupParseContext context, GLib.Error error) {

		}
		public Node parse (string foo) {
			MarkupParser parser_funcs = { StartElement, EndElement, Text, Passthrough, Error};
			MarkupParseContext context = new MarkupParseContext(parser_funcs, 0, (void*)this, null);
			Node rt = document.createFragment();
			queue = new Queue<weak Node>();
			queue.push_head(rt);
			try {
				context.parse(foo, foo.size());
			} catch (MarkupError e) {
				warning("%s", e.message);
				return rt;
			}
			return rt;
		
		}
		public static int test (string [] args){
			DocumentModel document = new Document();
			DocumentParser parser = new DocumentParser(document);
			Node segment = parser.parse(
"""
<html/>
<html><title>title</title>
<body name="body">
<div name="header">
	<h1> This is a header</h1>
</div>
<div name="content"></div>
<div name="tail"><br/></div>
</body>
</html>
"""
			);
			document.inserted += (d, node, child, ref_node) => {
				message("inserted %s %s", node.name, child.name);
			};
			document.root.append(segment);
			print("back to string %s\n", parser.document.root.to_string());
			string meta = document.root.to_meta_string();
			segment = parser.parse("""
					<gmarkup:meta id="42" name="test">
					<test>
						<gmarkup:meta id="43" name="test222">
						<test222/>
						</gmarkup:meta>
					</test>
					</gmarkup:meta>
					""");
			print("segment %s\n", segment.to_string());
			document.mergeMeta(segment, document.root, null);
			print("back to string %s\n", parser.document.root.to_string());
			document.memcheck();
			segment = parser.parse("""
					<gmarkup:meta id="3" name="#TEXT"><test/>
					</gmarkup:meta>
					""");
			weak Node node = document.getNode(2);
			print("node = %s", node.to_string());
			document.mergeMeta(segment, document.root, null);
			print("back to string %s\n", parser.document.root.to_string());
			document.memcheck();
			return 0;
		}
	}
}
