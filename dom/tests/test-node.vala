using DOM;
class NodeTest: TestMan {
	Document doc;
	NodeTest () {
		base("/DOM/Node");
		add("insertBefore", () => {
			doc = new Document.full(null, "root", null);
			Element n1 = doc.createElement("n1");
			assert(n1.ownerDocument == doc);
			doc.documentElement.insertBefore(n1, null);
			assert(n1.parentNode == doc.documentElement);
			assert(n1.nextSibling == null);
			assert(n1.previousSibling == null);

			Element n2 = doc.createElement("n2");
			doc.documentElement.insertBefore(n2, n1);

			assert(n2.nextSibling == n1);
			assert(n1.previousSibling == n2);

			Element n3 = doc.createElement("n3");
			n1.insertBefore(n3, null);
			assert(n1.firstChild == n3);
		});
		add("replaceChild", () => {
			Element n1 = doc.createElement("n1");
			assert(n1.ownerDocument == doc);
			doc.documentElement.insertBefore(n1, null);

			Element n2 = doc.createElement("n2");
			doc.documentElement.replaceChild(n2, n1);

			assert(n1.parentNode == null);
			assert(n2.parentNode == doc.documentElement);
		});
		add("removeChild", () => {
			Element n1 = doc.createElement("n1");
			assert(n1.ownerDocument == doc);
			doc.documentElement.insertBefore(n1, null);
			DOM.Node n1_ref = doc.documentElement.removeChild(n1);
			assert(n1_ref == n1);
			assert(n1 is Element);
			assert(n1.parentNode == null);
		});
		add("hasChildNodes ", () => {
			doc = new Document();
			assert(doc.hasChildNodes() == false);
			Element n1 = doc.createElement("n1");
			doc.appendChild(n1);
			assert(doc.hasChildNodes() == true);
			doc.removeChild(n1);
			assert(doc.hasChildNodes() == false);
		});
	}

public static int main(string[] args) {
	Test.init(ref args);

	NodeTest  t = new NodeTest();
	t.run();
	return 0;
}}
