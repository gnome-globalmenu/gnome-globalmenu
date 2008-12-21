using Gtk;
using Gnomenu;


namespace Gnomenu {
	class TestLabel: TestMan {
		MenuLabel label;
		TestLabel () {
			base("/MenuLabel");
			add("init", () => {
				label = new MenuLabel();
				label.label = "hello";
				label.accel = "Ctrl_L";
				label.visible = true;
				window.add(label);
			});
			add("usability", () => {
				window.show();		
				Gtk.main();
			});
			add("Gravity", () => {
				label.gravity = Gravity.LEFT;
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
