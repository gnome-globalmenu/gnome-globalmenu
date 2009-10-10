using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestAdapter: TestMan {
		static string test1 =
"""
<menu>
	<item label="_Go" id="Go">
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
	<item id="Edit" type="image" icon="gtk-go-up">
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
		Gnomenu.MenuBar menubar;
		Gnomenu.Adapter adapter;
		TestAdapter() {
			base("/GlobalMenu");
			add("widget", () => {
				menubar = new Gnomenu.MenuBar();
				adapter = new Adapter(menubar);
				adapter.is_topmost = true;
				window.add(menubar);
				window.show_all();
				window.set_keep_above(true);
				window.set_accept_focus(false);
				Parser.parse(adapter, test1);
				Gtk.main();
			});

		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestAdapter();
			t.run();
			return 0;
		}
	}
}
