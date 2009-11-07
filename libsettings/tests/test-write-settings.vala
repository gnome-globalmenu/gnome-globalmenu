public int main(string[] args) {
	Gtk.init(ref args);
	Gnomenu.Settings settings = new Gnomenu.Settings(Gdk.Screen.get_default());
	settings.notify["use-global-menu"] += () => {
		message("use-global-menu changed");
	};
	settings.use_global_menu = ! settings.use_global_menu;
	settings.push();
	Gdk.flush();
	Gtk.main();

	return 0;
}
