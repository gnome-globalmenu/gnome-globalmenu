using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestGlobalMenu : TestMan {
		GlobalMenu menubar;
		Entry entry;
		Box box;
		TestGlobalMenu () {
			base("/GlobalMenu");
			add("widget", () => {
				menubar = new GlobalMenu();
				window.add(menubar);
				window.show_all();
				Gtk.main();
			});

		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestGlobalMenu();
			t.run();
			return 0;
		}
	}
}
