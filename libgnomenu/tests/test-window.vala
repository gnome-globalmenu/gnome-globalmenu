
using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestWindow: TestMan {
		Window test_window;
		TestWindow() {
			base("/Window");

			add("newwindow", () => {
				window.add(new Label("Expecting a lot of property notify events"));
				window.show_all();
				Gtk.Window dup = new Gtk.Window(Gtk.WindowType.TOPLEVEL);
				dup.show_all();
				Window new_win = new Window(window.window);
				new_win.set_key_widget(dup);
				dup.key_press_event += (d, e) => {
					message("key press event");
				};
				new_win.property_notify_event += (n, e) => {
					message("property event: %s", e);
				};
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
