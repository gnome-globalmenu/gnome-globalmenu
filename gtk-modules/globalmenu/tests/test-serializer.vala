using Gtk;

namespace GnomenuGtk {

	class TestSerializer : TestMan {
		TestSerializer () {
			base("/GnomenuGTK/Serializer");

			add("test", () => {
				MenuBar menubar;
				Menu menu;
				MenuItem item;

				menubar = new MenuBar();
				menu = new Menu();
				item = new MenuItem();
				menubar.append(new CheckMenuItem.with_label("File"));
				menubar.append(new ImageMenuItem.with_label("Edit"));
				menubar.append(item = new CheckMenuItem.with_label("Help"));
				(item as CheckMenuItem).draw_as_radio = true;
				item.submenu = menu;
				menu.append(new MenuItem.with_label("About"));
				menu.append(new SeparatorMenuItem());
				
				message("%s", Serializer.to_string(menubar, true));
			});
		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);

			var t = new TestSerializer();

			t.run();

			return 0;
		}	
	}
}

}
