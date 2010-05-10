
public static class Plugin {
	public static bool disabled = false;
	private static bool tiny_mode = false;
	[CCode (cname="g_module_check_init")]
	public static string? g_module_load(Module module) {
		string app_name = Environment.get_prgname();
		if(app_name != "gnome-panel") {
			disabled = true;
			return null;
		}
		module.make_resident();
		return null;
	}
	private static TypeClass klass;
	private static Type panel_menu_bar_type;
	[CCode (cname="gtk_module_init")]
	public static void gtk_module_init([CCode (array_length_pos = 0.9)] ref string[] args) {

		if(disabled) return;

		module = Module.open(null, 0);
		panel_menu_bar_type = panel_menu_bar_get_type();
		klass = panel_menu_bar_type.class_ref();
		Signal.add_emission_hook(Signal.lookup("hierarchy-changed", panel_menu_bar_type),
			0, hierarchy_changed, null);
		
		hack_all();
		Log.set_handler ("libgnomenu", LogLevelFlags.LEVEL_DEBUG, void_log_handler);
	}

	private static void void_log_handler(string? domain, LogLevelFlags level, string message) {
		return;
	}
	private static bool hierarchy_changed(SignalInvocationHint ihint,
		[CCode (array_length_pos = 1.9)]
		Value[] param_values) {
		Object object = param_values[0].get_object();
		if(!object.get_type().is_a(panel_menu_bar_type)) return true;
		message("hierarch_changed! %s", object.get_type().name());
		hack(object as Gtk.MenuBar);
		return true;
	}

	private static void hack(Gtk.MenuBar widget) {
		if(widget.get_data<bool>("hacked") == true) {
			return;
		}
		widget.set_data("hacked", true);
		message("hacked");
		if(tiny_mode) {
			var item = new Gnomenu.GlobalMenuItem();
			widget.append(item);
		} else {
			var adapter = new Gnomenu.GlobalMenuAdapter(widget);
			widget.set_data_full("globalmenu-adapter", adapter.ref(), g_object_unref);
		}
	}
	private static void hack_all() {
		List<unowned Gtk.Window> toplevels = Gtk.Window.list_toplevels();
		foreach(var w in toplevels) {
			hack_all_r(w);
		}
	}
	private static void hack_all_r(Gtk.Widget w) {
		if(w.get_type().is_a(panel_menu_bar_type)) {
			hack(w as Gtk.MenuBar);
			return;
		}
		if(w is Gtk.Container) {
			var c = w as Gtk.Container;
			foreach(var child in c.get_children()) {
				hack_all_r(child);
			}
		}
	}

	private static Module module = null;

	static delegate Type GTypeFunction();

	private static Type panel_menu_bar_get_type() {
		void * pointer = null;
		module.symbol("panel_menu_bar_get_type", out pointer);
		GTypeFunction function = (GTypeFunction)pointer;
		return function();
	}
}
