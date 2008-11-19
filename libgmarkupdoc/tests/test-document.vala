using DOM;
void getElementById() {
}
class DocumentTest:TestMan {
	Document doc;

	Element e1_ref;
	Element e2_ref;
	Element e1;
	Element e2;
	DocumentTest () {
		base("/DOM/Document");
   		doc = new Document();
		e1 = doc.createElement("e");
		e2 = doc.createElement("e");

		add ("getElementById/setId", () => {
			e1.setAttribute("id", "e1");
			e1_ref = doc.getElementById("e1");
			assert(e1_ref == e1);
		});

		add ("getElementById/changeId", () => {
			e1.setAttribute("id", "e1new");
			e1_ref = doc.getElementById("e1");
			assert(e1_ref == null);
			e1_ref = doc.getElementById("e1new");
			assert(e1_ref == e1);
		});
		
		add ("getElementById/conflictId", () => {
			e2.setAttribute("id", "e1new");
			e1_ref = doc.getElementById("e1new");
			assert(e1_ref == e1);
		});

		add ("getElementbyId/resolveConflictId", () => {
			e2.setAttribute("id", "e2");
			e2_ref = doc.getElementById("e2");
			assert(e2_ref == e2);
		});
	}
	
}

public int main(string[] args) {
	Test.init(ref args);
	TestMan c = new DocumentTest();
	c.run();
	return 0;
}
