
using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestMenuBar : TestMan {
		Gtk.Window window;
		MenuBar menubar;
		Box box;
		static string test1 =
"""
<menu>
	<item id="Go">
		<menu>
			<item id="Here" type="c"/>
			<item id="There" type="i"/>
			<item id="Where?" type="r"/>
		</menu>
	</item>
	<item id="File">
		<menu>
			<item id="New"/>
			<item id="Open"/>
			<item id="Close"/>
		</menu>
	</item>
	<item id="Edit">
		<menu>
			<item id="Copy"/>
			<item id="Paste"/>
		</menu>
	</item>
	<item id="Help">
		<menu>
			<item id="About"/>
		</menu>
	</item>
	<item type="a"/>
</menu>
""";
		static string test2 =
"""
<menu>
	<item id="File" font="bold">
		<menu>
			<item id="New"/>
			<item id="Open"/>
			<item id="Close"/>
		</menu>
	</item>
	<item id="Edit">
		<menu>
			<item id="Copy"/>
			<item id="Paste"/>
			<item id="Cut"/>
			<item id="Find"/>
		</menu>
	</item>
	<item id="Help">
		<menu>
			<item id="About"/>
		</menu>
	</item>
</menu>
""";

		TestMenuBar () {
			base("/MenuBar");
			window = new Gtk.Window(WindowType.TOPLEVEL);
			Gdk.Color color;
			Gdk.Color.parse("#0000ff", out color);
			window.modify_bg(StateType.NORMAL, color);
			menubar = new MenuBar();
			menubar.activate += activate;

			add("usability", () => {
					assert(window is Gtk.Window);
				Parser.parse( menubar, test1);
				window.add(menubar);
				window.destroy += Gtk.main_quit;
				window.show_all();		
				Gtk.main();
			});
			add("Gravity", () => {
				Parser.parse( menubar, test1);
				menubar.gravity = Gravity.UP;
				window.add(menubar);
				window.destroy += Gtk.main_quit;
				window.show_all();
				Gtk.main();
			});
		}	
		public static int main (string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestMenuBar();
			t.run();
			return 0;
		}
		static void activate(MenuBar menubar, MenuItem item) {
			message("act: %s", item.path);
			Parser.parse(menubar, test2);
		}
	}

}
