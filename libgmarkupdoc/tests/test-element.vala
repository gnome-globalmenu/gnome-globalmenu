using DOM;
void getAttribute() {
	Document doc = new Document();
	Element e1 = doc.createElement("e1");
	assert(e1.getAttribute("p1") == null);
	e1.setAttribute("p1", "v1");
	assert(e1.getAttribute("p1") == "v1");
}
void setAttribute() {
	Document doc = new Document();
	Element e1 = doc.createElement("e1");
	e1.setAttribute("p1", "v1");
	assert(e1.getAttribute("p1") == "v1");
}
void removeAttribute() {
	Document doc = new Document();
	Element e1 = doc.createElement("e1");
	e1.setAttribute("p1", "v1");
	assert(e1.getAttribute("p1") == "v1");
	e1.removeAttribute("p1");
	assert(e1.hasAttribute("p1") == false);
}

void getAttributeNode() {
	Document doc = new Document();
	Element e1 = doc.createElement("e1");
	Attr a1 = e1.getAttributeNode("p1");
	a1.value = "v1";
	assert(a1.value == "v1");
	assert(e1.getAttribute("p1") == "v1");
}
void setAttributeNode() {
	Document doc = new Document();
	Element e1 = doc.createElement("e1");
	Attr a1 = doc.createAttribute("p1");
	assert(a1.name == "p1");
	Attr a1_ref = e1.setAttributeNode(a1);
	assert(a1_ref == a1);
	assert(e1.getAttributeNode("p1") == a1);
}
void removeAttributeNode() {
	Document doc = new Document();
	Element e1 = doc.createElement("e1");
	e1.setAttribute("p1", "v1");
	Attr a1 = e1.getAttributeNode("p1");
	e1.removeAttributeNode(a1);
	assert(e1.hasAttribute(a1.name) == false);
}

void hasAttribute() {
	Document doc = new Document();
	Element e1 = doc.createElement("e1");
	e1.setAttribute("p1", "v1");
	Attr a1 = e1.getAttributeNode("p1");
	e1.removeAttributeNode(a1);
	assert(e1.hasAttribute(a1.name) == false);
}
void getElementsByTagName() {
	Document doc = new Document();
	Element e1 = doc.createElement("e");
	Element e2 = doc.createElement("e");
	Element e3 = doc.createElement("e");
	Element e4 = doc.createElement("e");
	Element e5 = doc.createElement("e");
	Element e6 = doc.createElement("e");
	e1.appendChild(e2);
	e1.appendChild(e3);
	e1.appendChild(e4);
	e2.appendChild(e5);
	e5.appendChild(e6);
	Gee.List<Element> list = e1.getElementsByTagName("e");
	assert(list.get(0) == e1);
	assert(list.get(1) == e2);
	assert(list.get(2) == e5);
	assert(list.get(3) == e6);
	assert(list.get(4) == e3);
	assert(list.get(5) == e4);
}
public int main(string[] args) {
	Test.init(ref args);

	Test.add_func ("/DOM/Element/setAttributeNode", setAttributeNode);
	Test.add_func ("/DOM/Element/getAttributeNode", getAttributeNode);
	Test.add_func ("/DOM/Element/removeAttributeNode", removeAttributeNode);
	Test.add_func ("/DOM/Element/hasAttribute", hasAttribute);
	Test.add_func ("/DOM/Element/getAttribute", getAttribute);
	Test.add_func ("/DOM/Element/setAttribute", setAttribute);
	Test.add_func ("/DOM/Element/removeAttribute", removeAttribute);
	Test.add_func ("/DOM/Element/getElementsByTagName", getElementsByTagName);

	Test.run();
	return 0;
}
