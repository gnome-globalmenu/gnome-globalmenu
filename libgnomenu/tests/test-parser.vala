using Gtk;
using Gnomenu;

namespace Gnomenu {
	class TestParser : TestMan {
		MenuBar shell;
		string test1 = """<menu><item><menu><item/></menu></item><item/></menu>""";
		string test2 =
"""
<menu>
	<item id="File" font="Serif Bold 20">
		<menu>
			<item id="New" label="New" type="c" state="toggled"/>
			<item id="Open" label="Open" type="r" state="untoggled"/>
			<item id="Close" label="Close" type="r"/>
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
</menu>
""";
		TestParser () {
			base("/Parser");
			add("create", () => {
	   			shell = new MenuBar();
			});
			add("test1", () => {
				Parser.parse(shell, test1);
				Test.message("%s == %s", test1, Serializer.to_string(shell));
				assert(test1 == Serializer.to_string(shell));
			});
			add("test2/Show", () => {
				Parser.parse(shell, test2);
				Test.message("%s", Serializer.to_string(shell, true));
				Window window = new Window(WindowType.TOPLEVEL);
				window.add(shell);
				shell.visible = true;
				window.destroy += Gtk.main_quit;
				window.show_all();
				Gtk.main();
			});
			add("Reuse", () => {
				Parser.parse(shell, test2);
				Parser.parse(shell, test1);
				Test.message("%s == %s", test1, Serializer.to_string(shell));
				assert(test1 == Serializer.to_string(shell));
			});
		}
	}
	public static int main (string[] args) {
		Test.init(ref args);
		Gtk.init(ref args);
		var t = new TestParser();
		t.run();
		return 0;
	}
}
