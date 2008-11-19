using DOM;

public void testee() throws DOM.Exception {
	throw new DOM.Exception.NOT_FOUND_ERR("");
}
public int main(string[] args) {
	try {
		testee();
	} catch(DOM.Exception e) {
		message("%s", e.message);
	}
	return 0;
}
