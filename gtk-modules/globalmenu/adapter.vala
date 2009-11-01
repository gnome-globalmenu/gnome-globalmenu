public class Adapter {
	public Gtk.MenuBar menubar;
	public Adapter(Gtk.MenuBar menubar) {
		this.menubar = menubar;
	}
	public static unowned Adapter from_menubar(Gtk.MenuBar menubar) {
		Adapter * info = menubar.get_data("globalmenu-info");
		if(info != null) return info;
		info = new Adapter(menubar);
		menubar.set_data_full("globalmenu-info", info, unref_adapter);
		return info;
	}
	private static void unref_adapter(Adapter * info) {
		delete info;
	}
}
