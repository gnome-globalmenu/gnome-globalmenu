using Gtk;
namespace GnomenuGtk {

	class TestUnload : TestMan {
		Module module;
		Gtk.MenuBar menubar = new Gtk.MenuBar();
		Gtk.MenuItem item = new MenuItem.with_label("test1");
		Gtk.Menu submenu = new Gtk.Menu();

		TestUnload () {
			base("/GnomenuGTK/Unload");
			add("test", () => {
				item.submenu = submenu;
				menubar.append(item);
				menubar.append(new MenuItem.with_label("test2"));
				menubar.append(new MenuItem.with_label("test3"));
				Gtk.Box box = new Gtk.VBox(false, 0);
				Gtk.Button button1 = new Button.with_label("Load");
				Gtk.Button button2 = new Button.with_label("UnLoad");
				Gtk.Button button3 = new Button.with_label("add item");
				Gtk.Button button4 = new Button.with_label("change label");
				window.add(box);
				box.add(menubar);
				box.add(button1);
				box.add(button2);
				box.add(button3);
				box.add(button4);
				window.show_all();
				button1.clicked += load_module;
				button2.clicked += unload_module;
				button3.clicked += add_item;
				button4.clicked += change_label;
				Gtk.main();
			});

		}
		private void add_item() {
			submenu.append(new MenuItem.with_label("added item"));
			submenu.show_all();
		}
		private void change_label() {
			item.set_label("changed label");
		}
		private void load_module() {
			string module_name = "../.libs/libgnomenu-gtk.so";
			if(module == null) {
				module = Module.open(module_name, 
						ModuleFlags.BIND_LOCAL | ModuleFlags.BIND_LAZY);
				if(module == null) {
					message("%s", Module.error());
					return;
				}
				ModuleInitFunc init = null;
				void* init_ptr;
				module.symbol("gtk_module_init", out init_ptr);
				init = (ModuleInitFunc) init_ptr;
				message("gtk_module_init found: %p", init_ptr);
				init(0, null);
			}
		}
		private void unload_module() {
			module = null;
		}
		public static int main(string[] args) {
			Test.init(ref args);
			Gtk.init(ref args);

			var t = new TestUnload();

			t.run();

			return 0;
		}	
	}
}
