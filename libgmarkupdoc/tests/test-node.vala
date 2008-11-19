using DOM;
void insertBefore() {
	Document doc = new Document();
	Element n1 = doc.createElement("n1");
	assert(n1.ownerDocument == doc);
	doc.insertBefore(n1, null);
	assert(n1.parentNode == doc);
	assert(n1.nextSibling == null);
	assert(n1.previousSibling == null);

	Element n2 = doc.createElement("n2");
	doc.insertBefore(n2, n1);
	assert(n2.nextSibling == n1);
	assert(n1.previousSibling == n2);

	Element n3 = doc.createElement("n3");
	n1.insertBefore(n3, null);
	assert(n1.firstChild == n3);
}
void replaceChild() {
	Document doc = new Document();
	Element n1 = doc.createElement("n1");
	assert(n1.ownerDocument == doc);
	doc.insertBefore(n1, null);

	Element n2 = doc.createElement("n2");
	doc.replaceChild(n2, n1);

	assert(n1.parentNode == null);
	assert(n2.parentNode == doc);
}

void removeChild() {
	Document doc = new Document();
	Element n1 = doc.createElement("n1");
	assert(n1.ownerDocument == doc);
	doc.insertBefore(n1, null);
	DOM.Node n1_ref = doc.removeChild(n1);
	assert(n1_ref == n1);
	assert(n1 is Element);
	assert(n1.parentNode == null);
}
void hasChildNodes () {
	
	Document doc = new Document();
	assert(doc.hasChildNodes() == false);
	Element n1 = doc.createElement("n1");
	doc.appendChild(n1);
	assert(doc.hasChildNodes() == true);
	doc.removeChild(n1);
	assert(doc.hasChildNodes() == false);
}
public int main(string[] args) {
	Test.init(ref args);

	Test.add_func ("/DOM/Node/insertBefore", insertBefore);
	Test.add_func ("/DOM/Node/replaceChild", replaceChild);
	Test.add_func ("/DOM/Node/removeChild", removeChild);
	Test.add_func ("/DOM/Node/hasChildNodes", hasChildNodes);

	Test.run();
	return 0;
}
