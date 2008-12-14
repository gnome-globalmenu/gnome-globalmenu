using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestLabel: TestMan {
		Gtk.Window window;
		TestLabel () {
			base("/MenuLabel");
			window = new Gtk.Window(WindowType.TOPLEVEL);

			add("usability", () => {
				MenuLabel label = new MenuLabel();
				label.label = "hello";
				label.accel = "Ctrl_L";
				label.visible = true;
				window.add(label);
				window.destroy += Gtk.main_quit;
				window.show();		
				Gtk.main();
			});
			add("Gravity", () => {
				MenuLabel label = new MenuLabel();
				label.label = "hello";
				label.accel = "Ctrl_L";
				label.visible = true;
				label.gravity = Gravity.LEFT;
				window.add(label);
				window.destroy += Gtk.main_quit;
				window.show();		
				Gtk.main();
			});
		}	
		public static int main (string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestLabel();
			t.run();
			return 0;
		}
	}

}
