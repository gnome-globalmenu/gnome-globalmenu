using Gtk;
namespace GnomenuGtk {

	class TestUnload : TestMan {
		Module module;

		TestUnload () {
			base("/GnomenuGTK/Unload");
			add("test", () => {
				Gtk.MenuBar menubar = new Gtk.MenuBar();
				menubar.append(new MenuItem.with_label("test1"));
				menubar.append(new MenuItem.with_label("test2"));
				menubar.append(new MenuItem.with_label("test3"));
				Gtk.Box box = new Gtk.VBox(false, 0);
				Gtk.Button button1 = new Button.with_label("Load");
				Gtk.Button button2 = new Button.with_label("UnLoad");
				window.add(box);
				box.add(menubar);
				box.add(button1);
				box.add(button2);
				window.show_all();
				button1.clicked += load_module;
				button2.clicked += unload_module;
				Gtk.main();
			});

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
