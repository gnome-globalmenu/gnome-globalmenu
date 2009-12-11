public class Gnomenu.GlobalSettings : Gnomenu.Settings {
	public Gdk.Screen screen {get; private set;}
	public new static Gnomenu.GlobalSettings get(Gdk.Screen screen) {
		Gnomenu.GlobalSettings * settings = screen.get_data("globalmenu-settings");
		if(settings != null) return settings;
		return new GlobalSettings(screen);
	}
	private GlobalSettings(Gdk.Screen? screen = null) {
		attach_to_screen(screen);
		screen.set_data_full("globalmenu-settings", this.ref(), g_object_unref);
	}

	public void attach_to_screen(Gdk.Screen? screen) {
		this.screen = screen;
		if(this.screen == null)
			attach_to_window(null);
		else
			attach_to_window(this.screen.get_root_window());
	}
}
