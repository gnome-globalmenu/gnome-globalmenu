using DOM;
class ToStringTest: TestMan {
	Document doc;
	string result =  """<e id="e1"><e id="e2"/>This is a text node</e>""";
	ToStringTest () {
		base("/DOM/ToStringTest");
		doc = new Document();
		Element e1 = doc.createElement("e");
		Element e2 = doc.createElement("e");
		doc.appendChild(e1);
		e1.appendChild(e2);
		e1.setAttribute("id", "e1");
		e2.setAttribute("id", "e2");
		Text t1 = doc.createTextNode("This is a text node");
		e1.appendChild(t1);

		add("toString", () => {
			ToString v = new ToString();
			v.visit(doc);
			Test.message(v.result);
			assert(v.result == result);
		});
	
	}
	public static int main(string[] args) {
		Test.init(ref args);
		ToStringTest t = new ToStringTest();
		t.run();
		return 0;
	}
}
