using GLib;
namespace Markup {
	public abstract class XMLNode {
		public weak XMLNode parent;
		public List<XMLNode> children;
		public XMLNode (){ }
		public abstract virtual string to_string ();
	}

	public class XMLTextNode : XMLNode {
		public string text;
		public XMLTextNode(string text) {
			this.text = text;
		}
		public override string to_string () {
			return text;
		}
	}
	public class XMLSpecialNode: XMLNode {
		private string text;
		public XMLSpecialNode(string text) {
			this.text = text;
		}
		public override string to_string () {
			return text;
		}
	}
	public class XMLTagNode : XMLNode {
		public weak string tag;
		private HashTable<weak string, string> props;
		public XMLTagNode (string tag) {
			this.tag = tag;
		}
		public void set(string prop, string val) {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			props.insert(prop, val);
		}
		public void remove(string prop) {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			props.remove(prop);
		}
		public weak string? get(string prop) {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			return props.lookup(prop);
		}
		private string props_to_string() {
			if(props == null) {
				props = new HashTable<weak string, string>(str_hash, str_equal);
			}
			StringBuilder sb = new StringBuilder("");
			foreach(weak string key in props.get_keys()) {
				string escaped = GLib.Markup.escape_text(props.lookup(key));
				sb.append_printf(" %s=\"%s\"", key, escaped);
			}
			return sb.str;
		}
		public override string to_string() {
			StringBuilder sb = new StringBuilder("");
			if(this.children == null)
				sb.append_printf("<%s%s/>", tag, props_to_string());
			else {
				sb.append_printf("<%s%s>", tag, props_to_string());
				foreach(weak XMLNode child in children){
					sb.append_printf("%s", child.to_string());
				}
				sb.append_printf("</%s>", tag);
			}
			return sb.str;
		}
	}
	public class XML {
		private StringChunk strings;
		public XMLNode root;
		private weak XMLNode current;
		class XMLRootNode : XMLNode {
			public XMLRootNode(){}
			public override string to_string() {
				StringBuilder sb = new StringBuilder("");
				if(this.children == null)
					return "";
				else {
					foreach(weak XMLNode child in children){
						sb.append_printf("%s", child.to_string());
					}
				}
				return sb.str;
			}
		}
		public XML (){
			strings = new StringChunk(1024);
			root = new XMLRootNode();
		}
		public weak string S(string str) {
			return strings.insert_const(str);
		}
		[NoArrayLength]
		private static void StartElement (MarkupParseContext context, string element_name, string[] attribute_names, string[] attribute_values, void* user_data) throws MarkupError {
			weak XML xml = (XML) user_data;
			weak string tag = xml.S(element_name);
			XMLNode node = new XMLTagNode(tag);
			node.parent = xml.current;
			xml.current.children.append(node);
			xml.current = node;
			print("StartElement: %s\n", tag);

			weak XMLTagNode tagnode = node as XMLTagNode;
			for(uint i = 0; attribute_names[i]!=null; i++){
				weak string prop_name = xml.S(attribute_names[i]);
				weak string val = attribute_values[i];
				print("Prop %s = %s\n", prop_name, val);
				tagnode.set(prop_name, val);
			}
		}
		
		private static void EndElement (MarkupParseContext context, string element_name, void* user_data) throws MarkupError{
			XML xml = (XML) user_data;
			weak string tag = xml.S(element_name);
			xml.current = xml.current.parent;
			print("EndElement: %s\n", tag);
		}
		
		private static void Text (MarkupParseContext context, string text, ulong text_len, void* user_data) throws MarkupError {
			XML xml = (XML) user_data;
			string newtext = text.ndup(text_len);
			XMLTextNode node = new XMLTextNode(newtext);
			xml.current.children.append(node);
			print("Text: %s\n", newtext);
		}
		
		private static void Passthrough (MarkupParseContext context, string passthrough_text, ulong text_len, void* user_data) throws MarkupError {
			XML xml = (XML) user_data;
			string newtext = passthrough_text.ndup(text_len);
			XMLTextNode node = new XMLTextNode(newtext);
			xml.current.children.append(node);
			print("Special: %s\n", newtext);

		}
		
		private static void Error (MarkupParseContext context, GLib.Error error, void* user_data) {
			XML xml = (XML) user_data;

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
			XML xml = new XML();
			xml.parse("<?xml?>\n" +
					"<root id=\"root\">" +
					"<node id=\"node1\">"+
					"node1data" +
					"</node>\n"+
					"rootdata\n" +
					"</root>\n");
			print("back to string %s\n", xml.root.to_string());
			return 0;
		}
	}
}
