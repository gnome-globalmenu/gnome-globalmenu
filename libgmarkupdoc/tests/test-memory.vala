using DOM;
class MemoryTest : TestMan {
	Document doc;
	Element e1;
	Text t1;

	MemoryTest() {
		base("/DOM/Memory");
		doc = new Document();
		e1 = doc.createElement("e1");
		t1 = doc.createTextNode("text1");
		add("create", () => {
			test_ref(doc, 3);
			test_ref(e1, 1);
			test_ref(t1, 1);
		});
		add("appendChild", () => {
			e1.appendChild(t1);
			test_ref(e1, 1);
			test_ref(t1, 2);
		});
		add("removeChild", () => {
			e1.removeChild(t1);
			test_ref(e1, 1);
			test_ref(t1, 1);
		});
		add("Document/appendChild", () => {
			doc.appendChild(t1);
			test_ref(doc, 3);
			test_ref(t1, 1);
		});
		add("Document/replaceChild", () => {
			doc.replaceChild(e1, t1);
			test_ref(doc, 3);
			test_ref(t1, 1);
			test_ref(e1, 1);
		});
		add("Document/removeChild", () => {
			doc.removeChild(e1);
			test_ref(doc, 3);
			test_ref(e1, 1);
		});
		add("destroy", () => {
			weak Document doc_ref = doc;
			weak DOM.Node t1_ref = t1;
			weak DOM.Node e1_ref = e1;
			doc = null;
			test_ref(doc_ref, 2);
			e1 = null;
			test_ref(doc_ref, 1);
			test_ref(e1_ref, 0);
			t1 = null;
			test_ref(doc_ref, 0);
			test_ref(t1_ref, 0);
		});
	}
	
	private void test_ref(DOM.Node node, int expected) {
		Test.message("%s ref count = %ld", node.nodeName, node.ref_count);
		assert(expected == node.ref_count);
	}
}
public int main(string[] args) {
	Test.init(ref args);

	MemoryTest m = new MemoryTest();
	m.run();
	return 0;
}
