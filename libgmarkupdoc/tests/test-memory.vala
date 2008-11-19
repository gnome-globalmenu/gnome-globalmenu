using DOM;

void refcounts() {
	Document doc = new Document();
	message("%ld", doc.ref_count);
	assert(doc.ref_count == 1);

	Element e1 = doc.createElement("e1");
	message("%ld", doc.ref_count);
	assert(doc.ref_count == 1);

	Text t1 = doc.createTextNode("text1");
	message("%ld", doc.ref_count);
	assert(doc.ref_count == 1);

	e1.appendChild(t1);
	message("%ld", doc.ref_count);
	assert(doc.ref_count == 1);

	e1.removeChild(t1);
	message("%ld", doc.ref_count);
	assert(doc.ref_count == 1);

	doc.appendChild(t1);
	message("%ld", doc.ref_count);
	assert(doc.ref_count == 1);

	doc.replaceChild(e1, t1);

	message("%ld", doc.ref_count);
	assert(e1.ref_count == 2);
	assert(t1.ref_count == 1);
	assert(doc.ref_count == 1);
}
public int main(string[] args) {
	Test.init(ref args);

	Test.add_func ("/DOM/refcounts", refcounts);

	Test.run();
	return 0;
}
