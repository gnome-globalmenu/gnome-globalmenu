using DOM;
void getElementById() {
	Document doc = new Document();
	Element e1_ref;
	Element e2_ref;

	{
		Element e1 = doc.createElement("e");
		Element e2 = doc.createElement("e");
		assert(e1 is Element);

		e1.setAttribute("id", "e1");
		e1_ref = doc.getElementById("e1");
		assert(e1_ref == e1);

		e1.setAttribute("id", "e1new");
		e1_ref = doc.getElementById("e1");
		assert(e1_ref == null);

		e1_ref = doc.getElementById("e1new");
		assert(e1_ref == e1);


		e2.setAttribute("id", "e1new");
		e1_ref = doc.getElementById("e1new");
		assert(e1_ref == e1);
		
		e2.setAttribute("id", "e2");
		e2_ref = doc.getElementById("e2");
		assert(e2_ref == e2);
		e1_ref = null;
		e2_ref = null;
	}
	/*e1 is destroyed here*/
	e1_ref = doc.getElementById("e1new");
	assert(e1_ref == null);
}
public int main(string[] args) {
	Test.init(ref args);

	Test.add_func ("/DOM/Document/getElementById", getElementById);

	Test.run();
	return 0;
}
