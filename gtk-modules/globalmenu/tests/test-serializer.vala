using Gtk;

namespace GnomenuGtk {

	class TestSerializer : TestMan {
		MenuBar menubar;
		TestSerializer () {
			base("/GnomenuGTK/Serializer");

			add("test", () => {
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
			add("locator", () => {
				assert(null != Locator.locate(menubar, "123:/0"));
				assert(null != Locator.locate(menubar, "/0"));
				assert(null != Locator.locate(menubar, "/1"));
				assert(null != Locator.locate(menubar, "/2"));
				assert(null != Locator.locate(menubar, "/2/0"));
				assert(null != Locator.locate(menubar, "/2/1"));
				assert(null == Locator.locate(menubar, "/3"));
				assert(null == Locator.locate(menubar, "/3/3"));
				assert(null == Locator.locate(menubar, "/2/3"));
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
