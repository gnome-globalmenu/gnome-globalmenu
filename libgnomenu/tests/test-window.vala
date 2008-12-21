
using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestWindow: TestMan {
		Window test_window;
		TestWindow() {
			base("/Window");

			add("Native/Applet", () => {
				test_window = new Window(WindowType.TOPLEVEL);
				test_window.realize();
				test_window.destroy += Gtk.main_quit;
				test_window.menu_event += (window, path) => {
						message("menu item %s is activated", path);
				};
				test_window.show_all();
				test_window.menu_context =
					"""<menu><item label="See"/></menu>""";
				Gtk.main();
			});
			add("Foreign", () => {
				message("Test skipped");
				return;
				test_window = Window.new_from_native(0x4a0000c);
				Button btn = new Button.with_label("hello\nhhhh\n");
				btn.visible = true;
				btn.clicked += Gtk.main_quit;
				test_window.add(btn);
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
