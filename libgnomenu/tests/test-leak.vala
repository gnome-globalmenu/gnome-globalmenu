using Gtk;
using Gnomenu;

namespace Gnomenu {
	class TestLeak: TestMan {
		TestLeak () {
			base("/Leak");
			add("Leak", () => {
					return;
				int i;
				int j;
				Gtk.MenuShell shell;
				Gtk.Window window = new Gtk.Window(WindowType.TOPLEVEL);
				shell = new MenuBar();
				window.add(shell);
				for(i = 0; i< 10; i++) {
//				if(i==1 || i== 99) mem_profile();
					for(j=0; j<20; j++) {
							MenuItem item = new MenuItem();
							Menu menu = new Menu();
							menu.append(new MenuItem());
							menu.append(new MenuItem());
							menu.append(new MenuItem());
							item.submenu = menu;
							shell.append(item);
					}
					/*This is leaking. Vala's gtk binding says get_children is weak
					 * but it turns out to be strong.*/
					List<weak Widget> children = shell.get_children().copy();
					foreach(weak Widget child in children) {
						shell.remove(child);
					}
				}
			});
			add("Parser", () => {
				int i;
				int j;
				Gtk.MenuShell shell;
				Gtk.Window window = new Gtk.Window(WindowType.TOPLEVEL);
				shell = new MenuBar();
				window.add(shell);
				string test3;
				FileUtils.get_contents("./evo-nolabel.xml", out test3, null);
				Parser.parse(shell as MenuBar, test3);
//				mem_profile();
				for(i = 0; i< 10; i++) {
					Parser.parse(shell as MenuBar, test3);
				}
//				mem_profile();
			});
		}
	}
	public static int main (string[] args) {
		mem_set_vtable(mem_profiler_table);
		Test.init(ref args);
		Gtk.init(ref args);
		var t = new TestLeak();
		t.run();
		return 0;
	}
}
