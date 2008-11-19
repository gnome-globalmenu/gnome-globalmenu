using DOM;
void insertBefore() {
	Document doc = new Document();
	DocumentFragment f = doc.createDocumentFragment();
	Element e1 = doc.createElement("e1");
	Element e2 = doc.createElement("e2");
	f.appendChild(e1);
	f.appendChild(e2);
	doc.appendChild(f);
	assert(e1.parentNode == doc);
	assert(e2.parentNode == doc);
	assert(e1.nextSibling == e2);
	assert(e2.previousSibling == e1);
}
public int main(string[] args) {
	Test.init(ref args);

	Test.add_func ("/DOM/DocumentFragment/~insertBefore", insertBefore);

	Test.run();
	return 0;
}
