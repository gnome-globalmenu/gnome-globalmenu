
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
	<item label="Go">
		<menu>
			<item label="Here"/>
			<item label="There"/>
			<item label="Where?"/>
		</menu>
	</item>
	<item label="File">
		<menu>
			<item label="New"/>
			<item label="Open"/>
			<item label="Close"/>
		</menu>
	</item>
	<item label="Edit">
		<menu>
			<item label="Copy"/>
			<item label="Paste"/>
		</menu>
	</item>
	<item label="Help">
		<menu>
			<item label="About"/>
		</menu>
	</item>
</menu>
""";
		static string test2 =
"""
<menu>
	<item label="File">
		<menu>
			<item label="New"/>
			<item label="Open"/>
			<item label="Close"/>
		</menu>
	</item>
	<item label="Edit">
		<menu>
			<item label="Copy"/>
			<item label="Paste"/>
			<item label="Cut"/>
			<item label="Find"/>
		</menu>
	</item>
	<item label="Help">
		<menu>
			<item label="About"/>
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
			box = new Gtk.HBox(false, 0);
			Parser.parse( menubar, test1);
			box.pack_start_defaults(new Label("hello"));
			box.pack_start_defaults(menubar);
			window.add(box);

			add("usability", () => {
					assert(window is Gtk.Window);
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
