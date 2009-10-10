using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestGlobalMenuBar : TestMan {
		GlobalMenuBar menubar;
		Entry entry;
		Box box;
		TestGlobalMenuBar () {
			base("/GlobalMenu");
			add("widget", () => {
				menubar = new GlobalMenuBar();
				window.add(menubar);
				window.show_all();
				window.set_keep_above(true);
				window.set_accept_focus(false);
				Gtk.main();
			});

		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestGlobalMenuBar();
			t.run();
			return 0;
		}
	}
}
