using Gtk;
using Gnomenu;

namespace Gnomenu {
	class TestMenuShell : TestMan {
		MenuShell menu;
		MenuItem item;
		TestMenuShell () {
			base("/Menushell");
			add("create", () => {
				menu = new MenuShell();

			});
			add("append", () => {
				item = new MenuItem();
				weak MenuItem ref_item = menu.append(item);
				assert(ref_item == item);
				item = null;
				assert(ref_item.ref_count == 2); /*1 from menushell, 1 from parent_set*/
				
			});
			add("set", () => {
				item = new MenuItem();
				MenuItem old_item = menu.set(0, item);
				assert(old_item.ref_count == 1);
				old_item = null;
			});
			add("get", () => {
				weak MenuItem item_ref = menu.get(0);
				assert(item_ref.ref_count == 3); /*1 from this.item, 1 from menu, 1 from parent_set*/
				assert(item_ref == item);
			});
			add("length", () => {
				assert(menu.length == 1);
			});
			add("truncate", () => {
				menu.truncate(0);
				assert(menu.length == 0);
			});

			add("batchappend", () => {
				int i;
				for(i = 0; i< 100; i++) {
					menu.append(new MenuItem());
				}
				assert(menu.length == 100);
			});

			add("truncate2", () => {
				menu.truncate(50);
				assert(menu.length == 50);
			});
		}

		public static int main (string[] args) {
			Test.init(ref args);
			var t = new TestMenuShell();
			t.run();
			return 0;
		}
	}
}
