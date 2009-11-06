using Gtk;
using Gnomenu;

namespace Gnomenu {
	class TestParser : TestMan {
		MenuBar shell;
		string test1 = """<menu><item><menu><item/></menu></item><item/></menu>""";
		string test2 =
"""
<menu>
	<item id="File" font="Serif Bold 20" accel="Control+L">
		<menu>
			<item id="New" label="New" type="c" state="toggled"/>
			<item id="Open" label="Open" type="r" state="untoggled"/>
			<item id="Close" label="Close" type="r" accel="Control+Q"/>
		</menu>
	</item>
	<item id="Edit">
		<menu>
			<item id="Copy" type="image" icon="wnck-stock-delete"/>
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
		TestParser () {
			base("/Parser");
			add("test1", () => {
	   			shell = new MenuBar();
				Parser.parse(shell, test1);
				Test.message("%s == %s", test1, Serializer.to_string(shell));
				assert(test1 == Serializer.to_string(shell));
			});
			add("test2/Show", () => {
	   			shell = new MenuBar();
				Parser.parse(shell, test2);
				Test.message("%s", Serializer.to_string(shell, true));
				Gtk.Window window = new Gtk.Window(WindowType.TOPLEVEL);
				window.add(shell);
				shell.visible = true;
				window.destroy += Gtk.main_quit;
				window.show_all();
				Gtk.main();
			});
			add("Reuse", () => {
	   			shell = new MenuBar();
				Parser.parse(shell, test2);
				Parser.parse(shell, test1);
				Test.message("%s == %s", test1, Serializer.to_string(shell));
				assert(test1 == Serializer.to_string(shell));
			});
			add("Evolution", () => {
	   			shell = new MenuBar();
				string test3;
				string test4;
				FileUtils.get_contents("./evo-nolabel.xml", out test3, null);
				FileUtils.get_contents("./evo.xml", out test4, null);
				Parser.parse(shell, test3);
				Parser.parse(shell, test3);
				Parser.parse(shell, test3);
				//Parser.parse(shell, test4);
				Test.message("%s", Serializer.to_string(shell, true));
				Gtk.Window window = new Gtk.Window(WindowType.TOPLEVEL);
				window.add(shell);
				shell.visible = true;
				window.destroy += Gtk.main_quit;
				window.show_all();
				Gtk.main();
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
