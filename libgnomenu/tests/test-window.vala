
using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestWindow: TestMan {
		Gtk.Window test_window;
		TestWindow() {
			base("/Window");
			test_window = new Gtk.Window(WindowType.TOPLEVEL);

			add("TestWithApplet", () => {
				test_window.realize();
				test_window.destroy += Gtk.main_quit;
				set_menu_context(test_window.window,
					"""<menu><item label="See"/></menu>"""
					);
				test_window.show_all();
				Gtk.main();
			});
		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestWindow();

			t.run();
			return 0;
		}
	}
}
