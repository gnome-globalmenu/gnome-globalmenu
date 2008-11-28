using Gtk;
using Gnomenu;

namespace Gnomenu {
	class TestParser : TestMan {
		MenuShell shell;
		string test1 = """<menu><item><menu><item/></menu></item><item/></menu>""";
		TestParser () {
			base("/Parser");
			add("create", () => {
	   			shell = new MenuShell();
			});
			add("parse/test1", () => {
				Parser.parse(shell, test1);
				Test.message("%s == %s", test1, Serializer.to_string(shell));
				assert(test1 == Serializer.to_string(shell));
			});
		}
	}
	public static int main (string[] args) {
		Test.init(ref args);
		var t = new TestParser();
		t.run();
		return 0;
	}
}
