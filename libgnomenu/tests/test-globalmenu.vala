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
				box = new VBox(false, 0);
				menubar = new GlobalMenu();
				entry = new Entry();
				box.pack_start_defaults(entry);
				box.pack_start_defaults(menubar);
				entry.activate += (the_entry) => {
					menubar.switch_to(entry.text.to_ulong());
				};
				window.add(box);
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
