using DOM;
Text make(string blob) {
	return new Document().createTextNode(blob);
}
void substringData() {
	CharacterData testee = make("0123456789");
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
}
void appendData() {
	CharacterData testee = make("0123456789");
	testee.appendData("++++");
	Test.message("%s", testee.data);
	assert(testee.data == "0123456789++++");
}
void insertData() {
	CharacterData testee;

	testee = make("0123456789");
	testee.insertData(0, "+");
	Test.message("%s", testee.data);
	assert(testee.data == "+0123456789");

	testee = make("0123456789");
	testee.insertData(10, "+");
	Test.message("%s", testee.data);
	assert(testee.data == "0123456789+");
}
void deleteData() {
	CharacterData testee;

	testee = make("0123456789");
	testee.deleteData(0, 1);
	Test.message("%s", testee.data);
	assert(testee.data == "123456789");

	testee = make("0123456789");
	testee.deleteData(9, 0);
	Test.message("%s", testee.data);
	assert(testee.data == "0123456789");

	testee = make("0123456789");
	testee.deleteData(9, 1);
	Test.message("%s", testee.data);
	assert(testee.data == "012345678");

	testee = make("0123456789");
	testee.deleteData(0, 10);
	Test.message("%s", testee.data);
	assert(testee.data == "");

	testee = make("0123456789");
	testee.deleteData(1, 9);
	Test.message("%s", testee.data);
	assert(testee.data == "0");
}
void replaceData() {
	CharacterData testee;
	
	testee = make("0123456789");
	testee.replaceData(0, 1, "+");
	Test.message("%s", testee.data);
	assert(testee.data == "+123456789");

	testee = make("0123456789");
	testee.replaceData(9, 0, "+");
	Test.message("%s", testee.data);
	assert(testee.data == "012345678+9");

	testee = make("0123456789");
	testee.replaceData(9, 1, "+");
	Test.message("%s", testee.data);
	assert(testee.data == "012345678+");

	testee = make("0123456789");
	testee.replaceData(0, 10, "+");
	Test.message("%s", testee.data);
	assert(testee.data == "+");

	testee = make("0123456789");
	testee.replaceData(1, 9, "+");
	Test.message("%s", testee.data);
	assert(testee.data == "0+");

}
void splitText() {
	Text testee;
	Text splitted;


	testee = make("0123456789");
	splitted = testee.splitText(3);
	Test.message("%s", testee.data);
	Test.message("%s", splitted.data);
	assert(testee.data == "012");
	assert(splitted.data == "3456789");
}
public int main(string[] args) {
	Test.init(ref args);

	Document document = new Document();

	Test.add_func ("/DOM/CharacterData/substringData", substringData);
	Test.add_func ("/DOM/CharacterData/appendData", appendData);
	Test.add_func ("/DOM/CharacterData/insertData", insertData);
	Test.add_func ("/DOM/CharacterData/deleteData", deleteData);
	Test.add_func ("/DOM/CharacterData/replaceData", replaceData);

	Test.add_func ("/DOM/Text/splitText", splitText);

	Test.run();
	return 0;
}
