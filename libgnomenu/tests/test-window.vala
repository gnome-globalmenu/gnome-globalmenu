
using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestWindow: TestMan {
		Window test_window;
		TestWindow() {
			base("/Window");
			test_window = new Window(WindowType.TOPLEVEL);

			add("TestWithApplet", () => {
				test_window.realize();
				test_window.destroy += Gtk.main_quit;
				test_window.property_changed += (window, property) => {
					if(property == "_NET_GLOBALMENU_MENU_EVENT") {
						message("menu item %s is activated",
							window.get(property));
					}
				};
				test_window.show_all();
				test_window.set("_NET_GLOBALMENU_MENU_CONTEXT",
					"""<menu><item label="See"/></menu>"""
					);
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
