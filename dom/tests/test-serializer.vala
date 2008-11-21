using DOM;
class SerializerTest: TestMan {
	Document doc;
	SerializerTest () {
		base("/DOM/Serializer");
		doc = new Document();
		Element e1 = doc.createElement("e");
		Element e2 = doc.createElement("e");
		doc.appendChild(e1);
		e1.appendChild(e2);
		e1.setAttribute("id", "e1");
		e2.setAttribute("id", "e2");
		Text t1 = doc.createTextNode("This is a text node");
		e1.appendChild(t1);

		add("writeToString/nonpretty", () => {
			Serializer s = new Serializer();
			s.format_pretty_print = false;
			string result =  """<e id="e1"><e id="e2"/>This is a text node</e>""";
			string s_result = s.writeToString(doc);
			Test.message("%s", s_result);
			assert(s_result == result);
		});

		add("writeToString/pretty", () => {
			Serializer s = new Serializer();
			s.format_pretty_print = true;

			string result =  """<e id="e1">
 <e id="e2"/>
 This is a text node
</e>
""";
			string s_result = s.writeToString(doc);
			Test.message("%s", s_result);
			assert(s_result == result);
		});
	
	}
	public static int main(string[] args) {
		Test.init(ref args);
		SerializerTest t = new SerializerTest();
		t.run();
		return 0;
	}
}
