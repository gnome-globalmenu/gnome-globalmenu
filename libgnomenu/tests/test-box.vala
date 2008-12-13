using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestBox: TestMan {
		Gtk.Window window;
		MenuBar menubar1;
		MenuBar menubar2;
		MenuBar menubar3;
		Box box;
		static string test1 =
"""
<menu> <item id="Menu1"/> </menu>
""";
		static string test2 =
"""
<menu> <item id="Menu2"/> </menu>
""";
		static string test3 =
"""
<menu> <item id="Menu3"/> </menu>
""";
		TestBox () {
			base("/Box");
			window = new Gtk.Window(WindowType.TOPLEVEL);
			Gdk.Color color;
			Gdk.Color.parse("#0000ff", out color);
			window.modify_bg(StateType.NORMAL, color);
			menubar1 = new MenuBar();
			menubar2 = new MenuBar();
			menubar3 = new MenuBar();
			box = new Box();
			add("Basic", () => {
					assert(window is Gtk.Window);
				Parser.parse( menubar1, test1);
				Parser.parse( menubar2, test2);
				Parser.parse( menubar3, test3);
				box.pack_start_defaults(menubar1);
				box.pack_start_defaults(menubar2);
				box.pack_start_defaults(menubar3);
				window.add(box);
				window.destroy += Gtk.main_quit;
				window.show_all();		
				Gtk.main();
			});
			add("Gravity", () => {
				Parser.parse( menubar1, test1);
				Parser.parse( menubar2, test2);
				Parser.parse( menubar3, test3);
				box.pack_start_defaults(menubar1);
				box.pack_start_defaults(menubar2);
				box.pack_start_defaults(menubar3);
				window.add(box);
				box.gravity = Gravity.UP;
				window.destroy += Gtk.main_quit;
				window.show_all();
				Gtk.main();
			});
			add("Pack", () => {
				Parser.parse( menubar1, test1);
				Parser.parse( menubar2, test2);
				Parser.parse( menubar3, test3);
				box.pack_start_defaults(menubar1);
				box.pack_start_defaults(menubar2);
				box.pack_start_defaults(menubar3);
				window.add(box);
				box.pack_direction = PackDirection.TTB;
				window.destroy += Gtk.main_quit;
				window.show_all();
				Gtk.main();
			});
			/*
			add("Overflown", () => {
				Parser.parse( menubar, test1);
				menubar.gravity = Gravity.UP;
				window.add(menubar);
				window.set_size_request(10, 10);
				window.destroy += Gtk.main_quit;
				window.show_all();
				assert(menubar.overflown == true);
				Gtk.main();
			});
			*/
		}	
		public static int main (string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestBox();
			t.run();
			return 0;
		}
	}
}
