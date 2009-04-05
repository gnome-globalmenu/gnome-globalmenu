using Gtk;

public class TestMan : GLib.Object {
	public string uri {get; construct; }
	public Gtk.Window window;
	public TestMan(string uri) {
		this.uri = uri;
	}
	public void add(string name, DataTestFunc func) {
		Test.add_data_func(uri + "/" + name, func);
	}
	public void run() {
		create_test_window();
		Test.run();
		window.destroy();
		window = null;
	}
	public void create_test_window() {
		window = new Gtk.Window(WindowType.TOPLEVEL);
		message("window created");
		window.delete_event += (window) => {
			window.hide_on_delete();
			Gtk.main_quit();
			window.unrealize();
			return true;
		};
	}
}
