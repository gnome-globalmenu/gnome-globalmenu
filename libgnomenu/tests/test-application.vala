
using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestApplication: TestMan {
		TestApplication () {
			base("/GlobalMenu");
			add("widget", () => {
				Gtk.Box box = new Gtk.VBox(false, 0);
				message("%p %p", window, box);
				Wnck.Screen screen = Wnck.Screen.get_default();
				screen.force_update();
				weak List<Wnck.Window> list = screen.get_windows();
				foreach(Wnck.Window win in list) {
					Application app = Application.lookup_from_wnck(win.get_application());
					if(app.proxy_item.parent != box) {
						message("app found %s", app.readable_name);
						box.pack_start_defaults(app.proxy_item);
					}
				}
				window.add(box);
				window.show_all();

				Gtk.main();
			});

		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestApplication();
			t.run();
			return 0;
		}
	}
}
