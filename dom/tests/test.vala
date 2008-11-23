
public class TestMan {
	public string uri {get; construct; }
	public TestMan(string uri) {
		this.uri = uri;
	}
	public void add(string name, DataTestFunc func) {
		Test.add_data_func(uri + "/" + name, func);
	}
	public void run() {
		Test.run();
	}
}
