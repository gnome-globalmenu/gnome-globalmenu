using Gtk;
namespace GnomenuGtk {

	class TestUnload : TestMan {
		Window window = null;
		FileChooser chooser = null;
		Button load = null;
		Button unload = null;
		Settings settings = null;
		Module module;

		static const string rel_path = "/gtk-modules/globalmenu";
		TestUnload () {
			base("/GnomenuGTK/Unload");
			settings = Settings.get_default();
			add("test", () => {
				Builder builder = new Builder();
				try {
					builder.add_from_file(Config.ABSTOPSRCDIR + rel_path +"/tests/test-unload.ui");
				} catch(GLib.Error e) {
					critical("%s", e.message);
				}
				window = builder.get_object("test_window") as Window;
				chooser = builder.get_object("chooser") as FileChooser;
				chooser.set_filename(Config.ABSTOPSRCDIR + rel_path + "/.libs/libglobalmenu-gnome.so");
				load = builder.get_object("load") as Button;
				unload = builder.get_object("unload") as Button;
				load.clicked += load_module;
				unload.clicked += unload_module;
				window.show();
				window.destroy += Gtk.main_quit;
				Gtk.main();
			});

		}
		private void load_module(Button button) {
			string module_name = chooser.get_filename();
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
		private void unload_module(Button button) {
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
