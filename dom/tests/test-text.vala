using DOM;
class TextTest : TestMan {
	Document doc;
	TextTest () {
		base("/DOM/Text");
		doc = new Document();
		add("substringData", () => {
			CharacterData testee = doc.createTextNode("0123456789");
			bool caughtException;
			caughtException = false;
			try {
				testee.substringData(-1, 10);
			} catch (DOM.Exception e) {
				caughtException = true;
			}
			assert(caughtException);
			assert("01234" == testee.substringData(0, 5));
			assert("89" == testee.substringData(8, 2));
			assert("0123456789" == testee.substringData(0, 10));
		});
		add("appendData", () => {
			CharacterData testee = doc.createTextNode("0123456789");
			testee.appendData("++++");
			Test.message("%s", testee.data);
			assert(testee.data == "0123456789++++");
		});
		add("insertData", () => {
			CharacterData testee;

			testee = doc.createTextNode("0123456789");
			testee.insertData(0, "+");
			Test.message("%s", testee.data);
			assert(testee.data == "+0123456789");

			testee = doc.createTextNode("0123456789");
			testee.insertData(10, "+");
			Test.message("%s", testee.data);
			assert(testee.data == "0123456789+");
		});

		add("deleteData", () => {
			CharacterData testee;

			testee = doc.createTextNode("0123456789");
			testee.deleteData(0, 1);
			Test.message("%s", testee.data);
			assert(testee.data == "123456789");

			testee = doc.createTextNode("0123456789");
			testee.deleteData(9, 0);
			Test.message("%s", testee.data);
			assert(testee.data == "0123456789");

			testee = doc.createTextNode("0123456789");
			testee.deleteData(9, 1);
			Test.message("%s", testee.data);
			assert(testee.data == "012345678");

			testee = doc.createTextNode("0123456789");
			testee.deleteData(0, 10);
			Test.message("%s", testee.data);
			assert(testee.data == "");

			testee = doc.createTextNode("0123456789");
			testee.deleteData(1, 9);
			Test.message("%s", testee.data);
			assert(testee.data == "0");
		});
		
		add("replaceData", () => {
			CharacterData testee;
			
			testee = doc.createTextNode("0123456789");
			testee.replaceData(0, 1, "+");
			Test.message("%s", testee.data);
			assert(testee.data == "+123456789");

			testee = doc.createTextNode("0123456789");
			testee.replaceData(9, 0, "+");
			Test.message("%s", testee.data);
			assert(testee.data == "012345678+9");

			testee = doc.createTextNode("0123456789");
			testee.replaceData(9, 1, "+");
			Test.message("%s", testee.data);
			assert(testee.data == "012345678+");

			testee = doc.createTextNode("0123456789");
			testee.replaceData(0, 10, "+");
			Test.message("%s", testee.data);
			assert(testee.data == "+");

			testee = doc.createTextNode("0123456789");
			testee.replaceData(1, 9, "+");
			Test.message("%s", testee.data);
			assert(testee.data == "0+");
				
		});

		add("splitText", () => {
				
			Text testee;
			Text splitted;


			testee = doc.createTextNode("0123456789");
			splitted = testee.splitText(3);
			Test.message("%s", testee.data);
			Test.message("%s", splitted.data);
			assert(testee.data == "012");
			assert(splitted.data == "3456789");
				
		});
	}

	public static int main(string[] args) {
		Test.init(ref args);

		TextTest t = new TextTest();
		t.run();
		return 0;
	}
}
