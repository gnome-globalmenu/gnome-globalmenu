using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestBox: TestMan {
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
			add("init", () => {
				Gdk.Color color;
				Gdk.Color.parse("#0000ff", out color);
				window.modify_bg(StateType.NORMAL, color);
				menubar1 = new MenuBar();
				menubar2 = new MenuBar();
				menubar3 = new MenuBar();
				box = new Box();
				Parser.parse( menubar1, test1);
				Parser.parse( menubar2, test2);
				Parser.parse( menubar3, test3);
				box.pack_start_defaults(menubar1);
				box.pack_start_defaults(menubar2);
				box.pack_start_defaults(menubar3);
				window.add(box);
			});
			add("Basic", () => {
					assert(window is Gtk.Window);
				window.show();		
				Gtk.main();
			});
			add("Gravity", () => {
				box.gravity = Gravity.UP;
				window.show();
				Gtk.main();
			});
			add("Pack", () => {
				box.pack_direction = PackDirection.TTB;
				window.show();
				Gtk.main();
			});
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
