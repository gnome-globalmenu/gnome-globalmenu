using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestGlobalMenuItem : TestMan {
		Gtk.MenuBar menubar;
		TestGlobalMenuItem() {
			base("/GlobalMenuItem");
			add("widget", () => {
				menubar = new Gnomenu.MenuBar();
				Gtk.Box box = new Gtk.VBox(false, 0);
				Gtk.MenuItem item = new Gnomenu.GlobalMenuItem();
				menubar.set_size_request(100, 200);
				menubar.append(item);
				box.pack_start(menubar, true, true, 0);
				window.add(box);
				window.show_all();
				window.set_keep_above(true);
				window.set_accept_focus(false);
				Gtk.main();
			});

		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestGlobalMenuItem();
			t.run();
			return 0;
		}
	}
}
