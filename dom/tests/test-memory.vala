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
			test_ref(doc, 1);
			test_ref(e1, 1);
			test_ref(t1, 1);
		});
		add("appendChild", () => {
			e1.appendChild(t1);
			test_ref(e1, 1);
			test_ref(t1, 2);
		});
		add("removeChild", () => {
			DOM.Node node = e1.firstChild;
			node = null;
			e1.removeChild(t1);
			test_ref(e1, 1);
			test_ref(t1, 1);
		});
		add("Document/appendChild", () => {
			doc.appendChild(t1);
			test_ref(doc, 1);
			test_ref(t1, 2);
		});
		add("Document/replaceChild", () => {
			doc.replaceChild(e1, t1);
			test_ref(doc, 1);
			test_ref(t1, 1);
			test_ref(e1, 2);
		});
		add("Document/removeChild", () => {
			DOM.Node node = doc.firstChild;
			test_ref(e1, 3);
			node = null;
			test_ref(e1, 2);
			doc.removeChild(e1);
			test_ref(doc, 1);
			test_ref(e1, 1);
		});
		add("destroy", () => {
			weak Document doc_ref = doc;
			weak DOM.Node t1_ref = t1;
			weak DOM.Node e1_ref = e1;
			doc = null;
			test_ref(doc_ref, 0);
			assert(t1_ref.ownerDocument == null);
			assert(e1_ref.ownerDocument == null);
			e1 = null;
			test_ref(doc_ref, 0);
			test_ref(e1_ref, 0);
			t1 = null;
			test_ref(doc_ref, 0);
			test_ref(t1_ref, 0);
		});
	}
	
	private void test_ref(DOM.Node node, int expected) {
		Test.message("%s ref count = %ld, expected to be %d", node.nodeName, node.ref_count, expected);
		assert(expected == node.ref_count);
	}
	public static int main(string[] args) {
		Test.init(ref args);

		MemoryTest m = new MemoryTest();
		m.run();
		return 0;
	}
}
