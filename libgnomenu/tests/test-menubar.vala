
using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestMenuBar : TestMan {
		MenuBar menubar;
		Box box;
		static string test1 =
"""
<menu>
	<item id="Go">
		<menu>
			<item id="Here" type="c"/>
			<item id="There" type="i" icon="gtk-go-down"/>
			<item id="Where?" type="r"/>
		</menu>
	</item>
	<item id="File" type="icon" icon="gtk-file">
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

		Gdk.Color color;
		TestMenuBar () {
			base("/MenuBar");
			add("init", () => {
				Gdk.Color.parse("#0000ff", out color);
				window.modify_bg(StateType.NORMAL, color);
				menubar = new MenuBar();
		//		menubar.activate += activate;
				menubar.visible = true;
				window.add(menubar);
			});
			add("usability", () => {
					assert(window is Gtk.Window);
				Parser.parse( menubar, test1);
				menubar.min_length = -1;
				window.show();		
				Gtk.main();
			});
			add("Overflown", () => {
				Parser.parse( menubar, test1);
				Parser.parse( menubar, test1);
				menubar.modify_bg(StateType.NORMAL, color);
				menubar.gravity = Gravity.UP;
				menubar.min_length = 20;
				window.resize(20, 20);
				window.show();
				assert(menubar.overflown == true);
				Gtk.main();
			});
			add("Gravity", () => {
				Parser.parse( menubar, test1);
				menubar.gravity = Gravity.UP;
				window.show();
				Gtk.main();
			});

			add("get", () => {
				Parser.parse( menubar, test1);
				assert(menubar.get("/Go/Here") != null);
				assert(menubar.get("/Go") != null);
				assert(menubar.get("/Help/About") != null);
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
