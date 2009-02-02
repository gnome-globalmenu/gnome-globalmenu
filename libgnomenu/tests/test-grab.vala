using Gtk;
using Gnomenu;

namespace Gnomenu {
	class TestGrab: TestMan {
		TestGrab() {
			base("/Grab");

			add("Grab", () => {
				Window root = new Window(Gdk.get_default_root_window());
				root.set_key_widget(window);
				window.add(new Label("Switch to another window and Press a"));
				window.show_all();

				uint keyval = Gdk.keyval_from_name("A");
				if(false == root.grab_key(keyval, 0)) {
					message("grab failed");
				}
				window.key_press_event += (w, event) => {
					message("key: %s", Gdk.keyval_name(event.keyval));
				};
				Gtk.main();
			});
		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestGrab();

			t.run();
			return 0;
		}
	}
}
