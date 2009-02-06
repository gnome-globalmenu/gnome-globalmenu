namespace Xlib {
	class TestXlib : TestMan {
		unowned Window window;
		unowned Display display;
		unowned PropertyEvent event;
		TestXlib() {
			base("/Xlib");
			
		}
		public static int main (string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);
			var t = new TestXlib();
			t.run();
			return 0;
		}
	}
}
