
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
				Application.init();
				screen.force_update();
				foreach(Application app in Application.applications) {
					if(app.wnck_applications != null) {
						box.pack_start_defaults(app.proxy_item);
						if(app.not_in_menu) {
						message("%s %s not in menu",app.readable_name, app.exec_path);
						
						}
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
