public int main(string[] args) {
	Gtk.init(ref args);

	Gnomenu.GlobalSettings settings = new Gnomenu.GlobalSettings(Gdk.Screen.get_default());

	settings.notify["use-global-menu"] += () => {
		message("use-global-menu changed");
	};

	Gtk.main();
	return 0;
}
